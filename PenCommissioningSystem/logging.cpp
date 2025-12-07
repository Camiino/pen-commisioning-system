#include "logging.h"

void initSerial() {
  Serial.begin(115200);
  while (!Serial);  // wait for serial monitor
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
