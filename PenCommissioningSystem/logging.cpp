#include "logging.h"

void logInfo(const String &message) {
  Serial.println("[INFO] " + message);
}

void logError(const String &message) {
  Serial.println("[ERROR] " + message);
}

void logDebug(const String &message) {
  Serial.println("[DEBUG] " + message);
}
