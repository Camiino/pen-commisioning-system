// module
#include <Wire.h>
#include <Stepper.h>

byte command;

// Steps per revolution for 28BYJ-48
const int stepsPerRevolution = 2048;
const int gearRatio = 4;
const int revolution = int(stepsPerRevolution / gearRatio);
int failCounter = 0;

// LDR
const int ldrPin = A0;  // LDR an Analog-Pin A0
int threshold = 60;

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
  Wire.begin(0x11);
  Wire.onReceive(receiver);
  Serial.println("Ready to receive I²C commands");
}

void loop() {
  // dipense on I²C command
  if (command == 1) {
    dispenseUnit();
    command = 0;  // reset command
  }
}

// Funktion: Gibt einen Schaft aus (eine vollständige Umdrehung)
void dispenseUnit() {
  Serial.println("Dispensing one unit: shaft");

  // 1. read sensor before dispensing
  int initialValue = analogRead(ldrPin);
  Serial.print("Sensor value (starting position): ");
  Serial.println(initialValue);

  // 2. turning backwards a little to compensate backlash
  myStepper.step(-int(revolution / 8));
  delay(50);

  // 3. turn to sensor position
  myStepper.step(int(revolution / 4));
  delay(50);
  int currentValue = analogRead(ldrPin);
  Serial.print("Sensor value (over dispenser): ");
  Serial.println(currentValue);

  // 4. check sensor reading and decide what to do
  if (currentValue < initialValue - threshold) {
    Serial.println("Item detected --> proceed with dispensing");
    myStepper.step(int(revolution * 7 / 8));
    Serial.println("Dispensed one shaft");
    failCounter = 0;
  } else {
    Serial.println("No item detected!");
    myStepper.step(-int(revolution / 8));
    delay(50);
    failCounter++;

    if (failCounter < 3) {
      Serial.print("Fail counter: ");
      Serial.print(failCounter);
      Serial.println(" < 3 --> try again");
      dispenseUnit();
    } else {
      Serial.println("ERROR: 3 failed attempts at dispensing! Aborting...");
      failCounter = 0;
    }
  }
}
