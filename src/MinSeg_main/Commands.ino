void handleSerialCommands() {
  if (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim(); // Remove any hidden \r or spaces

    if (cmd == "START") {
      enableBalancing(); // Restarts the process and resets encoders
    } 
    else if (cmd == "STOP") {
      disableBalancing(); // Stops motors immediately
    }
    else if (cmd.startsWith("REF:")) {
      // Logic for slider if you choose to use it
      String valuePart = cmd.substring(4);
      float refValue = valuePart.toFloat();
      // reference_r = valuePart.toFloat(); 
    }
  }
}

/*
int forwardPWM = 120;
int backwardPWM = 140; // tune this separately

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
       // disableBalancing();
        Serial.println("STOP");
        break;

      case 'f':
      case 'F':
        disableBalancing();
        setMotorPWM(forwardPWM);
        Serial.print("forward PWM = ");
        Serial.println(forwardPWM);
        break;

      case 'g':
      case 'G':
      enableBalancing();
      break;
      
      case 'b':
      case 'B':
      disableBalancing();
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
*/