#include "logging.h"

void initSerial() {
  Serial.begin(115200);
  const unsigned long waitStartedAt = millis();
  while (!Serial && millis() - waitStartedAt < 2000) {
    delay(10);
  }
  delay(100);
  logInfo("Serial connection established");
}

void logInfo(const String &message) {
  Serial.println("[INFO] " + message);
}

void logError(const String &message) {
  Serial.println("[ERROR] " + message);
}

void logDebug(const String &message) {
  Serial.println("[DEBUG] " + message);
}
