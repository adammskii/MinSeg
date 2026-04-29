// Motor.ino

const int MOTOR_A_PIN = 5; // M1A
const int MOTOR_B_PIN = 4; // M1B

int currentMotorPWM = 0;

void initMotor() {
  pinMode(MOTOR_A_PIN, OUTPUT);
  pinMode(MOTOR_B_PIN, OUTPUT);
  stopMotor();
  Serial.println("Motor initialized");
}

void setMotorPWM(int u) {
  currentMotorPWM = constrain(u, -255, 255);

  if (currentMotorPWM > 0) {
    // One direction: A gets PWM, B is low
    analogWrite(MOTOR_A_PIN, currentMotorPWM);
    analogWrite(MOTOR_B_PIN, 0);
  } 
  else if (currentMotorPWM < 0) {
    // Other direction: B gets PWM, A is low
    analogWrite(MOTOR_A_PIN, 0);
    analogWrite(MOTOR_B_PIN, -currentMotorPWM);
  } 
  else {
    stopMotor();
  }
}

void stopMotor() {
  currentMotorPWM = 0;

  // Coast stop: both motor terminals low
  analogWrite(MOTOR_A_PIN, 0);
  analogWrite(MOTOR_B_PIN, 0);
}

int getMotorPWM() {
  return currentMotorPWM;
}