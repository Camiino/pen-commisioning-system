#include "wifi_manager.h"

WiFiManager wifiManager;

namespace {
constexpr char ACCESS_POINT_NAME[] = "Pen Commissioning Machine AP";
constexpr unsigned long WIFI_STATUS_LOG_INTERVAL_MS = 10000;

bool fallbackApActive = false;
unsigned long lastWifiStatusLogAt = 0;

void startFallbackAp(const IPAddress &localIP, const IPAddress &gateway, const IPAddress &subnet) {
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPdisconnect(true);
  WiFi.softAPConfig(localIP, gateway, subnet);

  if (!WiFi.softAP(ACCESS_POINT_NAME)) {
    logError("Fallback AP could not be started");
    fallbackApActive = false;
    return;
  }

  fallbackApActive = true;
  logInfo("Fallback AP enabled. Connect to '" + String(ACCESS_POINT_NAME) + "'");
  logInfo("Fallback AP IP: " + WiFi.softAPIP().toString());
}
}  // namespace

void initWiFi() {
  IPAddress localIP(192, 168, 1, 1);  // set local AP IP
  IPAddress gateway(192, 168, 1, 0);
  IPAddress subnet(255, 255, 255, 0);
  wifiManager.setAPStaticIPConfig(localIP, gateway, subnet);

  wifiManager.setAPCallback([](WiFiManager *manager) {
    (void)manager;
    logInfo("AP mode activated. Connect to '" + String(ACCESS_POINT_NAME) + "'");
    logInfo("Config portal IP: " + WiFi.softAPIP().toString());
  });

  wifiManager.setDebugOutput(false);  // deactivate debug logs
  logInfo("Starting WiFi manager");

  // start wifi manager (automatically if there are no saved credentials)
  if (!wifiManager.autoConnect(ACCESS_POINT_NAME)) {
    logError("WiFi auto-connect failed; backend sync remains offline");
    startFallbackAp(localIP, gateway, subnet);
    return;
  }

  fallbackApActive = false;
  logInfo("WiFi connected. IP: " + WiFi.localIP().toString());
}

void handleWiFi() {
  const bool connected = WiFi.status() == WL_CONNECTED;
  const unsigned long now = millis();

  if (connected) {
    lastWifiStatusLogAt = 0;
    return;
  }

  if (lastWifiStatusLogAt != 0 && now - lastWifiStatusLogAt < WIFI_STATUS_LOG_INTERVAL_MS) {
    return;
  }

  lastWifiStatusLogAt = now;
  if (fallbackApActive) {
    logInfo("WiFi not connected to router; fallback AP stays active and backend sync is paused");
    return;
  }

  logError("WiFi not connected; backend sync is paused");
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

String getLocalNetworkIp() {
  if (!isWiFiConnected()) {
    return "";
  }

  IPAddress ip = WiFi.localIP();
  if (ip == IPAddress(static_cast<uint32_t>(0))) {
    return "";
  }

  return ip.toString();
}

String getLocalBaseUrl() {
  String ip = getLocalNetworkIp();
  if (ip.length() == 0) {
    return "";
  }

  return "http://" + ip;
}
