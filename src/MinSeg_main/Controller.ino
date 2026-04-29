// Controller.ino

bool balancingEnabled = false;

float K_angle = 8.0;
float K_rate  = 0.4;

const int MAX_BALANCE_PWM = 180;

const float START_ANGLE_LIMIT = 8.0;   // degrees
const float FALL_ANGLE_LIMIT  = 25.0;  // degrees

void enableBalancing() {
  float angle = getTiltAngle();

  if (fabs(angle) > START_ANGLE_LIMIT) {
    Serial.println("Cannot start balance: angle too large");
    stopMotor();
    balancingEnabled = false;
    return;
  }

  resetEncoder();
  balancingEnabled = true;
  Serial.println("BALANCING ENABLED");
}

void disableBalancing() {
  balancingEnabled = false;
  stopMotor();
  Serial.println("BALANCING DISABLED");
}

bool isBalancingEnabled() {
  return balancingEnabled;
}

void updateBalanceController() {
  if (!balancingEnabled) {
    return;
  }

  float angle = getTiltAngle();
  float rate  = getTiltRate();

  // Safety cutoff
  if (fabs(angle) > FALL_ANGLE_LIMIT) {
    balancingEnabled = false;
    stopMotor();
    Serial.println("FALL DETECTED - MOTOR STOPPED");
    return;
  }

  // Your sign logic:
  // positive angle -> negative PWM -> direction b
  // negative angle -> positive PWM -> direction f
  float u = -(K_angle * angle + K_rate * rate);

  u = constrain(u, -MAX_BALANCE_PWM, MAX_BALANCE_PWM);

  setMotorPWM((int)u);
}