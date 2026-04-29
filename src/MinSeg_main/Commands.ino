int testPWM = 70; // start LOW, not 160

void handleSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    // Ignore line endings from Serial Monitor
    if (c == '\n' || c == '\r' || c == ' ') {
      continue;
    }

    switch (c) {
      case 's':
      case 'S':
        stopMotor();
        Serial.println("STOP");
        break;

      case 'f':
      case 'F':
        setMotorPWM(testPWM);
        Serial.print("forward PWM = ");
        Serial.println(testPWM);
        break;

      case 'b':
      case 'B':
        setMotorPWM(-testPWM);
        Serial.print("backward PWM = ");
        Serial.println(testPWM);
        break;

      case '+':
        testPWM = constrain(testPWM + 10, 0, 180);
        Serial.print("testPWM = ");
        Serial.println(testPWM);
        break;

      case '-':
        testPWM = constrain(testPWM - 10, 0, 180);
        Serial.print("testPWM = ");
        Serial.println(testPWM);
        break;

      case '0':
        resetEncoder();
        Serial.println("encoder reset");
        break;

      default:
        stopMotor();
        Serial.print("unknown command ");
        Serial.print(c);
        Serial.println(" -> STOP");
        break;
    }
  }
}