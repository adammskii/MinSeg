const int MOTOR_PWM_PIN = 5;
const int MOTOR_DIR_PIN = 4;

int currentMotorPWM = 0;

void initMotor() {
  pinMode(MOTOR_PWM_PIN, OUTPUT);
  pinMode(MOTOR_DIR_PIN, OUTPUT);
  //TCCR3B = (TCCR3B & 0b11111000) | 0x01;
  stopMotor();
  Serial.println("Motor initialized");
}

const float FORWARD_PWM_GAIN  = 1.00;
const float BACKWARD_PWM_GAIN = 1.17;   // boost backward direction

void setMotorPWM(int u) {
  currentMotorPWM = constrain(u, -255, 255);

  if (currentMotorPWM == 0) {
    stopMotor();
    return;
  }

  if (currentMotorPWM > 0) {
    int pwm = constrain((int)(currentMotorPWM * FORWARD_PWM_GAIN), 0, 255);
    digitalWrite(MOTOR_DIR_PIN, HIGH);
    analogWrite(MOTOR_PWM_PIN, pwm);
  } else {
    int pwm = constrain((int)((-currentMotorPWM) * BACKWARD_PWM_GAIN), 0, 255);
    digitalWrite(MOTOR_DIR_PIN, LOW);
    analogWrite(MOTOR_PWM_PIN, pwm);
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