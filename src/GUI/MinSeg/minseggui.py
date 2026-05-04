import sys
import serial
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QTimer
from ui_form import Ui_MinSegGUI

class MinSegGUI(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.ui = Ui_MinSegGUI()
        self.ui.setupUi(self)

        # ---------------------------------------------------------
        # 1. TIMING & RTS SETUP
        # ---------------------------------------------------------
        self.h_slow = 0.05  # Sampling interval h = 50ms
        self.k = 0          # Step counter for discrete time kh

        self.time_data = []
        self.angle_data = []

        # Setup plot on graph_1 (Yellow line)
        self.curve_angle = self.ui.graph_1.plot(pen='y')

        # ---------------------------------------------------------
        # 2. HARDWARE (COMMUNICATION BUS)
        # ---------------------------------------------------------
        try:
            self.bt = serial.Serial('/dev/cu.usbmodem31201', 115200, timeout=0.01)
        except serial.SerialException:
            print("Warning: Arduino not connected on /dev/cu.usbmodem31201")

        # ---------------------------------------------------------
        # 3. SIGNAL-SLOT CONNECTIONS
        # ---------------------------------------------------------
        self.ui.btn_start.clicked.connect(self.send_start)
        self.ui.btn_stop.clicked.connect(self.send_stop)

        # Connect the slider to send the reference 'r'
        self.ui.Slider_k.valueChanged.connect(self.send_reference)

        # ---------------------------------------------------------
        # 4. START THE LOOP
        # ---------------------------------------------------------
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_system)
        self.timer.start(int(self.h_slow * 1000))

    def send_start(self):
        if hasattr(self, 'bt'): self.bt.write(b"START\n")

    def send_stop(self):
        if hasattr(self, 'bt'): self.bt.write(b"STOP\n")

    def send_reference(self, value):
        """Sends the slider value as reference r to the Arduino"""
        if hasattr(self, 'bt'):
            # Sending value as a string: "REF:10\n"
            ref_msg = f"REF:{value}\n"
            self.bt.write(ref_msg.encode())

    def update_system(self):
        # Read the discrete measurement y(kh)
        if hasattr(self, 'bt') and self.bt.in_waiting > 0:
            try:
                raw_text = self.bt.readline().decode('utf-8').strip()
                data_list = raw_text.split(',')

                if len(data_list) == 4:
                    # Map the raw data to state variables
                    status = data_list[0]
                    angle  = float(data_list[1])
                    rate   = float(data_list[2])
                    pwm    = float(data_list[3])

                    # Update your UI Labels with the new objectNames
                    self.ui.label_status.setText(f"Status: {status}")
                    self.ui.label_angle.setText(f"Angle: {angle:.1f}°")
                    self.ui.label_pwm.setText(f"PWM: {pwm:.0f}")
                    self.ui.label_rate.setText(f"Rate: {rate:.1f}°/s")

                    # Log data for the stability plot
                    current_t = self.k * self.h_slow
                    self.time_data.append(current_t)
                    self.angle_data.append(angle)

                    # Update the plot to visualize state trajectory
                    self.curve_angle.setData(self.time_data[-100:], self.angle_data[-100:])
                    self.k += 1

            except (ValueError, IndexError):
                pass

if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = MinSegGUI()
    widget.show()
    sys.exit(app.exec())