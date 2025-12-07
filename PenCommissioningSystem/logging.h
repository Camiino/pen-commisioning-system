#pragma once
#include <Arduino.h>

void initSerial();
void logInfo(const String &message);
void logError(const String &message);
void logDebug(const String &message);
