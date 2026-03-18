#pragma once
#include "bundled_wifi_manager.h"
#include <IPAddress.h>

#include "logging.h"

void initWiFi();
void handleWiFi();
bool isWiFiConnected();
String getLocalNetworkIp();
String getLocalBaseUrl();
