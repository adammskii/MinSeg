int forwardPWM = 70;
int backwardPWM = 80; // tune this separately

void handleSerialCommands() {
  while (Serial.available() > 0) {
    char c = Serial.read();

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
        setMotorPWM(forwardPWM);
        Serial.print("forward PWM = ");
        Serial.println(forwardPWM);
        break;

      case 'b':
      case 'B':
        setMotorPWM(-backwardPWM);
        Serial.print("backward PWM = ");
        Serial.println(backwardPWM);
        break;

      case '+':
        forwardPWM = constrain(forwardPWM + 5, 0, 180);
        backwardPWM = constrain(backwardPWM + 5, 0, 180);
        Serial.print("forwardPWM = ");
        Serial.print(forwardPWM);
        Serial.print(" backwardPWM = ");
        Serial.println(backwardPWM);
        break;

      case '-':
        forwardPWM = constrain(forwardPWM - 5, 0, 180);
        backwardPWM = constrain(backwardPWM - 5, 0, 180);
        Serial.print("forwardPWM = ");
        Serial.print(forwardPWM);
        Serial.print(" backwardPWM = ");
        Serial.println(backwardPWM);
        break;

      case '0':
        resetEncoder();
        Serial.println("encoder reset");
        break;
    }
  }
}