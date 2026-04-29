// Motor.ino

const int MOTOR_PWM_PIN = 5;
const int MOTOR_DIR_PIN = 4;

int currentMotorPWM = 0;

void initMotor() {
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  stopMotor();
  Serial.println("Motor initialized");
}

void setMotorPWM(int u) {
  currentMotorPWM = constrain(u, -255, 255);

  if (currentMotorPWM == 0) {
    stopMotor();
    return;
  }

  if (currentMotorPWM > 0) {
    digitalWrite(MOTOR_DIR_PIN, HIGH);
    analogWrite(MOTOR_PWM_PIN, currentMotorPWM);
  } else {
    digitalWrite(MOTOR_DIR_PIN, LOW);
    analogWrite(MOTOR_PWM_PIN, -currentMotorPWM);
  }
}

void stopMotor() {
  currentMotorPWM = 0;
  analogWrite(MOTOR_PWM_PIN, 0);
  digitalWrite(MOTOR_DIR_PIN, LOW);
}

int getMotorPWM() {
  return currentMotorPWM;
}