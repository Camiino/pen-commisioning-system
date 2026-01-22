
// LDR
const int ldrPin = A0;  // LDR an Analog-Pin A0

void setup() {
  // setup serial interface
  Serial.begin(115200);
  Serial.println("Serial connection to shaft module initialized!");
}

void loop() {
  // read LDR value
  int ldrValue = analogRead(ldrPin);
  Serial.println(ldrValue);
  delay(50);
}