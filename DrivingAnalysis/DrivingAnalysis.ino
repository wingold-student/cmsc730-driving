#include "./GripStrength/GripStrength.h"
#include "./Wheel/Wheel.h"
#include "./HeartRate/HeartRate.h"
#include "./Pedals/Pedals.h"

bool printForHuman = false;


void pingUserOnDanger() {
  if (whiteKnuckleIncidentDetected ||
      dangerousTurnDetected ||
      brakingIncidentDetected) {
        bleSerial.write("!");
      }
}

void printDataForProcessing() {
    Serial.print(currSpeed);
    Serial.print(",");

    Serial.print(curGrip);
    Serial.print(",");

    Serial.print(actual_angle);
    Serial.print(",");

    Serial.print(heartRate);
    Serial.print(",");

    // Acceleration
    Serial.print(currGas);
    Serial.print(",");

    // Break
    Serial.print(currBrake);
    Serial.print(",");

    // Warning Detection
    Serial.print(dangerousTurnDetected);
    Serial.print(",");

    Serial.print(brakingIncidentDetected);
    Serial.print(",");

    Serial.print(whiteKnuckleIncidentDetected);
    Serial.print(",");

    Serial.print(highHeartRate);

    Serial.println();
}

void printDataForHuman() {
  Serial.print("Speed:\t");
  Serial.print(currSpeed);
  Serial.print("\t");

  Serial.print("Grip:\t");
  Serial.print(gripToString(curGrip));
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
  Serial.print("Gas:\t");
  Serial.print(gasToString(currGas));
  Serial.print("\t");

  // Break
  Serial.print("Brake:\t");
  Serial.print(brakeToString(currBrake));

  Serial.println();
}

void printEndForProcessing() {
  Serial.print(whiteKnuckleIncidents);
  Serial.print(",");
  
  Serial.print(whippingIncidents);
  Serial.print(",");

  Serial.print(brakingIncidents);
  Serial.print(",");

  Serial.print(highHeartRateIncidents);
  Serial.print(",");

  Serial.println(medianHeartRate);

  Serial.flush();
}

void printEndForHuman() {
  Serial.println("============END============");
  Serial.print("Whipping Incidents: ");
  Serial.println(whippingIncidents);
  Serial.print("White Knuckle Incidents: ");
  Serial.println(whiteKnuckleIncidents);
  Serial.print("Hard Brake then Flooring Incidents: ");
  Serial.println(brakingIncidents);
  Serial.print("High Heart Rate Incidents: ");
  Serial.println(highHeartRateIncidents);
  Serial.print("Median Heart Rate: ");
  Serial.println(medianHeartRate);
  Serial.flush();
}

void printEndStats() {
  if (printForHuman) {
    printEndForHuman();
  } else {
    printEndForProcessing();
  }
}

void printData() {
  medianHeartRate = sumHeartRate / heartRateCount;

  if (printForHuman) {
    printDataForHuman();
  } else {
    printDataForProcessing();
  }
}

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600); // TODO: Can make higher if change HM-10?
  Wire.begin();

  setupHeartRate();
  setupWheel();  

  if (printForHuman) {
    Serial.println("Starting....");
  }
}
 
void loop(void) {
  if (Serial.available() > 0) {
    printEndStats();
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
   * @brief Get the pressure on Gas
   * 
   * Updates curGas value
   */
  getGas();

  /**
   * @brief Get the pressure on Brake
   * 
   * Updates curBrake value
   * Updates brakingIncidents
   */
  getBrake();

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
   * Updates dangerousTurnExpected
   * Updates whippingIncidents
   * 
   * NOTE: This averages out acceleration over potentially
   * multiple reads. E.g. every Wheel.time_bucket milliseconds
   */
  getDangerousTurn(currSpeed);

  /**
   * @brief Sends message on HM10 for warning
   * 
   */
  pingUserOnDanger();

  printData();

  // Needed to let web server process data
  delay(150);
} 
