#include <MPU9250.h>
#include <Wire.h>
MPU9250 mpu;

const static uint8_t IMU_ADDR = 0x68;
const static float GYRO_CAL = 1.0;
float gyroZSpeed = 0.0;

enum GripStrength {
  NoGrip,
  LightGrip,
  NormalGrip,
  TightGrip,
  WhiteKnuckleGrip,
};
GripStrength prevGrip = NoGrip, curGrip = NoGrip;

String gripToString(GripStrength grip) {
  switch (grip) {
    case NoGrip:
      return String("No Grip");
    case LightGrip:
      return String("Light Grip");
    case NormalGrip:
      return String("Normal Grip");
    case TightGrip:
      return String("Tight Grip");
    case WhiteKnuckleGrip:
      return String("White Knuckle Grip");
    default:
      return String("Unknown Grip");
  }
}

int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
bool inc = true;
int prevSpeed = 0, currSpeed = 0;

int whiteKnuckleIncidents = 0;
int whippingIncidents = 0;

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
  Serial.print(" - ");

  prevGrip = curGrip;
  
  // We'll have a few threshholds, qualitatively determined
  if (fsrReading < 550) {
    curGrip = NoGrip;
  } else if (fsrReading < 600) {
    curGrip = LightGrip;
  } else if (fsrReading < 720) {
    curGrip = NormalGrip;
  } else if (fsrReading < 800) {
    curGrip = TightGrip;
  } else {
    curGrip = WhiteKnuckleGrip;
  }

  Serial.println(gripToString(curGrip));
  if (prevGrip != curGrip && curGrip == WhiteKnuckleGrip) {
    whiteKnuckleIncidents++;
  }

  if (isDangerousTurn(currSpeed, curZGyro)) {
    Serial.println("DANGEROUS TURN DETECTED");
    whippingIncident++;
  }
  
  delay(200);
} 
