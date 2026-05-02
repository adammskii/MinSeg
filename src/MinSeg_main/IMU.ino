#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"

MPU6050 mpu;

int16_t ax, ay, az;
int16_t gx, gy, gz;

const float imuAlpha = 0.98;
const float gyroScale = 131.0; // for ±250 deg/s

float gyroOffsetX = 0.0;
float accAngleOffset = 0.0;

float tiltAngle = 0.0;
float tiltRate = 0.0;


// Kalman
float phi_ILC[4] = {0.6156, -0.0012, 0.0001, 0.9955};
float phi_L[4] = {0.3844, 0.0012, 0.0099, 0.0045};
float ONE_LC[4] = {0.6156, -0.0012, -0.0061, 0.9956};
float L[4] = {0.3844, 0.0012, 0.0061, 0.0045};

float x_k1_k[2] = {0,0};
float x_k_k[2] = {0,0};


float getRawAccAngleX() {
  return atan2((float)ay, (float)az) * 180.0 / PI;
}

void initIMU() {
  Wire.begin();
  Wire.setClock(100000);
  Wire.setWireTimeout(3000, true);

  mpu.initialize();
  mpu.setSleepEnabled(false);
  mpu.setFullScaleAccelRange(MPU6050_ACCEL_FS_2);
  mpu.setFullScaleGyroRange(MPU6050_GYRO_FS_250);

  //if (!mpu.testConnection()) {
  //  Serial.println("MPU6050 connection failed");
  //  while (1);
  //}

  Serial.println("MPU6050 connected");
}

void calibrateIMU() {
  const int N = 300;
  long sumGx = 0;
  float sumAccAngle = 0.0;

  Serial.println("Hold MinSeg still at upright angle...");
  delay(1000);

  for (int i = 0; i < N; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    sumGx += gx;
    sumAccAngle += getRawAccAngleX();

    delay(5);
  }

  gyroOffsetX = sumGx / (float)N;
  accAngleOffset = sumAccAngle / (float)N;

  tiltAngle = 0.0;
  tiltRate = 0.0;

  Serial.print("gyroOffsetX = ");
  Serial.println(gyroOffsetX);

  Serial.print("accAngleOffset = ");
  Serial.println(accAngleOffset);

  Serial.println("IMU calibration done.");
}

void updateIMU() {
  const float dt = 0.005; // same as Ts_us = 5000

  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float accAngle = getRawAccAngleX() - accAngleOffset;
  tiltRate = (gx - gyroOffsetX) / gyroScale;

  tiltAngle = imuAlpha * (tiltAngle + tiltRate * dt)
            + (1.0 - imuAlpha) * accAngle;
  kalmanIMU();
}

float getTiltAngle() {
  //return tiltAngle;
  return x_k_k[1];
}

float getTiltRate() {
  //return tiltRate;
  return x_k_k[0];
}



void kalmanIMU(){
  x_k_k[0] = ONE_LC[0]*x_k1_k[0] + ONE_LC[1]*x_k1_k[1] + phi_L[0]*tiltRate + phi_L[1]*tiltAngle;
  x_k_k[1] = ONE_LC[2]*x_k1_k[0] + ONE_LC[3]*x_k1_k[1] + phi_L[2]*tiltRate + phi_L[3]*tiltAngle;

  x_k1_k[0] = phi_ILC[0]*x_k1_k[0] + phi_ILC[1]*x_k1_k[1] + phi_L[0]*tiltRate + phi_L[1]*tiltAngle;
  x_k1_k[1] = phi_ILC[2]*x_k1_k[0] + phi_ILC[3]*x_k1_k[1] + phi_L[2]*tiltRate + phi_L[3]*tiltAngle;

}