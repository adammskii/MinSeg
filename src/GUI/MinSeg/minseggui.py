import sys
import argparse
import serial
from serial.tools import list_ports

from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QTimer
from ui_form import Ui_MinSegGUI


# Use 9600 if you are using Communication.ino / Bluetooth HC-05/HC-06 default.
# Use 115200 if you are using the old USB print code in MinSeg_main.ino.
DEFAULT_BAUD = 9600


def find_serial_port():
    ports = sorted(list_ports.comports(), key=lambda p: p.device)

    if not ports:
        print("No serial ports found.")
        return None

    print("Available serial ports:")
    for p in ports:
        print(f"  {p.device}: {p.description} [{p.hwid}]")

    # Prefer Bluetooth first
    bluetooth_keywords = [
        "bluetooth",
        "standard serial over bluetooth",
        "bthmodem",
        "hc-05",
        "hc-06",
        "zs-040",
    ]

    for p in ports:
        text = f"{p.device} {p.description} {p.hwid}".lower()
        if any(keyword in text for keyword in bluetooth_keywords):
            return p.device

    # Then try Arduino / USB-ish ports
    usb_keywords = [
        "arduino",
        "mega",
        "usb serial",
        "usb-serial",
        "ch340",
    ]

    for p in ports:
        text = f"{p.device} {p.description} {p.hwid}".lower()
        if any(keyword in text for keyword in usb_keywords):
            return p.device

    # Last fallback: first available port
    return ports[0].device


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--port", default=None, help="Serial port, e.g. COM7")
    parser.add_argument("--baud", type=int, default=DEFAULT_BAUD, help="Baud rate")
    return parser.parse_known_args()


class MinSegGUI(QMainWindow):
    def __init__(self, port=None, baud=DEFAULT_BAUD, parent=None):
        super().__init__(parent)
        self.ui = Ui_MinSegGUI()
        self.ui.setupUi(self)

        # ---------------------------------------------------------
        # 1. TIMING
        # ---------------------------------------------------------
        self.h_slow = 0.05  # 50 ms
        self.k = 0

        self.time_data = []
        self.angle_data = []

        self.curve_angle = self.ui.graph_1.plot(pen="y")

        # ---------------------------------------------------------
        # 2. SERIAL / BLUETOOTH
        # ---------------------------------------------------------
        self.bt = None
        selected_port = port or find_serial_port()

        if selected_port is None:
            print("Warning: No Arduino/Bluetooth serial port found.")
            self.ui.label_status.setText("Status: No serial port")
        else:
            try:
                self.bt = serial.Serial(
                    port=selected_port,
                    baudrate=baud,
                    timeout=0.01,
                    write_timeout=0.1,
                )
                print(f"Connected to {selected_port} at {baud} baud")
                self.ui.label_status.setText(f"Status: Connected to {selected_port}")
            except (serial.SerialException, OSError) as e:
                print(f"Warning: Could not open {selected_port}: {e}")
                self.bt = None
                self.ui.label_status.setText("Status: Serial connection failed")

        # ---------------------------------------------------------
        # 3. BUTTONS / SLIDER
        # ---------------------------------------------------------
        self.ui.btn_start.clicked.connect(self.send_start)
        self.ui.btn_stop.clicked.connect(self.send_stop)
        self.ui.Slider_k.valueChanged.connect(self.send_reference)

        # ---------------------------------------------------------
        # 4. UPDATE LOOP
        # ---------------------------------------------------------
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_system)
        self.timer.start(int(self.h_slow * 1000))

    def send_line(self, line):
        if self.bt is None:
            print(f"Not connected. Could not send: {line}")
            return

        try:
            self.bt.write((line + "\n").encode("utf-8"))
            print(f"Sent: {line}")
        except (serial.SerialException, OSError) as e:
            print(f"Serial write error: {e}")
            self.ui.label_status.setText("Status: Serial write error")

    def send_start(self):
        self.send_line("START")

    def send_stop(self):
        self.send_line("STOP")

    def send_reference(self, value):
        # Communication.ino expects commands like:
        # SET ref 10
        self.send_line(f"SET ref {value}")

    def update_system(self):
        if self.bt is None:
            return

        try:
            while self.bt.in_waiting > 0:
                raw_text = self.bt.readline().decode("utf-8", errors="ignore").strip()

                if not raw_text:
                    return

                self.handle_serial_line(raw_text)

        except (serial.SerialException, OSError) as e:
            print(f"Serial read error: {e}")
            self.ui.label_status.setText("Status: Serial read error")

    def handle_serial_line(self, raw_text):
        data_list = raw_text.split(",")

        try:
            # New Communication.ino format:
            # D,time_ms,theta,thetaDot,encoderCount,motorCommand,controllerEnabled
            if len(data_list) == 7 and data_list[0] == "D":
                time_ms = float(data_list[1])
                angle = float(data_list[2])
                rate = float(data_list[3])
                encoder_count = int(data_list[4])
                pwm = float(data_list[5])
                enabled = int(data_list[6])

                status = "BALANCING" if enabled == 1 else "STOPPED"

            # Old MinSeg_main.ino format:
            # BALANCING,angle,rate,pwm
            # STOPPED,angle,rate,pwm
            elif len(data_list) == 4:
                status = data_list[0]
                angle = float(data_list[1])
                rate = float(data_list[2])
                pwm = float(data_list[3])
                time_ms = self.k * self.h_slow * 1000.0

            else:
                # These are normal non-data messages from Arduino:
                # READY, OK START, OK STOP, BALANCING ENABLED, etc.
                print(f"Ignored line: {raw_text}")

                if raw_text == "READY":
                    self.ui.label_status.setText("Status: Arduino ready")
                elif raw_text.startswith("OK"):
                    self.ui.label_status.setText(f"Status: {raw_text}")
                elif raw_text.startswith("ERR"):
                    self.ui.label_status.setText(f"Status: {raw_text}")

                return

            self.ui.label_status.setText(f"Status: {status}")
            self.ui.label_angle.setText(f"Angle: {angle:.1f}°")
            self.ui.label_pwm.setText(f"PWM: {pwm:.0f}")
            self.ui.label_rate.setText(f"Rate: {rate:.1f}°/s")

            current_t = time_ms / 1000.0
            self.time_data.append(current_t)
            self.angle_data.append(angle)

            self.curve_angle.setData(self.time_data[-100:], self.angle_data[-100:])
            self.k += 1

        except (ValueError, IndexError) as e:
            print(f"Serial parse error: {e}")
            print(f"Raw line was: {raw_text}")


if __name__ == "__main__":
    args, _ = parse_args()

    app = QApplication(sys.argv)
    widget = MinSegGUI(port=args.port, baud=args.baud)
    widget.show()
    sys.exit(app.exec())