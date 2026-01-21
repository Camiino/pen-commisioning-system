// module
#include <Wire.h>
#include <Stepper.h>

byte command;

// Steps per revolution for 28BYJ-48
const int stepsPerRevolution = 2048;
const int gearRatio = 4;
const int revolution = int(stepsPerRevolution / gearRatio);

// LDR
const int ldrPin = A0;  // LDR an Analog-Pin A0

// Define stepper motor object
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void receiver(int n_bytes) {
  while (Wire.available()) {
    command = Wire.read();
  }
}

void setup() {
  // setup serial interface
  Serial.begin(115200);
  Serial.println("Serial connection to shaft module initialized!");

  // stepper
  myStepper.setSpeed(10); // RPM

  // setup I²C
  Wire.begin(0x10);
  Wire.onReceive(receiver);
  Serial.println("Ready to receive I²C commands");
}

void loop() {
  // read LDR value
  int ldrValue = analogRead(ldrPin);
  //Serial.println(ldrValue);
  //delay(50);

  // dipense on I²C command
  if (command == 1) {
    dispenseUnit();
    command = 0;  // reset command
  }
}

// Funktion: Gibt einen Schaft aus (eine vollständige Umdrehung)
void dispenseUnit() {
  Serial.println("Dispensing one unit: shaft");

  // turning backwards a little
  myStepper.step(-int(revolution/ 8));
  delay(50);
  // one full revolution + correction
  myStepper.step(revolution + int(revolution / 8));
  Serial.println("Dispensed one shaft");

  delay(50);

}
