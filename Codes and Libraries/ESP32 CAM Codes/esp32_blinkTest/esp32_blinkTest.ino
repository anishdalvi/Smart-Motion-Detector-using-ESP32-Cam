//LED BUILT_IN is GPIO 33 
void setup() {
  pinMode(33, OUTPUT); // Set the pin as output
}

// Remember that the pin work with inverted logic
// LOW to Turn on and HIGH to turn off
void loop() {
  digitalWrite(33, LOW); //Turn on
  delay (1000); //Wait 1 sec
  digitalWrite(33, HIGH); //Turn off
  delay (1000); //Wait 1 sec
}