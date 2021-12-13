int gasPin = 1; 
int brakePin = 2;
int gasReading;
int brakeReading;

unsigned long oldTime = millis();
unsigned long currTime = millis();
unsigned long lastFloor = millis();

int brakingIncidents = 0;
bool brakingIncidentDetected = false;
float currSpeed = 0;

enum GasPress {
  NoGas,
  SafeGas,
  DangGas
};

enum BrakePress {
  NoBrake,
  SafeBrake,
  DangBrake
};

GasPress currGas = NoGas, prevGas = NoGas;
BrakePress currBrake = NoBrake, prevBrake = NoBrake;

String gasToString(GasPress gas) {
  switch (gas) {
    case NoGas:
      return String("No Gas");
    case SafeGas:
      return String("Light Gas");
    case DangGas:
      return String("Flooring");
  }
}

String brakeToString(BrakePress brake) {
  switch (brake) {
    case NoGas:
      return String("Not Braking");
    case SafeGas:
      return String("Light Braking");
    case DangGas:
      return String("Hard Braking");
  }
}

GasPress getGas() {
    gasReading = analogRead(gasPin);
    float speedIncrease = 0;
    currTime = millis();
    // We'll have a few threshholds, qualitatively determined
    if (gasReading < 5) {
        currGas = NoGas;
        if (currSpeed > 0) {
          speedIncrease = -0.5;
        } else {
          currSpeed = 0;
        }
    } else if (gasReading < 200) {
        currGas = SafeGas;
        speedIncrease = 0.75;
    } else {
        currGas = DangGas;
        speedIncrease = 2;
        lastFloor = currTime;
    }

    if ((currTime - oldTime) > 200) {
      currSpeed += speedIncrease;
      if (currSpeed > 110) {
        currSpeed = 110;
      }
    }

    return currGas;
}

BrakePress getBrake() {
    brakeReading = analogRead(brakePin);
    float speedDecrease = 0;
    brakingIncidentDetected = false;
    
    currTime = millis();
    // We'll have a few threshholds, qualitatively determined
    if (brakeReading < 5) {
        currBrake = NoBrake;
    } else if (brakeReading < 200) {
        currBrake = SafeBrake;
        speedDecrease = 0.75;
    } else {
        currBrake = DangBrake;
        speedDecrease = 2;
        if ((currTime - lastFloor) > 5000) {
          brakingIncidents++;
          brakingIncidentDetected = true;
        }
    }

    if ((currTime - oldTime) > 200) {
      currSpeed -= speedDecrease;
      if (currSpeed <= 0) {
        currSpeed = 0;
      }
      oldTime = currTime;
    }

    return currBrake;
}
