// MinSeg_main.ino

// ---------------------------------------------------------
// Communication variables defined in Communication.ino
// ---------------------------------------------------------
extern bool cmdStartRequested;
extern bool cmdStopRequested;
extern bool cmdCalRequested;

extern bool cmdSetRequested;
extern char cmdSetName[20];
extern float cmdSetValue;

// ---------------------------------------------------------
// Timing
// ---------------------------------------------------------
const unsigned long Ts_us = 5000;  // 5 ms = 200 Hz control loop
const unsigned long Tp_ms = 50;    // 50 ms = 20 Hz telemetry/GUI update

unsigned long lastControl = 0;
unsigned long lastTelemetry = 0;

void setup() {
  // Communication.ino handles Serial.begin(COMM_BAUD)
  initCommunication();
  Serial.setTimeout(1);

  initIMU();
  calibrateIMU();

  initMotor();
  initEncoder();

  lastControl = micros();
  lastTelemetry = millis();
}

void loop() {
  // Read commands from GUI / Bluetooth / USB serial
  handleCommunication();
  applyCommunicationCommands();

  // Fast control loop
  unsigned long now = micros();

  if (now - lastControl >= Ts_us) {
    lastControl += Ts_us;

    updateIMU();
    updateEncoderSpeed();

    updateBalanceController();
  }

  // Slow telemetry loop for GUI
  unsigned long now_ms = millis();

  if (now_ms - lastTelemetry >= Tp_ms) {
    lastTelemetry = now_ms;

    sendTelemetry(
      getTiltAngle(),
      getTiltRate(),
      getEncoderCount(),
      getMotorPWM(),
      isBalancingEnabled()
    );
  }
}

void applyCommunicationCommands() {
  if (cmdStartRequested) {
    cmdStartRequested = false;
    enableBalancing();
  }

  if (cmdStopRequested) {
    cmdStopRequested = false;
    disableBalancing();
  }

  if (cmdCalRequested) {
    cmdCalRequested = false;
    disableBalancing();
    calibrateIMU();
  }

  if (cmdSetRequested) {
    cmdSetRequested = false;

    // GUI sends: SET ref <delta>
    if (strcmp(cmdSetName, "ref") == 0) {
      long delta = (long)cmdSetValue;

      if (delta != 0) {
        setBalanceStartCount(delta);
      }

      Serial.print("OK REF ");
      Serial.println(delta);
    } else {
      Serial.print("ERR unknown SET name: ");
      Serial.println(cmdSetName);
    }
  }
}