int fsrPin = 0;     // the FSR and 10K pulldown are connected to a0
int fsrReading;     // the analog reading from the FSR resistor divider
bool inc = true;
int currSpeed = 0;

void setup(void) {
  // We'll send debugging information via the Serial monitor
  Serial.begin(9600);   
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
  Serial.print("Current Speed: ");
  Serial.print(currSpeed);
  Serial.print(" ");
  
  fsrReading = analogRead(fsrPin);
  int i = 0;

  
  
  Serial.print("Analog reading: ");
  Serial.print(fsrReading);     // the raw analog reading
 
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
  delay(200);
} 
