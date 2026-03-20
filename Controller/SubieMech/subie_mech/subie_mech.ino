// module
#include <Stepper.h>
#include <Wire.h>

enum CommandCode : byte {
  COMMAND_NONE = 0,
  COMMAND_DISPENSE_ONE = 1,
};

enum ModuleStatus : byte {
  STATUS_IDLE = 0,
  STATUS_BUSY = 1,
  STATUS_SUCCESS = 2,
  STATUS_EMPTY = 3,
  STATUS_FAILED = 4,
};

volatile byte pendingCommand = COMMAND_NONE;
volatile byte moduleStatus = STATUS_IDLE;

// Steps per revolution for 28BYJ-48
const int stepsPerRevolution = 2048;
const int gearRatio = 4;
const int revolution = int(stepsPerRevolution / gearRatio);
const int maxAttempts = 3;

// LDR
const int ldrPin = A0;
int threshold = 60;

// Define stepper motor object
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void receiver(int n_bytes) {
  (void)n_bytes;

  while (Wire.available()) {
    const byte receivedCommand = Wire.read();
    if (receivedCommand == COMMAND_DISPENSE_ONE && moduleStatus != STATUS_BUSY) {
      pendingCommand = COMMAND_DISPENSE_ONE;
      moduleStatus = STATUS_BUSY;
    }
  }
}

void requestEvent() {
  Wire.write(moduleStatus);
}

bool isItemDetected(int sensorValue, int referenceValue) {
  return sensorValue < referenceValue - threshold;
}

void moveToHomeFromSensor() {
  myStepper.step(-int(revolution / 8));
  delay(50);
}

byte dispenseUnit() {
  bool itemDetectedAtLeastOnce = false;

  for (int attempt = 1; attempt <= maxAttempts; ++attempt) {
    Serial.print("Dispense attempt ");
    Serial.print(attempt);
    Serial.print(" of ");
    Serial.println(maxAttempts);

    const int initialValue = analogRead(ldrPin);
    Serial.print("Sensor value (home position): ");
    Serial.println(initialValue);

    myStepper.step(-int(revolution / 8));
    delay(50);

    myStepper.step(int(revolution / 4));
    delay(50);

    const int detectedValue = analogRead(ldrPin);
    Serial.print("Sensor value (detection position): ");
    Serial.println(detectedValue);

    if (!isItemDetected(detectedValue, initialValue)) {
      Serial.println("No item detected at sensor position");
      moveToHomeFromSensor();
      continue;
    }

    itemDetectedAtLeastOnce = true;
    Serial.println("Item detected -> rotate one full cycle to dispense");

    // For the wheel mechanism, keep rotating in the dispense direction
    // until the slot returns to the sensor instead of reversing back.
    myStepper.step(revolution);
    delay(50);

    const int confirmationValue = analogRead(ldrPin);
    Serial.print("Sensor value (post-dispense check): ");
    Serial.println(confirmationValue);

    const bool confirmed = !isItemDetected(confirmationValue, initialValue);
    moveToHomeFromSensor();

    if (confirmed) {
      Serial.println("Dispense confirmed: item no longer detected");
      return STATUS_SUCCESS;
    }

    Serial.println("Dispense not confirmed: item still detected");
  }

  if (!itemDetectedAtLeastOnce) {
    Serial.println("Dispense aborted: no item available");
    return STATUS_EMPTY;
  }

  Serial.println("Dispense failed after all attempts");
  return STATUS_FAILED;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Serial connection to shaft module initialized!");

  myStepper.setSpeed(10);

  Wire.begin(0x11);
  Wire.onReceive(receiver);
  Wire.onRequest(requestEvent);
  Serial.println("Ready to receive I2C commands");
}

void loop() {
  if (pendingCommand == COMMAND_DISPENSE_ONE) {
    pendingCommand = COMMAND_NONE;
    moduleStatus = STATUS_BUSY;
    moduleStatus = dispenseUnit();
  }
}
