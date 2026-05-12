// Communication.ino
// For MinSeg shield Bluetooth port using Serial at 9600 baud

const unsigned long COMM_BAUD = 9600;
const unsigned long TELEMETRY_PERIOD_MS = 50; // 20 Hz

char rxBuffer[80];
byte rxIndex = 0;
unsigned long lastTelemetryTime = 0;

// These are command request flags.
// Your main loop can react to these.
bool cmdStartRequested = false;
bool cmdStopRequested = false;
bool cmdCalRequested = false;

bool cmdSetRequested = false;
char cmdSetName[20];
float cmdSetValue = 0.0;

void initCommunication() {
  Serial.begin(COMM_BAUD);
  delay(500);
  Serial.println("READY");
}

void handleCommunication() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      rxBuffer[rxIndex] = '\0';
      processCommand(rxBuffer);
      rxIndex = 0;
    } else {
      if (rxIndex < sizeof(rxBuffer) - 1) {
        rxBuffer[rxIndex++] = c;
      } else {
        rxIndex = 0;
        Serial.println("ERR command too long");
      }
    }
  }
}

void processCommand(char* cmd) {
  if (strcmp(cmd, "START") == 0) {
    cmdStartRequested = true;
    Serial.println("OK START");
    return;
  }

  if (strcmp(cmd, "STOP") == 0) {
    cmdStopRequested = true;
    Serial.println("OK STOP");
    return;
  }

  if (strcmp(cmd, "CAL") == 0) {
    cmdCalRequested = true;
    Serial.println("OK CAL");
    return;
  }

  if (strcmp(cmd, "GET") == 0) {
    Serial.println("OK GET");
    return;
  }

  // Example: SET Ktheta -12.5
  char name[20];
  float value;

  if (sscanf(cmd, "SET %19s %f", name, &value) == 2) {
    strcpy(cmdSetName, name);
    cmdSetValue = value;
    cmdSetRequested = true;

    Serial.print("OK SET ");
    Serial.print(cmdSetName);
    Serial.print(" ");
    Serial.println(cmdSetValue, 6);
    return;
  }

  Serial.print("ERR unknown command: ");
  Serial.println(cmd);
}

void sendTelemetry(float theta, float thetaDot, long encoderCount, int motorCommand, bool controllerEnabled) {
  unsigned long now = millis();

  if (now - lastTelemetryTime < TELEMETRY_PERIOD_MS) {
    return;
  }

  lastTelemetryTime = now;

  // Format:
  // D,time_ms,theta,thetaDot,encoderCount,motorCommand,controllerEnabled
  Serial.print("D,");
  Serial.print(now);
  Serial.print(",");
  Serial.print(theta, 4);
  Serial.print(",");
  Serial.print(thetaDot, 4);
  Serial.print(",");
  Serial.print(encoderCount);
  Serial.print(",");
  Serial.print(motorCommand);
  Serial.print(",");
  Serial.println(controllerEnabled ? 1 : 0);
}