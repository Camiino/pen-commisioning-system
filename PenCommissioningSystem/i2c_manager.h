#pragma once
#include <Wire.h>
#include <vector>

struct ModuleInfo {
  uint8_t address;
  String type;
  uint32_t uniqueID;
  int stock;
};

// external functions
void initI2C();
void scanForModules();
void dispenseFromModule(uint8_t moduleAddress);
void updateModuleStock(uint8_t moduleAddress, int newStock);
std::vector<ModuleInfo> getConnectedModules();
