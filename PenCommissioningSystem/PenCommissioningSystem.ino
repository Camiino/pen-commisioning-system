#include "logging.h"
#include "inventory_state.h"
#include "wifi_manager.h"
#include "web_server.h"

void setup() {
  initSerial();
  initInventoryState();
  initWiFi();
  initWebServer();
}

void loop() {
  handleWiFi();  // check wifi status
  handleWebServer();  // manage web server requests
}
