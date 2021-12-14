#include <SoftwareSerial.h>

// 866C8571-8E4D-04D2-8593-C32FB7872F3C
int TX_PIN = 10, RX_PIN = 11;
const int BUFFER_SIZE = 4;
byte buf[BUFFER_SIZE] = {0};

const static highHeartRateThreshold = 95, heartRateCount = 0;
const static float medianHeartRate = 0;
unsigned long lastHighHeartRate = 0, thirtyMin = 1800000;
int heartRate = 0, highHeartRateIncidents = 0;
bool highHeartRate = false;

SoftwareSerial bleSerial(RX_PIN, TX_PIN);

void setupHeartRate() {
  bleSerial.begin(9600);
  lastHighHeartRate = millis();
}

int getHeartRate() {
  // put your main code here, to run repeatedly:
  if (bleSerial.available() == 4) {
    bleSerial.readBytes(buf, BUFFER_SIZE);
    heartRate = buf[0];

    medianHeartRate += heartRate;
    heartRateCount += 1;
    medianHeartRate /= heartRateCount;

    unsigned long sinceLastHighRate = millis() - lastHighHeartRate;
    if (heartRate > highHeartRateThreshold && sinceLastHighRate > thirtyMin) {
      highHeartRate = true;
      highHeartRateIncidents += 1;
    } else {
      highHeartRate = false;
    }
  }

  // Send data to connected peripheral
  // Will cause a ping and notifcation on Apple Watch
  if (Serial.available()) {
    bleSerial.write(Serial.read());
  }

  return heartRate;
}
