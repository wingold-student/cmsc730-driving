int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider

int whiteKnuckleIncidents = 0;
bool whiteKnuckleIncidentDetected = false;

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

GripStrength getGrip() {
    fsrReading = analogRead(fsrPin);
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

    if (prevGrip != curGrip && curGrip == WhiteKnuckleGrip) {
        whiteKnuckleIncidents++;
        whiteKnuckldeIncidentDetected = true;
    }
    whiteKnuckleIncidentDetected = false;

    return curGrip;
}
