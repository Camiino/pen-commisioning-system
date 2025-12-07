#include "wifi_manager.h"

WiFiManager wifiManager;

void initWiFi() {
  IPAddress localIP(192,168, 1, 1);  // set local AP IP
  IPAddress gateway(192, 168, 1, 0);
  IPAddress subnet(255, 255, 255, 0);
  wifiManager.setAPStaticIPConfig(localIP, gateway, subnet);

  // uncomment for testing
  wifiManager.resetSettings();  // wipe saved credentials
  //wifiManager.setDebugOutput(false);  // deactivate debug logs

  // start wifi manager (automatically if there are no saved credentials)
  if (!wifiManager.autoConnect("Pen Commissioning Machine AP")) {
    logError("Error connecting to WiFi!");
    ESP.restart();  // restart on error
  }

  // create callback for AP
  wifiManager.setAPCallback([](WiFiManager* wifiManager) {
    logInfo("AP mode activated. Connect to 'Pen Commissioning Machine AP'");
  });

  logInfo("WiFi connected. IP: " + WiFi.localIP().toString());
}

void handleWiFi() {
  // optional: check wifi status regularly
  if (WiFi.status() != WL_CONNECTED) {
    logError("WiFi connection lost!");
    // todo: add action here
  }
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String getWiFiStatus() {
  if (isWiFiConnected()) {
    return "Connected. IP: " + WiFi.localIP().toString();
  } else {
    return "AP mode activated. Connect to 'Pen Commissioning Machine AP'";
  }
}
