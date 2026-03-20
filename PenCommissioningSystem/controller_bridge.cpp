#include "controller_bridge.h"

#include <Wire.h>

#include "inventory_state.h"
#include "logging.h"

namespace {
constexpr uint8_t COMMAND_DISPENSE_ONE = 1;
constexpr uint8_t STATUS_IDLE = 0;
constexpr uint8_t STATUS_BUSY = 1;
constexpr uint8_t STATUS_SUCCESS = 2;
constexpr uint8_t STATUS_EMPTY = 3;
constexpr uint8_t STATUS_FAILED = 4;
constexpr unsigned long STATUS_POLL_INTERVAL_MS = 100;
constexpr unsigned long DISPENSE_TIMEOUT_MS = 12000;

struct ControllerRoute {
  const char *componentKey;
  uint8_t address;
  const char *label;
};

// Update componentKey as more physical modules are added.
ControllerRoute controllerRoutes[] = {
  {"shell", 0x11, "subie_mech"},
  {"tip", 0x11, "subie_mech"},
};

bool controllerInitialized = false;

const ControllerRoute *findRouteByKey(const String &key) {
  const char *resolvedKey = resolveComponentKey(key);
  if (resolvedKey == nullptr) {
    return nullptr;
  }

  for (size_t index = 0; index < sizeof(controllerRoutes) / sizeof(controllerRoutes[0]); ++index) {
    if (String(controllerRoutes[index].componentKey).equalsIgnoreCase(resolvedKey)) {
      return &controllerRoutes[index];
    }
  }

  return nullptr;
}

String formatAddress(uint8_t address) {
  return "0x" + String(address, HEX);
}

const char *statusLabel(uint8_t status) {
  switch (status) {
    case STATUS_IDLE:
      return "idle";
    case STATUS_BUSY:
      return "busy";
    case STATUS_SUCCESS:
      return "success";
    case STATUS_EMPTY:
      return "empty";
    case STATUS_FAILED:
      return "failed";
    default:
      return "unknown";
  }
}

bool sendCommand(uint8_t address, uint8_t command, String &error) {
  Wire.beginTransmission(address);
  Wire.write(command);
  const uint8_t result = Wire.endTransmission();
  if (result == 0) {
    return true;
  }

  error = "I2C write failed with code " + String(result);
  return false;
}

bool readStatus(uint8_t address, uint8_t &status, String &error) {
  const uint8_t expectedBytes = 1;
  const uint8_t receivedBytes = Wire.requestFrom(static_cast<int>(address), static_cast<int>(expectedBytes));
  if (receivedBytes != expectedBytes || !Wire.available()) {
    error = "I2C status read failed";
    return false;
  }

  status = Wire.read();
  return true;
}
}  // namespace

void initControllerBridge() {
  Wire.begin();
  controllerInitialized = true;
  logInfo("Controller bridge initialised");
}

bool hasDispenseController(const String &key) {
  return findRouteByKey(key) != nullptr;
}

bool confirmDispenseForComponent(const String &key, String &details, String &error) {
  const ControllerRoute *route = findRouteByKey(key);
  if (route == nullptr) {
    error = "No dispense controller configured for " + key;
    return false;
  }

  if (!controllerInitialized) {
    error = "Controller bridge not initialised";
    return false;
  }

  uint8_t status = STATUS_IDLE;
  if (!readStatus(route->address, status, error)) {
    error = "Unable to read controller status from " + String(route->label) + ": " + error;
    return false;
  }

  if (status == STATUS_BUSY) {
    error = String(route->label) + " is busy";
    return false;
  }

  if (!sendCommand(route->address, COMMAND_DISPENSE_ONE, error)) {
    error = "Unable to send dispense command to " + String(route->label) + ": " + error;
    return false;
  }

  const unsigned long startedAt = millis();
  while (millis() - startedAt < DISPENSE_TIMEOUT_MS) {
    delay(STATUS_POLL_INTERVAL_MS);

    String statusError;
    if (!readStatus(route->address, status, statusError)) {
      logError("Status poll failed for " + String(route->label) + ": " + statusError);
      continue;
    }

    if (status == STATUS_IDLE || status == STATUS_BUSY) {
      continue;
    }

    if (status == STATUS_SUCCESS) {
      details = "Bestaetigt durch " + String(route->label) + " (" + formatAddress(route->address) + ")";
      return true;
    }

    error = String(route->label) + " reported " + statusLabel(status);
    return false;
  }

  error = "Timed out waiting for dispense confirmation from " + String(route->label);
  return false;
}
