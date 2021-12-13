#include <MPU9250.h>

MPU9250 mpu;
static uint32_t prev_ms = millis(), curr_ms = 0, time_diff = 0;
static float prev_gyro_z = 0.0, curr_gyro_z = 0.0, gyro_diff = 0.0, gyro_acc = 0.0;
static 

void setup() {
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
    //erial.println("Please leave the device still on the flat plane.");
    mpu.verbose(true);
    delay(1000);
    mpu.calibrateAccelGyro();

    //Serial.println("Mag calibration will start in 5sec.");
    //Serial.println("Please Wave device in a figure eight until done.");
    delay(1000);
    mpu.calibrateMag();

    print_calibration();
    mpu.verbose(false);
    
}

void loop() {
    if (mpu.update()) {
        curr_ms = millis();
        if (curr_ms > prev_ms + 15) {
            // print_roll_pitch_yaw();
            // print_euler();
            //print_gyro();

            time_diff = curr_ms - prev_ms;
            prev_ms = curr_ms;

            curr_gyro_z = mpu.getGyroZ();
            float z_acc = mpu.getAccZ();
            gyro_diff = curr_gyro_z - prev_gyro_z;
            gyro_acc = gyro_diff / time_diff;

            Serial.print("Vel:");
            Serial.print(curr_gyro_z);
            Serial.print(", GyroAcc:");
            Serial.print(gyro_acc);
        }
    }
}

void print_roll_pitch_yaw() {
    Serial.print("Yaw:");
    Serial.print(mpu.getYaw(), 2);
    Serial.print(", Pitch:");
    Serial.print(mpu.getPitch(), 2);
    Serial.print(", Roll:");
    Serial.println(mpu.getRoll(), 2);
}

void print_euler() {
    Serial.print("X:");
    Serial.print(mpu.getEulerX(), 2);
    Serial.print(", Y:");
    Serial.print(mpu.getEulerY(), 2);
    Serial.print(", Z:");
    Serial.println(mpu.getEulerZ(), 2);
}

void print_gyro() {
  Serial.print("X:");
  Serial.print(mpu.getGyroX(), 2);
  Serial.print(", Y:");
  Serial.print(mpu.getGyroY(), 2);
  Serial.print(", Z:");
  Serial.println(mpu.getGyroZ(), 2);
}
