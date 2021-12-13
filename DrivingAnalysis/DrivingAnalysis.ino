#include "./GripStrength/GripStrength.h"
#include "./Wheel/Wheel.h"
#include "./HeartRate/HeartRate.h"

bool printToMonitor = true, printForHuman = true;

int currSpeed = 0;


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

void printData() {
  if (printForHuman) {
    Serial.print("Speed:\t")
    Serial.print(currSpeed);
    Serial.print("\t");

    Serial.print("Grip:\t");
    Serial.print(curGrip);
    Serial.print("\t");

    Serial.print("Angle:\t");
    Serial.print(actual_angle);
    Serial.print("\t");

    Serial.print("DangerousTurn:\t");
    Serial.print(dangerousTurnDetected);
    Serial.print("\t");

    Serial.print("HeartRate:\t");
    Serial.print(heartRate);
    Serial.print("\t");

    // Acceleration

    // Break

    Serial.println();
    
  } else {
    Serial.print(currSpeed);
    Serial.print(",");

    Serial.print(curGrip);
    Serial.print(",");

    Serial.print(actual_angle);
    Serial.print(",");

    Serial.print(dangerousTurnDetected);
    Serial.print(",");

    Serial.print(heartRate);
    Serial.print(",");

    // Acceleration
    Serial.print(0);
    Serial.print(",");

    // Break
    Serial.print(0);

    Serial.println();
  }
}

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600); // TODO: Can make higher if change HM-10?
  Wire.begin();

  setupHeartRate();
  setupWheel();  

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

  /**
   * @brief Get the Grip of the wheel
   * 
   * Updates curGrip value
   * Updates whiteKnuckleIncidents
   */
  getGrip();

  /**
   * @brief Get the Wheel Angle
   * Update the MPU readings
   * Updates actual_angle value, 0 to 360.0
   */
  mpu.update();
  getWheelAngle();

  /**
   * @brief Get the Heart Rate
   * 
   * Updates heartRate value from HM10
   */
  getHeartRate();

  /**
   * @brief Determines if a dangerous turn/whip was detected
   * given the vehicles current speed
   * 
   * NOTE: This averages out acceleration over potentially
   * multiple reads. E.g. every Wheel.time_bucket milliseconds
   */
  bool dangerousTurn = getDangerousTurn(currSpeed);

  printData();
  // printOutputForSerialMonitor(fsrReading, curGrip, currSpeed, curZGyro);
  //printDataAsCSV(curGrip, currSpeed, curZGyro);

  //delay(1000);
} 
