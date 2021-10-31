#include <MPU9250.h>
#include <Wire.h>
MPU9250 mpu;

const static uint8_t IMU_ADDR = 0x68;
const static byte Z_H_GYRO_REG = 0x47;
const static byte Z_L_GYRO_REG = 0x48;
const static float GYRO_SENS = 131.0;
const static float GYRO_CAL = 1.0;
float gyroZSpeed = 0.0;

int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
bool inc = true;
int prevSpeed = 0, currSpeed = 0;

int whiteKnuckleIncidents = 0;
int whippingIncidents = 0;

int16_t toTwoByte(byte high, byte low) {
  return high << 8 | low;
}

int16_t readTwoByteRegs(byte high_reg, byte low_reg) {
  Wire.beginTransmission(IMU_ADDR);
  Wire.write(high_reg);
  Wire.endTransmission();

  Wire.requestFrom(IMU_ADDR, 1);
  byte high_b = Wire.read();

  Wire.beginTransmission(IMU_ADDR);
  Wire.write(low_reg);
  Wire.endTransmission();
  
  Wire.requestFrom(IMU_ADDR, 1);
  byte low_b = Wire.read();
  
  return toTwoByte(high_b, low_b);
}

float readZGyro() {
  int16_t raw_gyro = readTwoByteRegs(Z_H_GYRO_REG, Z_L_GYRO_REG);
  return raw_gyro / GYRO_SENS;
}

bool isDangerousTurn(int speed, float turnVel) {
  return (speed >= 65 && turnVel > 50) ||
          (speed >= 45 && turnVel > 80) ||
          (speed >= 25 && turnVel > 130);
}

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.setup(IMU_ADDR)) {  // change to your own address
        while (1) {
            Serial.println("MPU connection failed.");
            delay(5000);
        }
    }
  Serial.println("Please leave the device still on the flat plane.");
  mpu.calibrateAccelGyro();
  Serial.println("Starting....");
}
 
void loop(void) {
  if(currSpeed >= 85)
  {
    inc = false;
  }
  else if(currSpeed == 0)
  {
    inc = true;
  }
  
  if(inc){
    currSpeed++;
  }
  else
  {
    currSpeed--;
  }

  fsrReading = analogRead(fsrPin);
  int i = 0;
  
  Serial.print("Analog reading: ");
  Serial.print(fsrReading);     // the raw analog reading
  Serial.print(" Current Speed: ");
  Serial.print(currSpeed);
  Serial.print(" ");

  mpu.update();
  float curZGyro = abs(mpu.getGyroZ());
  Serial.print("Z Gyro (degs/s):");
  Serial.print(curZGyro);
  Serial.print(" ");
  
  // We'll have a few threshholds, qualitatively determined
  if (fsrReading < 550) {
    Serial.println(" - Not Holding Wheel");
  } else if (fsrReading < 600) {
    Serial.println(" - Lightly Holding Wheel");
  } else if (fsrReading < 720) {
    Serial.println(" - Normal Holding Wheel");
  } else if (fsrReading < 800) {
    Serial.println(" - Gripping Wheel");
  } else {
    Serial.println(" - White Knuckling");
  }

  if (isDangerousTurn(currSpeed, curZGyro)) {
    Serial.println("DANGEROUS TURN DETECTED"); 
  }
  
  delay(200);
} 
