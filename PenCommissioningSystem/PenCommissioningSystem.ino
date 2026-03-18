#include "logging.h"
#include "device_api.h"
#include "inventory_state.h"
#include "remote_sync.h"
#include "wifi_manager.h"
#include "web_server.h"

void setup() {
  initSerial();
  initInventoryState();
  initWiFi();
  initDeviceApi();
  initWebServer();
  initRemoteSync();
}

void loop() {
  handleWiFi();  // check wifi status
  handleWebServer();  // manage web server requests
  handleRemoteSync();
}
