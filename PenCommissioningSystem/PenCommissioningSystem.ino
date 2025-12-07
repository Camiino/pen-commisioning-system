#include "logging.h"
#include "wifi_manager.h"
#include "web_server.h"

void setup() {
  // set up serial interface
  Serial.begin(115200);
  while (!Serial);
  logInfo("System started");

  // start wifi manager
  initWiFi();
  
  // initialize web server
  initWebServer();
  }

void loop() {
  handleWiFi();  // check wifi status
  handleWebServer();  // manage web server requests
}