"""
MinSeg GUI — real-time monitoring and control for the FRTN01 MinSeg.

Connects to the Arduino over a serial port (USB or HC-06 Bluetooth virtual COM)
and:
  * parses the telemetry line printed by MinSeg_main.ino:
        angle:..  rate:..  enc:..  rpmRaw:..  rpm:..  pwm:..
  * plots tilt angle, tilt rate, wheel RPM and motor PWM in real time,
  * sends the single-character commands handled by Commands.ino:
        s  stop motor + disable balancing
        g  enable balancing
        f  open-loop forward at forwardPWM
        b  open-loop backward at backwardPWM
        +  forwardPWM/backwardPWM += 5
        -  forwardPWM/backwardPWM -= 5
        0  reset encoder.

Install dependencies once:
    pip install pyqt5 pyqtgraph pyserial

Then run:
    python minseg_gui.py
"""

import re
import sys
import time
from collections import deque

import serial
import serial.tools.list_ports
from PyQt5 import QtCore, QtWidgets
import pyqtgraph as pg


# -----------------------------------------------------------------------------
# Configuration
# -----------------------------------------------------------------------------

BAUD = 115200
PLOT_WINDOW_SEC = 10.0     # rolling window length shown in the plots
TELEM_RATE_HZ   = 10       # MinSeg_main prints every 100 ms => 10 Hz
BUFFER_SIZE     = int(PLOT_WINDOW_SEC * TELEM_RATE_HZ * 4)  # generous slack

# Telemetry line regex. Matches:
#   angle:-1.23\trate:0.45\tenc:-12\trpmRaw:0.00\trpm:0.00\tpwm:0
TELEM_RE = re.compile(
    r"angle:\s*(-?\d+\.?\d*)\s+"
    r"rate:\s*(-?\d+\.?\d*)\s+"
    r"enc:\s*(-?\d+)\s+"
    r"rpmRaw:\s*(-?\d+\.?\d*)\s+"
    r"rpm:\s*(-?\d+\.?\d*)\s+"
    r"pwm:\s*(-?\d+)"
)


# -----------------------------------------------------------------------------
# Serial reader thread
# -----------------------------------------------------------------------------

class SerialReader(QtCore.QThread):
    """Reads lines from the serial port and emits parsed telemetry samples."""

    sample   = QtCore.pyqtSignal(float, float, float, int, float, float, int)
    raw_line = QtCore.pyqtSignal(str)
    error    = QtCore.pyqtSignal(str)

    def __init__(self, port, baud=BAUD):
        super().__init__()
        self.port = port
        self.baud = baud
        self._stop = False
        self._serial = None

    def run(self):
        try:
            self._serial = serial.Serial(self.port, self.baud, timeout=0.1)
            # Many Arduinos reset on connect; give the bootloader a moment.
            time.sleep(2.0)
            self._serial.reset_input_buffer()
        except Exception as e:
            self.error.emit(f"Could not open {self.port}: {e}")
            return

        t0 = time.time()
        while not self._stop:
            try:
                raw = self._serial.readline()
            except Exception as e:
                self.error.emit(f"Read error: {e}")
                break
            if not raw:
                continue

            line = raw.decode("utf-8", errors="ignore").strip()
            if not line:
                continue

            self.raw_line.emit(line)

            m = TELEM_RE.search(line)
            if m:
                angle   = float(m.group(1))
                rate    = float(m.group(2))
                enc     = int(m.group(3))
                rpm_raw = float(m.group(4))
                rpm     = float(m.group(5))
                pwm     = int(m.group(6))
                t       = time.time() - t0
                self.sample.emit(t, angle, rate, enc, rpm_raw, rpm, pwm)

        if self._serial and self._serial.is_open:
            self._serial.close()

    def send(self, text):
        """Send a short string (typically one character) to the Arduino."""
        if self._serial and self._serial.is_open:
            try:
                self._serial.write(text.encode("ascii"))
            except Exception as e:
                self.error.emit(f"Write error: {e}")

    def stop(self):
        self._stop = True


# -----------------------------------------------------------------------------
# Main window
# -----------------------------------------------------------------------------

class MainWindow(QtWidgets.QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("MinSeg Control")
        self.resize(1250, 820)

        self.reader = None

        # rolling buffers for the plots
        self.t_buf     = deque(maxlen=BUFFER_SIZE)
        self.angle_buf = deque(maxlen=BUFFER_SIZE)
        self.rate_buf  = deque(maxlen=BUFFER_SIZE)
        self.rpm_buf   = deque(maxlen=BUFFER_SIZE)
        self.pwm_buf   = deque(maxlen=BUFFER_SIZE)

        self._build_ui()

        # repaint plots at 20 Hz, independent of telemetry rate
        self.plot_timer = QtCore.QTimer(self)
        self.plot_timer.timeout.connect(self._update_plots)
        self.plot_timer.start(50)

    # ---------- UI construction ----------

    def _build_ui(self):
        central = QtWidgets.QWidget()
        self.setCentralWidget(central)
        root = QtWidgets.QHBoxLayout(central)

        # ------- left column: controls -------
        left = QtWidgets.QVBoxLayout()
        left_w = QtWidgets.QWidget()
        left_w.setLayout(left)
        left_w.setFixedWidth(330)
        root.addWidget(left_w)

        # connection
        conn_box = QtWidgets.QGroupBox("Connection")
        cl = QtWidgets.QGridLayout(conn_box)
        cl.addWidget(QtWidgets.QLabel("Port:"), 0, 0)
        self.port_combo = QtWidgets.QComboBox()
        cl.addWidget(self.port_combo, 0, 1)
        self.refresh_btn = QtWidgets.QPushButton("⟳")
        self.refresh_btn.setMaximumWidth(36)
        self.refresh_btn.setToolTip("Rescan serial ports")
        self.refresh_btn.clicked.connect(self._refresh_ports)
        cl.addWidget(self.refresh_btn, 0, 2)
        self.connect_btn = QtWidgets.QPushButton("Connect")
        self.connect_btn.clicked.connect(self._toggle_connection)
        cl.addWidget(self.connect_btn, 1, 0, 1, 3)
        left.addWidget(conn_box)
        self._refresh_ports()

        # live values
        live_box = QtWidgets.QGroupBox("Live values")
        lv = QtWidgets.QFormLayout(live_box)
        big = QtWidgets.QApplication.font()
        big.setPointSize(big.pointSize() + 2)
        big.setBold(True)
        self.angle_lbl = QtWidgets.QLabel("—")
        self.rate_lbl  = QtWidgets.QLabel("—")
        self.enc_lbl   = QtWidgets.QLabel("—")
        self.rpm_lbl   = QtWidgets.QLabel("—")
        self.pwm_lbl   = QtWidgets.QLabel("—")
        for w in (self.angle_lbl, self.rate_lbl, self.enc_lbl,
                  self.rpm_lbl, self.pwm_lbl):
            w.setFont(big)
        lv.addRow("Angle (°):",      self.angle_lbl)
        lv.addRow("Rate (°/s):",     self.rate_lbl)
        lv.addRow("Encoder count:",  self.enc_lbl)
        lv.addRow("Wheel RPM:",      self.rpm_lbl)
        lv.addRow("Motor PWM:",      self.pwm_lbl)
        left.addWidget(live_box)

        # commands
        cmd_box = QtWidgets.QGroupBox("Commands")
        cg = QtWidgets.QGridLayout(cmd_box)

        self.balance_btn = QtWidgets.QPushButton("Enable balancing  (g)")
        self.balance_btn.setStyleSheet(
            "background-color:#2e7d32; color:white; font-weight:bold; padding:8px;")
        self.balance_btn.clicked.connect(lambda: self._send("g"))
        cg.addWidget(self.balance_btn, 0, 0, 1, 2)

        self.stop_btn = QtWidgets.QPushButton("STOP  (s)")
        self.stop_btn.setStyleSheet(
            "background-color:#c62828; color:white; font-weight:bold; padding:8px;")
        self.stop_btn.clicked.connect(lambda: self._send("s"))
        cg.addWidget(self.stop_btn, 1, 0, 1, 2)

        self.fwd_btn = QtWidgets.QPushButton("Forward  (f)")
        self.fwd_btn.clicked.connect(lambda: self._send("f"))
        cg.addWidget(self.fwd_btn, 2, 0)

        self.bwd_btn = QtWidgets.QPushButton("Backward  (b)")
        self.bwd_btn.clicked.connect(lambda: self._send("b"))
        cg.addWidget(self.bwd_btn, 2, 1)

        self.minus_btn = QtWidgets.QPushButton("PWM −5")
        self.minus_btn.clicked.connect(lambda: self._send("-"))
        cg.addWidget(self.minus_btn, 3, 0)

        self.plus_btn = QtWidgets.QPushButton("PWM +5")
        self.plus_btn.clicked.connect(lambda: self._send("+"))
        cg.addWidget(self.plus_btn, 3, 1)

        self.reset_btn = QtWidgets.QPushButton("Reset encoder  (0)")
        self.reset_btn.clicked.connect(lambda: self._send("0"))
        cg.addWidget(self.reset_btn, 4, 0, 1, 2)

        left.addWidget(cmd_box)

        # serial log
        log_box = QtWidgets.QGroupBox("Serial log")
        ll = QtWidgets.QVBoxLayout(log_box)
        self.log = QtWidgets.QPlainTextEdit()
        self.log.setReadOnly(True)
        self.log.setMaximumBlockCount(500)
        mono = self.log.font()
        mono.setFamily("Menlo")  # Mac; falls back gracefully on Windows/Linux
        mono.setStyleHint(mono.Monospace)
        mono.setPointSize(9)
        self.log.setFont(mono)
        ll.addWidget(self.log)
        left.addWidget(log_box, 1)

        # disabled until connected
        self._set_commands_enabled(False)

        # ------- right column: plots -------
        right = QtWidgets.QVBoxLayout()
        root.addLayout(right, 1)

        pg.setConfigOptions(antialias=True, background='#1e1e1e', foreground='#dddddd')

        self.plot_angle = pg.PlotWidget(title="Tilt angle (°)")
        self.plot_angle.showGrid(x=True, y=True, alpha=0.3)
        self.plot_angle.setYRange(-30, 30)
        self.curve_angle = self.plot_angle.plot(pen=pg.mkPen('#ffd54f', width=2))
        right.addWidget(self.plot_angle)

        self.plot_rate = pg.PlotWidget(title="Tilt rate (°/s)")
        self.plot_rate.showGrid(x=True, y=True, alpha=0.3)
        self.curve_rate = self.plot_rate.plot(pen=pg.mkPen('#4dd0e1', width=2))
        right.addWidget(self.plot_rate)

        self.plot_rpm = pg.PlotWidget(title="Wheel RPM (filtered)")
        self.plot_rpm.showGrid(x=True, y=True, alpha=0.3)
        self.curve_rpm = self.plot_rpm.plot(pen=pg.mkPen('#ce93d8', width=2))
        right.addWidget(self.plot_rpm)

        self.plot_pwm = pg.PlotWidget(title="Motor PWM")
        self.plot_pwm.showGrid(x=True, y=True, alpha=0.3)
        self.plot_pwm.setYRange(-260, 260)
        self.curve_pwm = self.plot_pwm.plot(pen=pg.mkPen('#81c784', width=2))
        right.addWidget(self.plot_pwm)

    # ---------- helpers ----------

    def _refresh_ports(self):
        prev = self.port_combo.currentData()
        self.port_combo.clear()
        ports = serial.tools.list_ports.comports()
        for p in ports:
            self.port_combo.addItem(f"{p.device}   ({p.description})", p.device)
        if not ports:
            self.port_combo.addItem("(no ports detected)", None)
        # restore previous selection if still present
        if prev:
            idx = self.port_combo.findData(prev)
            if idx >= 0:
                self.port_combo.setCurrentIndex(idx)

    def _set_commands_enabled(self, on):
        for b in (self.balance_btn, self.stop_btn, self.fwd_btn, self.bwd_btn,
                  self.plus_btn, self.minus_btn, self.reset_btn):
            b.setEnabled(on)

    def _toggle_connection(self):
        if self.reader is None:
            port = self.port_combo.currentData()
            if not port:
                QtWidgets.QMessageBox.warning(
                    self, "No port", "Select a serial port first.")
                return
            self._clear_buffers()
            self.reader = SerialReader(port)
            self.reader.sample.connect(self._on_sample)
            self.reader.raw_line.connect(self._on_raw)
            self.reader.error.connect(self._on_error)
            self.reader.finished.connect(self._on_disconnect)
            self.reader.start()
            self.connect_btn.setText("Disconnect")
            self.port_combo.setEnabled(False)
            self.refresh_btn.setEnabled(False)
            self._set_commands_enabled(True)
            self.log.appendPlainText(f"[connected to {port}]")
        else:
            self.reader.stop()

    def _on_disconnect(self):
        self.reader = None
        self.connect_btn.setText("Connect")
        self.port_combo.setEnabled(True)
        self.refresh_btn.setEnabled(True)
        self._set_commands_enabled(False)
        self.log.appendPlainText("[disconnected]")

    def _on_error(self, msg):
        self.log.appendPlainText(f"[error] {msg}")

    def _send(self, ch):
        if self.reader:
            self.reader.send(ch)
            self.log.appendPlainText(f"> {ch}")

    def _on_raw(self, line):
        # Telemetry lines are noisy; only log the rest (status / debug prints).
        if not TELEM_RE.search(line):
            self.log.appendPlainText(line)

    def _on_sample(self, t, angle, rate, enc, rpm_raw, rpm, pwm):
        self.t_buf.append(t)
        self.angle_buf.append(angle)
        self.rate_buf.append(rate)
        self.rpm_buf.append(rpm)
        self.pwm_buf.append(pwm)

        self.angle_lbl.setText(f"{angle:+7.2f}")
        self.rate_lbl.setText(f"{rate:+7.2f}")
        self.enc_lbl.setText(f"{enc:d}")
        self.rpm_lbl.setText(f"{rpm:+7.2f}")
        self.pwm_lbl.setText(f"{pwm:d}")

    def _update_plots(self):
        if not self.t_buf:
            return
        # only show the last PLOT_WINDOW_SEC seconds
        t_now = self.t_buf[-1]
        t_min = t_now - PLOT_WINDOW_SEC
        # find first index inside the window
        i = 0
        for i, tv in enumerate(self.t_buf):
            if tv >= t_min:
                break
        t = list(self.t_buf)[i:]
        self.curve_angle.setData(t, list(self.angle_buf)[i:])
        self.curve_rate.setData(t,  list(self.rate_buf)[i:])
        self.curve_rpm.setData(t,   list(self.rpm_buf)[i:])
        self.curve_pwm.setData(t,   list(self.pwm_buf)[i:])

    def _clear_buffers(self):
        for b in (self.t_buf, self.angle_buf, self.rate_buf,
                  self.rpm_buf, self.pwm_buf):
            b.clear()

    def closeEvent(self, ev):
        if self.reader:
            self.reader.stop()
            self.reader.wait(1500)
        ev.accept()


# -----------------------------------------------------------------------------

def main():
    app = QtWidgets.QApplication(sys.argv)
    w = MainWindow()
    w.show()
    sys.exit(app.exec_())


if __name__ == "__main__":
    main()
