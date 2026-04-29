// Encoder.ino

const int ENC_A_PIN = 2;
const int ENC_B_PIN = 3;

volatile long encoderCount = 0;

long lastEncoderCount = 0;
float wheelRateCountsPerSec = 0.0;

void initEncoder() {
  pinMode(ENC_A_PIN, INPUT_PULLUP);
  pinMode(ENC_B_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_A_PIN), encoderISR_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B_PIN), encoderISR_B, CHANGE);

  encoderCount = 0;
  lastEncoderCount = 0;

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

  lastEncoderCount = 0;
  wheelRateCountsPerSec = 0.0;
}

void updateEncoder(float dt) {
  long count = getEncoderCount();
  long delta = count - lastEncoderCount;

  wheelRateCountsPerSec = delta / dt;
  lastEncoderCount = count;
}

float getWheelRateCountsPerSec() {
  return wheelRateCountsPerSec;
}