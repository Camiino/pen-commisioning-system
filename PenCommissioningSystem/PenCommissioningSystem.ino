#include "logging.h"
#include "wifi_manager.h"
#include "web_server.h"

void setup() {
  initSerial();
  initEEPROM();
  initWiFi();
  initWebServer();
  }

void loop() {
  handleWiFi();  // check wifi status
  handleWebServer();  // manage web server requests
}