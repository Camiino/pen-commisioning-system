#include "logging.h"
#include "wifi_manager.h"
#include "web_server.h"
#include "i2c_manager.h"

void setup() {
  initSerial();
  initEEPROM();
  initI2C();
  initWiFi();
  initWebServer();
  }

void loop() {
  handleWiFi();  // check wifi status
  handleWebServer();  // manage web server requests
  // later add scanForModules()
}