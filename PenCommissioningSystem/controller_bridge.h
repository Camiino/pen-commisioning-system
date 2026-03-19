#pragma once

#include <Arduino.h>

void initControllerBridge();
bool hasDispenseController(const String &key);
bool confirmDispenseForComponent(const String &key, String &details, String &error);
