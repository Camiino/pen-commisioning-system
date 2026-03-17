#pragma once

#include <Arduino.h>

constexpr int COMPONENT_COUNT = 5;
constexpr int MAX_LOG_ENTRIES = 20;

struct Component {
  const char *key;
  const char *name;
  const char *sku;
  const char *bin;
  const char *iconId;
  int stock;
  int reorderLevel;
};

struct LogEntry {
  unsigned long timestampSeconds;
  String action;
  String item;
  int quantity;
  String details;
};

extern Component components[COMPONENT_COUNT];
extern LogEntry logEntries[MAX_LOG_ENTRIES];
extern int logCount;

void initInventoryState();
Component *findComponentByKey(const String &key);
Component *findComponentByName(const String &name);
int getComponentIndexByKey(const String &key);
void addLogEntry(const String &action, const String &item, int quantity, const String &details = "");
String getCurrentTimestamp();
bool adjustComponentStock(const String &key, int delta, const String &action, const String &details, String &error);
bool setComponentStock(const String &key, int target, const String &action, const String &details, String &error);
bool setComponentMin(const String &key, int target, String &error);
bool fulfillOrder(const String *keys, const int *quantities, size_t lineCount, String &error);
void buildInventoryJson(String &out);
void buildAuditLogJson(String &out);
