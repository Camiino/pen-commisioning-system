// 28BYJ-48 Stepper Motor with Arduino - Basic
#include <Stepper.h>

// Steps per revolution for 28BYJ-48
const int stepsPerRevolution = 2048;

// Define stepper motor object
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(10); // RPM
}

void loop() {
  myStepper.step(stepsPerRevolution); // One full rotation clockwise
  delay(1000);
  myStepper.step(-stepsPerRevolution); // One full rotation counter-clockwise
  delay(1000);
}