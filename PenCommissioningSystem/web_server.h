#pragma once
#include <WebServer.h>
#include <DNSServer.h>

#include "inventory_state.h"
#include "logging.h"
#include "wifi_manager.h"

// external variables
extern WebServer server;
extern DNSServer dnsServer;

// main functions
void initWebServer();
void handleWebServer();

// handler for pages
void handleRoot();
void handleOrder();
void handleAdmin();
void handleUpdateStock();
void handleNotFound();
