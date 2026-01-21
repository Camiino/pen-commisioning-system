#include "i2c_manager.h"
#include "logging.h"

std::vector<ModuleInfo> modules;
std::vector<uint8_t> availableAddresses;

void initI2C() {
  Wire.begin();  // Master-Modus
  logInfo("I2C-Bus initialized.");

  // initialize available addresses (0x11–0x77)
  for (uint8_t addr = 0x11; addr <= 0x77; addr++) {
    availableAddresses.push_back(addr);
  }

  // hard coded module for now
  ModuleInfo shaftModule;
  shaftModule.address = 0x11;
  shaftModule.type = "Shaft";
  shaftModule.uniqueID = 0xDEADBEEF;  // placeholder UID
  shaftModule.stock = 0;  // initial stock
  modules.push_back(shaftModule);

  // delete address 0x11 from available
  for (auto it = availableAddresses.begin(); it != availableAddresses.end(); ++it) {
    if (*it == 0x11) {
      availableAddresses.erase(it);
      break;
    }
  }

  logInfo("Added module shaft with hardcoded address 0x11");
}

void scanForModules() {
  // Hier später die Logik zum Scannen nach Modulen
  // (z. B. Wire.requestFrom für jede Adresse)
}

void dispenseFromModule(uint8_t moduleAddress) {
  Wire.beginTransmission(moduleAddress);
  byte command = 1;
  Wire.write(command);  // command "1" = dispense
  Wire.endTransmission();
  logInfo("Send dispense command to 0x" + String(moduleAddress, HEX));
}

// NOTE: Not implemented yet
void updateModuleStock(uint8_t moduleAddress, int newStock) {
  Wire.beginTransmission(moduleAddress);
  Wire.write('S');  // Befehl "S" = Stock aktualisieren
  Wire.write((newStock >> 8) & 0xFF);  // High-Byte
  Wire.write(newStock & 0xFF);         // Low-Byte
  Wire.endTransmission();
  logInfo("Stock for module 0x" + String(moduleAddress, HEX) + " updated");
}

std::vector<ModuleInfo> getConnectedModules() {
  return modules;
}
