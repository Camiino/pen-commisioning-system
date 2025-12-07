#pragma once
#include <WiFiManager.h>
#include <IPAddress.h>

#include "logging.h"

void initWiFi();
void handleWiFi();
bool isWiFiConnected();
String getWiFiStatus();
