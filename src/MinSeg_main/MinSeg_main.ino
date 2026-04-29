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
  handleSerialCommands();

  unsigned long now = micros();

  if (now - lastControl >= Ts_us) {
    lastControl += Ts_us;

    updateIMU();
    updateEncoderSpeed();
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

 

  if (millis() - lastPrint >= 100) {
  lastPrint = millis();

  Serial.print("angle:");
  Serial.print(getTiltAngle());

  Serial.print("\trate:");
  Serial.print(getTiltRate());

  Serial.print("\tenc:");
  Serial.print(getEncoderCount());

  Serial.print("\trpmRaw:");
  Serial.print(getWheelRPMRaw());

  Serial.print("\trpm:");
  Serial.print(getWheelRPM());

  Serial.print("\tpwm:");
  Serial.println(getMotorPWM());
}
}