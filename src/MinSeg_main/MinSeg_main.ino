const unsigned long Ts_us = 5000; // 5 ms = 200 Hz

unsigned long lastControl = 0;
unsigned long lastPrint = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  initIMU();
  calibrateIMU();

  initMotor();
  initEncoder();

  lastControl = micros();
}

void loop() {
  unsigned long now = micros();

  if (now - lastControl >= Ts_us) {
    lastControl += Ts_us;

    updateIMU();
    updateEncoder(0.005);
  }

  /*
  if (millis() - lastPrint >= 50) {
    lastPrint = millis();

    Serial.print("angle:");
    Serial.print(getTiltAngle());

    Serial.print("\trate:");
    Serial.println(getTiltRate());
  }
  */
  // Temporary motor test

  handleSerialCommands();

  if (millis() - lastPrint >= 50) {
    lastPrint = millis();

    Serial.print("angle:");
    Serial.print(getTiltAngle());

    Serial.print("\trate:");
    Serial.print(getTiltRate());

    Serial.print("\tenc:");
    Serial.print(getEncoderCount());

    Serial.print("\twheelRate:");
    Serial.println(getWheelRateCountsPerSec());
  }
}