void handleSerialCommands() {
  if (!Serial.available()) {
    return;
  }

  char c = Serial.read();

  if (c == 's') {
    stopMotor();
    Serial.println("stop");
  }

  if (c == 'f') {
    setMotorPWM(160);
    Serial.println("forward");
  }

  if (c == 'b') {
    setMotorPWM(-160);
    Serial.println("backward");
  }

  if (c == '0') {
    resetEncoder();
    Serial.println("encoder reset");
  }
}