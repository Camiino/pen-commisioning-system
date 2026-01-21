// 28BYJ-48 Stepper Motor with Arduino - Basic
#include <Stepper.h>

// Steps per revolution for 28BYJ-48
const int stepsPerRevolution = 2048;
const int gearRatio = 4;

// Define stepper motor object
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(10); // RPM
}

void loop() {

  dispenseUnit();
}

// Funktion: Gibt einen Schaft aus (eine vollständige Umdrehung)
void dispenseUnit() {
  Serial.println("Starte Ausgabemechanismus...");

  // Eine vollständige Umdrehung im Uhrzeigersinn
  myStepper.step(int(stepsPerRevolution / gearRatio));
  Serial.println("Schaft ausgegeben (eine Umdrehung).");

  // Optional: Kurze Pause für Stabilität
  delay(5000);
}