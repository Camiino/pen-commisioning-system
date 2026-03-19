#include "remote_sync.h"

#include <Arduino.h>
#include <WiFiClient.h>

#if defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecureBearSSL.h>
#else
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#endif

#include "inventory_state.h"
#include "logging.h"
#include "wifi_manager.h"

namespace {
constexpr unsigned long COMMAND_POLL_INTERVAL_MS = 1000;
constexpr unsigned long STATE_PUSH_INTERVAL_MS = 30000;
constexpr unsigned long IDLE_POLL_LOG_INTERVAL_MS = 30000;
constexpr uint16_t REMOTE_HTTP_TIMEOUT_MS = 5000;
constexpr const char *PLACEHOLDER_DEVICE_TOKEN = "please-change-me";
constexpr const char *DEFAULT_REMOTE_DEVICE_TOKEN = "pcs-remote-sync-20260318";

const char *REMOTE_BACKEND_BASE = "https://automat-back.webeesign.com";
const char *REMOTE_DEVICE_TOKEN = DEFAULT_REMOTE_DEVICE_TOKEN;

WiFiClient plainClient;
#if defined(ESP8266)
BearSSL::WiFiClientSecure secureClient;
#else
WiFiClientSecure secureClient;
#endif

unsigned long lastCommandPollAt = 0;
unsigned long lastStatePushAt = 0;
unsigned long lastIdlePollLogAt = 0;
bool remoteSyncConfigured = false;
String remoteDeviceId;

struct PendingCommand {
  String id;
  String type;
  String key;
  String reason;
  String details;
  String lines;
  int delta = 0;
  int value = 0;
  int target = 0;
};

String jsonEscape(const String &value) {
  String escaped;
  escaped.reserve(value.length() + 8);

  for (size_t index = 0; index < value.length(); ++index) {
    const char current = value.charAt(index);
    if (current == '\\' || current == '"') {
      escaped += '\\';
      escaped += current;
      continue;
    }

    if (current == '\n') {
      escaped += "\\n";
      continue;
    }

    if (current == '\r') {
      continue;
    }

    escaped += current;
  }

  return escaped;
}

String componentJson(const Component &component) {
  String json = "{";
  json += "\"key\":\"" + String(component.key) + "\",";
  json += "\"name\":\"" + jsonEscape(String(component.name)) + "\",";
  json += "\"sku\":\"" + jsonEscape(String(component.sku)) + "\",";
  json += "\"stock\":" + String(component.stock) + ",";
  json += "\"min\":" + String(component.reorderLevel) + ",";
  json += "\"bin\":\"" + jsonEscape(String(component.bin)) + "\"";
  json += "}";
  return json;
}

String makeDefaultDeviceId() {
#if defined(ESP8266)
  return "pen-device-" + String(ESP.getChipId(), HEX);
#else
  return "pen-device-" + String(static_cast<uint32_t>(ESP.getEfuseMac()), HEX);
#endif
}

bool hasRemoteSyncConfig() {
  String backendBase = REMOTE_BACKEND_BASE;
  String deviceToken = REMOTE_DEVICE_TOKEN;
  backendBase.trim();
  deviceToken.trim();

  return backendBase.length() > 0 &&
         deviceToken.length() > 0 &&
         deviceToken != PLACEHOLDER_DEVICE_TOKEN;
}

bool beginHttp(HTTPClient &http, const String &url) {
  bool ok = false;
  if (url.startsWith("https://")) {
    ok = http.begin(secureClient, url);
  } else {
    ok = http.begin(plainClient, url);
  }

  if (ok) {
    http.setTimeout(REMOTE_HTTP_TIMEOUT_MS);
    http.setReuse(false);
  }

  return ok;
}

bool parseKeyValueLine(const String &line, String &key, String &value) {
  const int split = line.indexOf('=');
  if (split <= 0) {
    return false;
  }

  key = line.substring(0, split);
  value = line.substring(split + 1);
  key.trim();
  value.trim();
  return true;
}

bool parsePendingCommand(const String &body, PendingCommand &command) {
  int start = 0;

  while (start <= body.length()) {
    int end = body.indexOf('\n', start);
    if (end < 0) {
      end = body.length();
    }

    String line = body.substring(start, end);
    line.trim();
    if (line.length() > 0) {
      String key;
      String value;
      if (parseKeyValueLine(line, key, value)) {
        if (key == "id") command.id = value;
        else if (key == "type") command.type = value;
        else if (key == "key") command.key = value;
        else if (key == "reason") command.reason = value;
        else if (key == "details") command.details = value;
        else if (key == "lines") command.lines = value;
        else if (key == "delta") command.delta = value.toInt();
        else if (key == "value") command.value = value.toInt();
        else if (key == "target") command.target = value.toInt();
      }
    }

    if (end >= body.length()) {
      break;
    }
    start = end + 1;
  }

  return command.id.length() > 0 && command.type.length() > 0;
}

bool postJson(const String &path, const String &payload, int &statusCode, String &responseBody) {
  HTTPClient http;
  const String url = String(REMOTE_BACKEND_BASE) + path;
  if (!beginHttp(http, url)) {
    logError("Remote sync failed to begin request: " + url);
    return false;
  }

  http.addHeader("Content-Type", "application/json");
  http.addHeader("Connection", "close");
  http.addHeader("x-device-id", remoteDeviceId);
  http.addHeader("x-device-token", REMOTE_DEVICE_TOKEN);

  statusCode = http.POST(payload);
  if (statusCode > 0) {
    responseBody = http.getString();
  } else {
    responseBody = http.errorToString(statusCode);
  }
  http.end();
  return statusCode > 0;
}

bool getText(const String &path, int &statusCode, String &responseBody) {
  HTTPClient http;
  const String url = String(REMOTE_BACKEND_BASE) + path;
  if (!beginHttp(http, url)) {
    logError("Remote sync failed to begin poll: " + url);
    return false;
  }

  http.addHeader("Connection", "close");
  http.addHeader("x-device-id", remoteDeviceId);
  http.addHeader("x-device-token", REMOTE_DEVICE_TOKEN);

  statusCode = http.GET();
  if (statusCode > 0) {
    responseBody = http.getString();
  } else {
    responseBody = http.errorToString(statusCode);
  }
  http.end();
  return statusCode > 0;
}

String buildStatePayload() {
  String inventoryJson;
  buildInventoryJson(inventoryJson);
  const String localIp = getLocalNetworkIp();
  const String localBaseUrl = getLocalBaseUrl();

  String payload = "{";
  payload += "\"deviceId\":\"" + jsonEscape(remoteDeviceId) + "\",";
  payload += "\"inventory\":" + inventoryJson + ",";
  payload += "\"network\":{";
  payload += "\"localIp\":\"" + jsonEscape(localIp) + "\",";
  payload += "\"localBaseUrl\":\"" + jsonEscape(localBaseUrl) + "\"";
  payload += "}";
  payload += "}";
  return payload;
}

void pushState(const String &context) {
  if (!remoteSyncConfigured || !isWiFiConnected()) {
    return;
  }

  int statusCode = 0;
  String responseBody;
  if (!postJson("/api/device/state", buildStatePayload(), statusCode, responseBody)) {
    logError("Remote state push failed: " + responseBody);
    return;
  }

  if (statusCode >= 200 && statusCode < 300) {
    lastStatePushAt = millis();
    return;
  }

  logError("Remote state push rejected (" + String(statusCode) + ") during " + context + ": " + responseBody);
}

void sendCommandResult(const PendingCommand &command, bool ok, const String &responseJson, const String &errorMessage) {
  String inventoryJson;
  buildInventoryJson(inventoryJson);
  const String localIp = getLocalNetworkIp();
  const String localBaseUrl = getLocalBaseUrl();

  String payload = "{";
  payload += "\"deviceId\":\"" + jsonEscape(remoteDeviceId) + "\",";
  payload += "\"ok\":" + String(ok ? "true" : "false") + ",";
  payload += "\"inventory\":" + inventoryJson;
  payload += ",\"network\":{";
  payload += "\"localIp\":\"" + jsonEscape(localIp) + "\",";
  payload += "\"localBaseUrl\":\"" + jsonEscape(localBaseUrl) + "\"";
  payload += "}";
  if (ok) {
    payload += ",\"response\":" + (responseJson.length() > 0 ? responseJson : "null");
  } else {
    payload += ",\"error\":\"" + jsonEscape(errorMessage) + "\"";
  }
  payload += "}";

  int statusCode = 0;
  String responseBody;
  const String path = "/api/device/commands/" + command.id + "/result";
  if (!postJson(path, payload, statusCode, responseBody)) {
    logError("Failed to send command result: " + responseBody);
    return;
  }

  if (statusCode < 200 || statusCode >= 300) {
    logError("Command result rejected (" + String(statusCode) + "): " + responseBody);
    return;
  }

  logInfo("Remote command " + command.id + " result sent");
}

bool executeFulfillOrder(const String &encodedLines, String &responseJson, String &error) {
  String keys[COMPONENT_COUNT];
  int quantities[COMPONENT_COUNT];
  size_t lineCount = 0;
  int start = 0;

  while (start < encodedLines.length()) {
    int separator = encodedLines.indexOf(',', start);
    if (separator < 0) {
      separator = encodedLines.length();
    }

    String token = encodedLines.substring(start, separator);
    token.trim();

    if (token.length() > 0) {
      const int split = token.indexOf(':');
      if (split <= 0 || split >= token.length() - 1) {
        error = "Invalid lines payload";
        return false;
      }

      if (lineCount >= COMPONENT_COUNT) {
        error = "Too many line items";
        return false;
      }

      keys[lineCount] = token.substring(0, split);
      quantities[lineCount] = token.substring(split + 1).toInt();
      ++lineCount;
    }

    start = separator + 1;
  }

  if (!fulfillOrder(keys, quantities, lineCount, error)) {
    return false;
  }

  buildInventoryJson(responseJson);
  return true;
}

void executeCommand(const PendingCommand &command) {
  String error;
  String responseJson;
  bool ok = false;

  logInfo("Executing remote command " + command.id + " (" + command.type + ")");

  if (command.type == "adjustStock") {
    ok = adjustComponentStock(command.key, command.delta, command.reason, command.details, error);
    if (ok) {
      Component *component = findComponentByKey(command.key);
      responseJson = component ? componentJson(*component) : "null";
    }
  } else if (command.type == "setMin") {
    ok = setComponentMin(command.key, command.value, error);
    if (ok) {
      Component *component = findComponentByKey(command.key);
      responseJson = component ? componentJson(*component) : "null";
    }
  } else if (command.type == "refreshStock") {
    ok = setComponentStock(command.key, command.target, command.reason, command.details, error);
    if (ok) {
      Component *component = findComponentByKey(command.key);
      responseJson = component ? componentJson(*component) : "null";
    }
  } else if (command.type == "fulfillOrder") {
    ok = executeFulfillOrder(command.lines, responseJson, error);
  } else {
    error = "Unsupported command type";
  }

  if (!ok) {
    logError("Remote command " + command.id + " failed: " + error);
  }
  sendCommandResult(command, ok, responseJson, error);
}

void pollCommands() {
  if (!remoteSyncConfigured || !isWiFiConnected()) {
    return;
  }

  int statusCode = 0;
  String responseBody;
  if (!getText("/api/device/commands/poll", statusCode, responseBody)) {
    logError("Remote command poll failed: " + responseBody);
    return;
  }

  if (statusCode == 204) {
    const unsigned long now = millis();
    if (now - lastIdlePollLogAt >= IDLE_POLL_LOG_INTERVAL_MS) {
      lastIdlePollLogAt = now;
      logInfo("Remote command poll OK (no pending command)");
    }
    return;
  }

  if (statusCode < 200 || statusCode >= 300) {
    logError("Remote command poll rejected (" + String(statusCode) + "): " + responseBody);
    return;
  }

  PendingCommand command;
  if (!parsePendingCommand(responseBody, command)) {
    logError("Could not parse remote command payload");
    return;
  }

  logInfo("Claimed remote command " + command.id + " (" + command.type + ")");
  executeCommand(command);
}
}  // namespace

void initRemoteSync() {
  secureClient.setInsecure();
  remoteDeviceId = makeDefaultDeviceId();
  remoteSyncConfigured = hasRemoteSyncConfig();

  if (!remoteSyncConfigured) {
    logInfo("Remote sync disabled. Set REMOTE_DEVICE_TOKEN in remote_sync.cpp to enable it.");
    return;
  }

  logInfo("Remote sync enabled for device " + remoteDeviceId);
  pushState("startup");
}

void handleRemoteSync() {
  if (!remoteSyncConfigured || !isWiFiConnected()) {
    return;
  }

  const unsigned long now = millis();

  if (now - lastCommandPollAt >= COMMAND_POLL_INTERVAL_MS) {
    lastCommandPollAt = now;
    pollCommands();
  }

  if (now - lastStatePushAt >= STATE_PUSH_INTERVAL_MS) {
    pushState("periodic sync");
  }
}
