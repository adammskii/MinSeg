// Motor.ino

const int MOTOR_PWM_PIN = 5;
const int MOTOR_DIR_PIN = 4;

void initMotor() {
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  stopMotor();
  Serial.println("Motor initialized");
}

void setMotorPWM(int u) {
  u = constrain(u, -255, 255);

  if (u >= 0) {
    digitalWrite(MOTOR_DIR_PIN, HIGH);
    analogWrite(MOTOR_PWM_PIN, u);
  } else {
    digitalWrite(MOTOR_DIR_PIN, LOW);
    analogWrite(MOTOR_PWM_PIN, -u);
  }
}

void stopMotor() {
  analogWrite(MOTOR_PWM_PIN, 0);
}