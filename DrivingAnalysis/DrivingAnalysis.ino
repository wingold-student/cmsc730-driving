#include <MPU9250.h>
#include <Wire.h>
MPU9250 mpu;

const static uint8_t IMU_ADDR = 0x68;
const static float GYRO_CAL = 1.0;
float gyroZSpeed = 0.0;

bool printToMonitor = false;

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
int prevSpeed = 0, currSpeed = 25;

int whiteKnuckleIncidents = 0;
int whippingIncidents = 0;

bool isDangerousTurn(int speed, float turnVel) {
  return (speed >= 65 && turnVel > 50) ||
          (speed >= 45 && turnVel > 80) ||
          (speed >= 25 && turnVel > 130);
}

void printEndForSerialMonitor() {
    Serial.println("============END============");
    Serial.print("Whipping Incidents: ");
    Serial.println(whippingIncidents);
    Serial.print("White Knuckle Incidents: ");
    Serial.println(whiteKnuckleIncidents);
    Serial.flush();
}

void printOutputForSerialMonitor(int fsrReading, GripStrength curGrip, int currSpeed, float curZGyro) {
  Serial.print("Analog reading: ");
  Serial.print(fsrReading);     // the raw analog reading
  Serial.print(" Current Speed: ");
  Serial.print(currSpeed);
  Serial.print(" ");

  Serial.print("Z Gyro (degs/s):");
  Serial.print(curZGyro);
  Serial.print(" - ");

  Serial.println(gripToString(curGrip));

  if (isDangerousTurn(currSpeed, curZGyro)) {
    Serial.println("DANGEROUS TURN DETECTED");
  }
}

void printDataAsCSV(GripStrength curGrip, int currSpeed, float curZGyro) {
  Serial.print(currSpeed);
  Serial.print(",");
  Serial.print(curGrip);
  Serial.print(",");
  Serial.print(curZGyro);
  Serial.print(",");
  if (isDangerousTurn(currSpeed, curZGyro)) {
    Serial.print(1);
  } else {
    Serial.print(0);
  }
  Serial.println("");
}

void printEndAsCSV() {
  Serial.print(whiteKnuckleIncidents);
  Serial.print(",");
  Serial.println(whippingIncidents);
  Serial.flush();
}

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(115200);
  Wire.begin();

  if (!mpu.setup(IMU_ADDR)) {
        while (1) {
            Serial.println("MPU connection failed.");
            delay(5000);
        }
    }
  if (printToMonitor) {
    Serial.println("Leave the device still...");
  }
  mpu.calibrateAccelGyro();

  if (printToMonitor) {
    Serial.println("Starting....");
  }
}
 
void loop(void) {
  if (Serial.available() > 0) {
    // printEndForSerialMonitor();
    printEndAsCSV();
    exit(0);
  }
  
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

  mpu.update();
  float curZGyro = abs(mpu.getGyroZ());

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

  // printOutputForSerialMonitor(fsrReading, curGrip, currSpeed, curZGyro);
  printDataAsCSV(curGrip, currSpeed, curZGyro);

  if (prevGrip != curGrip && curGrip == WhiteKnuckleGrip) {
    whiteKnuckleIncidents++;
  }

  if (isDangerousTurn(currSpeed, curZGyro)) {
    whippingIncidents++;
  }
  
  delay(1000);
} 
