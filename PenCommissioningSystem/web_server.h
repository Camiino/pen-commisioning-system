#pragma once
#include <WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "logging.h"
#include "wifi_manager.h"

// eeprom setup
#define EEPROM_SIZE 1024
#define STOCK_ADDRESS 0  // addresses 0-27 (7 components x 4 bytes)
#define LOG_START_ADDRESS 28  // addresses 28-1024 for logs
#define MAX_LOG_ENTRIES 50

// external variables
extern WebServer server;
extern DNSServer dnsServer;

// EEPROM functions
void initEEPROM();
void saveStockToEEPROM();
void loadStockFromEEPROM();

// main functions
void initWebServer();
void handleWebServer();

// log functionalities
void addLogEntry(String component, int quantity, String action);
String getCurrentTimestamp();


// handler for pages
void handleRoot();
void handleOrder();
void handleAdmin();
void handleUpdateStock();
void handleNotFound();
