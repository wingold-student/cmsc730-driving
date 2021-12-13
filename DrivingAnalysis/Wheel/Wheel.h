#include "lib/MPU9250.h"

MPU9250 mpu;

static float gyro_bucket = 0.0;
static float prev_gyro_z = 0.0, curr_gyro_z = 0.0, gyro_diff = 0.0, gyro_acc = 0.0, gyro_max = 0.0;
static float prev_gyro_acc = 0.0, curr_gyro_acc = 0.0;
static int gyro_bucket_size = 0;
static uint32_t time_bucket = 25;

static uint32_t prev_ms = 0, curr_ms = 0, time_diff = 0;

float actual_angle = 0.0;
int whippingIncidents = 0;
bool dangerousTurnDetected = false;

void setupWheel() {
  // Wire.begin();
  delay(1000);
  if (!mpu.setup(0x68)) {  // change to your own address
      while (1) {
          Serial.println("MPU connection failed. Please check your connection with `connection_check` example.");
          delay(5000);
      }
  }
  
  // calibrate anytime you want to
  //Serial.println("Accel Gyro calibration will start in 5sec.");
  //Serial.println("Please leave the device still on the flat plane.");
  mpu.verbose(false);
  
  // Serial.println("Calibrating...");
  delay(1000);
  mpu.calibrateAccelGyro();
  
  //Serial.println("Mag calibration will start in 5sec.");
  //Serial.println("Please Wave device in a figure eight until done.");
  //delay(1000);
  //mpu.calibrateMag();

  mpu.selectFilter(QuatFilterSel::MAHONY);
  mpu.setFilterIterations(1);
  
  prev_ms = millis();
  // start_time = prev_ms;
}

void getGyroVel() {
  curr_gyro_z = mpu.getGyroZ();
  Serial.print("Vel:");
  Serial.println(curr_gyro_z);
}

void getAvgGyroVel(uint32_t time_diff) {
  if (time_diff == 0) {
    return;
  }
  
  if (time_diff >= time_bucket && gyro_bucket_size > 0) {
    prev_gyro_z = curr_gyro_z;
    curr_gyro_z = gyro_bucket / gyro_bucket_size;
    Serial.print("Z:");
    Serial.print(curr_gyro_z);
    Serial.print(", Bucket:");
    Serial.println(gyro_bucket_size);

    gyro_bucket = 0.0;
    gyro_bucket_size = 0;
    prev_ms = curr_ms;
    
  } else {
    gyro_bucket += mpu.getGyroZ();
    gyro_bucket_size += 1;
  }
}

bool updateAvgGyroAcc(uint32_t time_diff) {
  if (time_diff == 0) {
    return false;
  }

  if (time_diff >= time_bucket) {
    prev_gyro_acc = curr_gyro_acc;

    curr_gyro_acc = gyro_acc / gyro_bucket_size;

    if (abs(curr_gyro_acc) > abs(gyro_max)) {
      gyro_max = curr_gyro_acc;
    }

    gyro_bucket = 0.0;
    gyro_bucket_size = 0;

    prev_ms = curr_ms;

    // Serial.print("AvgAcc:");
    // Serial.print(curr_gyro_acc);
    // Serial.print(", Bucket:");
    // Serial.println(gyro_bucket_size);

    return true;
    
  } else {
    curr_gyro_z = mpu.getGyroZ();
    gyro_diff = curr_gyro_z - prev_gyro_z;

    float time_diff_s = time_diff / 1000.0;
    gyro_acc = gyro_diff / time_diff_s;
  
    prev_gyro_z = curr_gyro_z;
    
    gyro_bucket += gyro_acc;
    gyro_bucket_size += 1;

    return false;
  }
}


void getAccs() {
  Serial.print("X:");
  Serial.print(mpu.getAccX());
  Serial.print(", Y:");
  Serial.print(mpu.getAccY());
  Serial.print(", Z:");
  Serial.println(mpu.getAccZ());
}

int getQuadrant() {
  float xAcc = mpu.getAccX();
  float yAcc = mpu.getAccY();

  if (xAcc > 0 && yAcc <= 0) {
    return 1;
  } else if (xAcc >= 0 && yAcc > 0) {
    return 4;
  } else if (xAcc < 0 && yAcc > 0) {
    return 3;
  } else /*if (x <= 0 && y < 0*/{
    return 2;
  }
}

float getWheelAngle() {
  float pitch_angle = mpu.getPitch();
  int quad = getQuadrant();
  
  if (quad == 1) {
    actual_angle = pitch_angle;
  } else if (quad == 2) {
    actual_angle = 270. + abs(-90. - pitch_angle);
  } else if (quad == 3) {
    actual_angle = 180. + (0 - pitch_angle);
  } else {
    actual_angle = 90. + (90. - pitch_angle);
  }

  return actual_angle;
}

/** TODO:
 * Update with new acceleration reading, not velocity
 */
bool isDangerousTurn(int speed, float turnVel) {
  return (speed >= 65 && turnVel > 50) ||
          (speed >= 45 && turnVel > 80) ||
          (speed >= 25 && turnVel > 130);
}

/** TODO:
 * FIX CUT-OFFS!!!
 */
bool isDangerousTurnAcc(int speed, float turnAcc) {
  return (speed >= 65 && turnAcc > 50) ||
          (speed >= 45 && turnAcc > 80) ||
          (speed >= 25 && turnAcc > 130);
}

bool getDangerousTurn(int speed) {
  curr_ms = millis();

  time_diff = curr_ms - prev_ms;
  bool is_new_acc = updateAvgGyroAcc(time_diff);

  if (is_new_acc) {
    dangerousTurnDetected = isDangerousTurnAcc(speed, curr_gyro_acc);
    if (dangerousTurnDetected) {
      whippingIncidents++;
    }
  } else {
    dangerousTurnDetected = false;
  }

  return dangerousTurnDetected;
}
