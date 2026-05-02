// Encoder.ino

const int ENC_A_PIN = 2;
const int ENC_B_PIN = 3;

const float ENCODER_COUNTS_PER_REV = 720.0;

// Speed estimation
const unsigned long SPEED_SAMPLE_US = 5000; // 5 ms
const float RPM_FILTER_ALPHA = 0.80;         // higher = smoother

volatile long encoderCount = 0;

long lastSpeedCount = 0;
unsigned long lastSpeedTime = 0;

float wheelRPM_raw = 0.0;
float wheelRPM_filtered = 0.0;

void initEncoder() {
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), encoderISR_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), encoderISR_B, CHANGE);

  encoderCount = 0;
  lastSpeedCount = 0;
  lastSpeedTime = micros();

  wheelRPM_raw = 0.0;
  wheelRPM_filtered = 0.0;

  Serial.println("Encoder initialized");
}

void encoderISR_A() {
  bool A = digitalRead(ENC_A_PIN);
  bool B = digitalRead(ENC_B_PIN);

  if (A == B) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

void encoderISR_B() {
  bool A = digitalRead(ENC_A_PIN);
  bool B = digitalRead(ENC_B_PIN);

  if (A != B) {
    encoderCount++;
  } else {
    encoderCount--;
  }
}

long getEncoderCount() {
  noInterrupts();
  long count = encoderCount;
  interrupts();
  return count;
}

void resetEncoder() {
  noInterrupts();
  encoderCount = 0;
  interrupts();

  lastSpeedCount = 0;
  lastSpeedTime = micros();

  wheelRPM_raw = 0.0;
  wheelRPM_filtered = 0.0;
}

void updateEncoderSpeed() {
  unsigned long now = micros();

  if ((unsigned long)(now - lastSpeedTime) < SPEED_SAMPLE_US) {
    return;
  }

  float dt = (now - lastSpeedTime) / 1000000.0;

  long count = getEncoderCount();
  long delta = count - lastSpeedCount;

  float countsPerSecond = delta / dt;
  wheelRPM_raw = countsPerSecond * 60.0 / ENCODER_COUNTS_PER_REV;

  wheelRPM_filtered =
    RPM_FILTER_ALPHA * wheelRPM_filtered
    + (1.0 - RPM_FILTER_ALPHA) * wheelRPM_raw;

  lastSpeedCount = count;
  lastSpeedTime = now;
}

float getWheelRPM() {
  return wheelRPM_filtered;
}

float getWheelRPMRaw() {
  return wheelRPM_raw;
}