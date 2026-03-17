#include "device_api.h"

#include <WebServer.h>

#include "inventory_state.h"
#include "logging.h"
#include "web_server.h"

namespace {
void sendJson(int statusCode, const String &body) {
  server.send(statusCode, "application/json", body);
}

void sendError(int statusCode, const String &message) {
  sendJson(statusCode, "{\"error\":\"" + message + "\"}");
}

String componentJson(const Component &component) {
  String json = "{";
  json += "\"key\":\"" + String(component.key) + "\",";
  json += "\"name\":\"" + String(component.name) + "\",";
  json += "\"sku\":\"" + String(component.sku) + "\",";
  json += "\"stock\":" + String(component.stock) + ",";
  json += "\"min\":" + String(component.reorderLevel) + ",";
  json += "\"bin\":\"" + String(component.bin) + "\"";
  json += "}";
  return json;
}

void handleApiHealth() {
  sendJson(200, "{\"ok\":true}");
}

void handleApiInventory() {
  String payload;
  buildInventoryJson(payload);
  sendJson(200, payload);
}

void handleApiAuditLog() {
  String payload;
  buildAuditLogJson(payload);
  sendJson(200, payload);
}

void handleApiAdjustStock() {
  if (!server.hasArg("key") || !server.hasArg("delta")) {
    sendError(400, "key and delta are required");
    return;
  }

  String error;
  if (!adjustComponentStock(
          server.arg("key"),
          server.arg("delta").toInt(),
          server.hasArg("reason") ? server.arg("reason") : "Bestand angepasst",
          server.hasArg("details") ? server.arg("details") : "",
          error)) {
    sendError(409, error);
    return;
  }

  Component *component = findComponentByKey(server.arg("key"));
  sendJson(200, componentJson(*component));
}

void handleApiSetMin() {
  if (!server.hasArg("key") || !server.hasArg("value")) {
    sendError(400, "key and value are required");
    return;
  }

  String error;
  if (!setComponentMin(server.arg("key"), server.arg("value").toInt(), error)) {
    sendError(400, error);
    return;
  }

  Component *component = findComponentByKey(server.arg("key"));
  sendJson(200, componentJson(*component));
}

void handleApiRefreshStock() {
  if (!server.hasArg("key") || !server.hasArg("target")) {
    sendError(400, "key and target are required");
    return;
  }

  String error;
  if (!setComponentStock(
          server.arg("key"),
          server.arg("target").toInt(),
          server.hasArg("reason") ? server.arg("reason") : "Magazin ausgetauscht",
          server.hasArg("details") ? server.arg("details") : "",
          error)) {
    sendError(400, error);
    return;
  }

  Component *component = findComponentByKey(server.arg("key"));
  sendJson(200, componentJson(*component));
}

void handleApiFulfillOrder() {
  if (!server.hasArg("lines")) {
    sendError(400, "lines is required");
    return;
  }

  String keys[COMPONENT_COUNT];
  int quantities[COMPONENT_COUNT];
  size_t lineCount = 0;
  String rawLines = server.arg("lines");
  int start = 0;

  while (start < rawLines.length()) {
    int separator = rawLines.indexOf(',', start);
    if (separator < 0) {
      separator = rawLines.length();
    }

    String token = rawLines.substring(start, separator);
    token.trim();

    if (token.length() > 0) {
      const int split = token.indexOf(':');
      if (split <= 0 || split >= token.length() - 1) {
        sendError(400, "Invalid lines payload");
        return;
      }

      if (lineCount >= COMPONENT_COUNT) {
        sendError(400, "Too many line items");
        return;
      }

      keys[lineCount] = token.substring(0, split);
      quantities[lineCount] = token.substring(split + 1).toInt();
      ++lineCount;
    }

    start = separator + 1;
  }

  String error;
  if (!fulfillOrder(keys, quantities, lineCount, error)) {
    sendError(409, error);
    return;
  }

  String payload;
  buildInventoryJson(payload);
  sendJson(200, payload);
}
}  // namespace

void initDeviceApi() {
  server.on("/api/health", HTTP_GET, handleApiHealth);
  server.on("/api/inventory", HTTP_GET, handleApiInventory);
  server.on("/api/audit-log", HTTP_GET, handleApiAuditLog);
  server.on("/api/inventory/adjust", HTTP_POST, handleApiAdjustStock);
  server.on("/api/inventory/min", HTTP_POST, handleApiSetMin);
  server.on("/api/inventory/refresh", HTTP_POST, handleApiRefreshStock);
  server.on("/api/orders/fulfill", HTTP_POST, handleApiFulfillOrder);

  logInfo("Device API routes registered");
}
