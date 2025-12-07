#pragma once
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "logging.h"
#include "wifi_manager.h"

// external variables
extern WebServer server;
extern DNSServer dnsServer;

// main functions
void initWebServer();
void handleWebServer();

// log functionalities
void addLogEntry(String component, int quantity, String action);
String getCurrentTimestamp();

// EEPROM functions
void initEEPROM();
void saveStockToEEPROM(int stock);

// handler for pages
void handleRoot();
void handleOrder();
void handleAdmin();
void handleUpdateStock();
void handleNotFound();
