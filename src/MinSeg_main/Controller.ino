// Controller.ino

bool balancingEnabled = true;

const int MAX_BALANCE_PWM = 200;

const float START_ANGLE_LIMIT = 8.0;   // degrees
const float FALL_ANGLE_LIMIT  = 25.0;  // degrees

// LQR gain from MATLAB.
// State order must match A:
// x = [thetaDot, theta, wheelDot, wheelAngle]
const float K_lqr[4] = {
  -11.0281,
  -62.7680,
  -0.8960,
  -0.1926
};

const float TWO_PI_F = 6.28318530718;
const float COUNTS_PER_REV = 720.0;

long balanceStartCount = 0;

float modelInputToPWM(float u) {
  // If your MATLAB input is motor voltage, use battery voltage here.
  const float U_MAX = 9.0;

  float pwm = 255.0 * u / U_MAX;

  return constrain((int)pwm, -MAX_BALANCE_PWM, MAX_BALANCE_PWM);
}

void enableBalancing() {
  float angleDeg = getTiltAngle();

  if (fabs(angleDeg) > START_ANGLE_LIMIT) {
    Serial.println("Cannot start balance: angle too large");
    stopMotor();
    balancingEnabled = false;
    return;
  }

  resetEncoder();
  balanceStartCount = getEncoderCount();

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

  float angleDeg = getTiltAngle();

  if (fabs(angleDeg) > FALL_ANGLE_LIMIT) {
    balancingEnabled = false;
    stopMotor();
    Serial.println("FALL DETECTED - MOTOR STOPPED");
    return;
  }

  // Convert Arduino measurements to model units.
  float theta    = getTiltAngle() * PI / 180.0;
  float thetaDot = getTiltRate()  * PI / 180.0;

  long encoderCounts = getEncoderCount() - balanceStartCount;

  float wheelAngle = ((float)encoderCounts / COUNTS_PER_REV) * TWO_PI_F;
  float wheelDot   = getWheelRPM() * TWO_PI_F / 60.0;

  // State order must match MATLAB A matrix:
  // x = [thetaDot, theta, wheelDot, wheelAngle]
  float u = +(K_lqr[0] * thetaDot
            + K_lqr[1] * theta
            + K_lqr[2] * wheelDot
            + K_lqr[3] * wheelAngle);

  u *= 1.0; //Gain 

  int pwm = modelInputToPWM(u);

  setMotorPWM(pwm);
}