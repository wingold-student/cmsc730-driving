#include <SoftwareSerial.h>

// 866C8571-8E4D-04D2-8593-C32FB7872F3C
int TX_PIN = 10, RX_PIN = 11;
const int BUFFER_SIZE = 4;
byte buf[BUFFER_SIZE] = {0};

int heartRate = 0;

SoftwareSerial bleSerial(RX_PIN, TX_PIN);

void setupHeartRate() {
  bleSerial.begin(9600);
}

int getHeartRate() {
  // put your main code here, to run repeatedly:
  if (bleSerial.available() == 4) {
    bleSerial.readBytes(buf, BUFFER_SIZE);
    heartRate = buf[0];
    //Serial.print("Rate: ");
    //Serial.println(heartRate);
    // Serial.write(bleSerial.read());
  }
  if (Serial.available()) {
    bleSerial.write(Serial.read());
    //Serial.println();
  }

  return heartRate;
}
