#include "lib/MPU9250.h"

MPU9250 mpu;

static float gyro_bucket = 0.0;
static float prev_gyro_z = 0.0, curr_gyro_z = 0.0, gyro_diff = 0.0, gyro_acc = 0.0, gyro_max = 0.0;
static int gyro_bucket_size = 0;
static uint32_t prev_ms = 0, curr_ms = 0, time_diff = 0, time_bucket = 25;
static uint32_t start_time = 0, time_len = 10000;

enum class Turn {
  CENTER,
  RIGHT,
  LEFT
};

static Turn curr_turn = Turn::CENTER, prev_turn = Turn::CENTER;
static float prev_angle = 0.0, curr_angle = 0.0, actual_angle = 0.0;
float pitch_angle = 0.0, theta = 0., phi = 0.;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Wire.begin();
  delay(2000);
  
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
  Serial.println("Calibrating...");
  delay(1000);
  mpu.calibrateAccelGyro();
  
  //Serial.println("Mag calibration will start in 5sec.");
  //Serial.println("Please Wave device in a figure eight until done.");
  //delay(1000);
  //mpu.calibrateMag();

  mpu.selectFilter(QuatFilterSel::MAHONY);
  mpu.setFilterIterations(1);
  
  prev_ms = millis();
  Serial.println("Starting...");
  start_time = prev_ms;
}

void doGyroVel() {
  curr_gyro_z = mpu.getGyroZ();
  Serial.print("Vel:");
  Serial.println(curr_gyro_z);
}

void doAvgGyro(uint32_t time_diff) {
  if (time_diff == 0) {
    return;
  }
  
  if (time_diff >= time_bucket && gyro_bucket_size > 0) {
    float avgGyro = gyro_bucket / gyro_bucket_size;
    Serial.print("Z:");
    Serial.print(avgGyro);
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

void doAvgGyroAcc(uint32_t time_diff) {
  if (time_diff == 0) {
    return;
  }

  if (time_diff >= time_bucket) {
    float gyro_acc_avg = gyro_acc / gyro_bucket_size;
    if (abs(gyro_acc_avg) > abs(gyro_max)) {
      gyro_max = gyro_acc_avg;
    }
    Serial.print("AvgAcc:");
    Serial.print(gyro_acc_avg);
    Serial.print(", Bucket:");
    Serial.println(gyro_bucket_size);
    
    gyro_bucket = 0.0;
    gyro_bucket_size = 0;

    prev_ms = curr_ms;
  } else {
    curr_gyro_z = mpu.getGyroZ();
    gyro_diff = curr_gyro_z - prev_gyro_z;
    float time_diff_s = time_diff / 1000.0;
    gyro_acc = gyro_diff / time_diff_s;
  
    prev_gyro_z = curr_gyro_z;
    
    gyro_bucket += gyro_acc;
    gyro_bucket_size += 1;
  }
}


void doAccs() {
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

void doWithQuadrants() {
  float pitch_angle = mpu.getPitch();
  int quad = getQuadrant();
  float acutal_angle = 0.0;
  
  if (quad == 1) {
    actual_angle = pitch_angle;
  } else if (quad == 2) {
    // actual_angle = pitch_angle + 360.0;
    actual_angle = 270. + abs(-90. - pitch_angle);
  } else if (quad == 3) {
    // actual_angle = pitch_angle + 270.0;
    actual_angle = 180. + (0 - pitch_angle);
  } else {
    // actual_angle = pitch_angle + 180.0;
    actual_angle = 90. + (90. - pitch_angle);
  }

  Serial.print("Quadrant:");
  Serial.print(quad);
  Serial.print(", Angle:");
  Serial.println(actual_angle);
}

void loop() {
  curr_ms = millis();
  
  if (mpu.update()) {
    time_diff = curr_ms - prev_ms;
    doWithQuadrants();
  }
}
