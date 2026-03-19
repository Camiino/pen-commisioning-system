#include "inventory_state.h"

#include <EEPROM.h>

#include "controller_bridge.h"
#include "logging.h"

namespace {
constexpr int EEPROM_SIZE = 512;
constexpr int STOCK_BASE_ADDRESS = 0;
constexpr int MIN_BASE_ADDRESS = STOCK_BASE_ADDRESS + (COMPONENT_COUNT * static_cast<int>(sizeof(int)));

String escapeJson(const String &value) {
  String escaped;
  escaped.reserve(value.length() + 8);

  for (size_t i = 0; i < value.length(); ++i) {
    const char current = value.charAt(i);
    if (current == '\\' || current == '"') {
      escaped += '\\';
      escaped += current;
      continue;
    }

    if (current == '\n') {
      escaped += "\\n";
      continue;
    }

    if (current == '\r') {
      continue;
    }

    escaped += current;
  }

  return escaped;
}

void saveInventoryState() {
  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    const int stockAddress = STOCK_BASE_ADDRESS + (index * static_cast<int>(sizeof(int)));
    const int minAddress = MIN_BASE_ADDRESS + (index * static_cast<int>(sizeof(int)));
    EEPROM.put(stockAddress, components[index].stock);
    EEPROM.put(minAddress, components[index].reorderLevel);
  }

  EEPROM.commit();
}

bool isReasonableValue(int value) {
  return value >= 0 && value <= 100000;
}

bool validateOrderLines(const String *keys, const int *quantities, size_t lineCount, String &error) {
  if (lineCount == 0) {
    error = "At least one order line is required";
    return false;
  }

  for (size_t index = 0; index < lineCount; ++index) {
    if (quantities[index] <= 0) {
      error = "Quantities must be positive";
      return false;
    }

    Component *component = findComponentByKey(keys[index]);
    if (component == nullptr) {
      error = "Unknown item key";
      return false;
    }

    if (component->stock < quantities[index]) {
      error = "Insufficient stock for " + String(component->name);
      return false;
    }
  }

  return true;
}
}  // namespace

Component components[COMPONENT_COUNT] = {
  {"spring", "Feder", "KUG-FED-001", "A-12", "3", 45, 20},
  {"refill", "Mine", "KUG-MIN-001", "B-08", "1", 15, 25},
  {"upperShell", "Oberschale", "KUG-OBS-001", "C-04", "0", 38, 15},
  {"lowerShell", "Unterschale", "KUG-UNS-001", "C-05", "0", 42, 15},
  {"plunger", "Druecker", "KUG-DRU-001", "D-11", "4", 8, 10},
};

LogEntry logEntries[MAX_LOG_ENTRIES];
int logCount = 0;

void initInventoryState() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    logError("EEPROM initialisation failed.");
    return;
  }

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    const int stockAddress = STOCK_BASE_ADDRESS + (index * static_cast<int>(sizeof(int)));
    const int minAddress = MIN_BASE_ADDRESS + (index * static_cast<int>(sizeof(int)));

    int storedStock = 0;
    int storedMin = 0;
    EEPROM.get(stockAddress, storedStock);
    EEPROM.get(minAddress, storedMin);

    if (isReasonableValue(storedStock)) {
      components[index].stock = storedStock;
    }

    if (isReasonableValue(storedMin)) {
      components[index].reorderLevel = storedMin;
    }
  }

  logInfo("Inventory state loaded from EEPROM");
}

Component *findComponentByKey(const String &key) {
  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    if (key.equalsIgnoreCase(components[index].key)) {
      return &components[index];
    }
  }

  return nullptr;
}

Component *findComponentByName(const String &name) {
  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    if (name.equalsIgnoreCase(components[index].name)) {
      return &components[index];
    }
  }

  return nullptr;
}

int getComponentIndexByKey(const String &key) {
  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    if (key.equalsIgnoreCase(components[index].key)) {
      return index;
    }
  }

  return -1;
}

void addLogEntry(const String &action, const String &item, int quantity, const String &details) {
  const unsigned long timestampSeconds = millis() / 1000UL;
  LogEntry nextEntry = {timestampSeconds, action, item, quantity, details};

  if (logCount < MAX_LOG_ENTRIES) {
    logEntries[logCount] = nextEntry;
    ++logCount;
    return;
  }

  for (int index = 1; index < MAX_LOG_ENTRIES; ++index) {
    logEntries[index - 1] = logEntries[index];
  }

  logEntries[MAX_LOG_ENTRIES - 1] = nextEntry;
}

String getCurrentTimestamp() {
  return String(millis() / 1000UL) + "s";
}

bool adjustComponentStock(const String &key, int delta, const String &action, const String &details, String &error) {
  Component *component = findComponentByKey(key);
  if (component == nullptr) {
    error = "Unknown item key";
    return false;
  }

  const int nextStock = component->stock + delta;
  if (nextStock < 0) {
    error = "Insufficient stock";
    return false;
  }

  component->stock = nextStock;
  saveInventoryState();
  addLogEntry(action, component->name, delta, details);
  return true;
}

bool setComponentStock(const String &key, int target, const String &action, const String &details, String &error) {
  if (target < 0) {
    error = "Target stock must be non-negative";
    return false;
  }

  Component *component = findComponentByKey(key);
  if (component == nullptr) {
    error = "Unknown item key";
    return false;
  }

  const int delta = target - component->stock;
  component->stock = target;
  saveInventoryState();
  addLogEntry(action, component->name, delta, details);
  return true;
}

bool setComponentMin(const String &key, int target, String &error) {
  if (target < 0) {
    error = "Minimum stock must be non-negative";
    return false;
  }

  Component *component = findComponentByKey(key);
  if (component == nullptr) {
    error = "Unknown item key";
    return false;
  }

  const int previousMin = component->reorderLevel;
  component->reorderLevel = target;
  saveInventoryState();
  addLogEntry("Mindestbestand geaendert", component->name, target, "Vorher: " + String(previousMin));
  return true;
}

bool fulfillOrder(const String *keys, const int *quantities, size_t lineCount, String &error) {
  if (!validateOrderLines(keys, quantities, lineCount, error)) {
    return false;
  }

  bool savedDirectChanges = false;
  int processedUnits = 0;

  for (size_t index = 0; index < lineCount; ++index) {
    Component *component = findComponentByKey(keys[index]);
    if (component == nullptr) {
      error = "Unknown item key";
      return false;
    }

    if (hasDispenseController(component->key)) {
      for (int unit = 0; unit < quantities[index]; ++unit) {
        String dispenseDetails;
        if (!confirmDispenseForComponent(component->key, dispenseDetails, error)) {
          if (processedUnits > 0) {
            error += ". Order stopped after " + String(processedUnits) + " confirmed unit(s)";
          }
          return false;
        }

        String stockError;
        if (!adjustComponentStock(component->key, -1, "Bestellung ausgegeben", dispenseDetails, stockError)) {
          error = "Confirmed dispense for " + String(component->name) + " could not be written to inventory: " + stockError;
          return false;
        }

        ++processedUnits;
      }

      continue;
    }

    component->stock -= quantities[index];
    addLogEntry("Bestellung ausgegeben", component->name, -quantities[index], "No controller confirmation configured");
    processedUnits += quantities[index];
    savedDirectChanges = true;
  }

  if (savedDirectChanges) {
    saveInventoryState();
  }

  return true;
}

void buildInventoryJson(String &out) {
  out = "[";

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    if (index > 0) {
      out += ",";
    }

    out += "{";
    out += "\"key\":\"" + String(components[index].key) + "\",";
    out += "\"name\":\"" + escapeJson(String(components[index].name)) + "\",";
    out += "\"sku\":\"" + escapeJson(String(components[index].sku)) + "\",";
    out += "\"stock\":" + String(components[index].stock) + ",";
    out += "\"min\":" + String(components[index].reorderLevel) + ",";
    out += "\"bin\":\"" + escapeJson(String(components[index].bin)) + "\"";
    out += "}";
  }

  out += "]";
}

void buildAuditLogJson(String &out) {
  out = "[";

  for (int index = 0; index < logCount; ++index) {
    if (index > 0) {
      out += ",";
    }

    out += "{";
    out += "\"id\":\"LOG-" + String(index + 1) + "\",";
    out += "\"timestamp\":\"" + String(logEntries[index].timestampSeconds) + "s\",";
    out += "\"action\":\"" + escapeJson(logEntries[index].action) + "\",";
    out += "\"item\":\"" + escapeJson(logEntries[index].item) + "\",";
    out += "\"user\":\"System\",";
    out += "\"details\":\"" + escapeJson(logEntries[index].details) + "\",";
    out += "\"quantity\":" + String(logEntries[index].quantity);
    out += "}";
  }

  out += "]";
}
