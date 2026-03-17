#include "web_server.h"

#include "device_api.h"

// web server and dns
WebServer server(80);
DNSServer dnsServer;

// Base64 icons for pen components
const String shaftIcon = "data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhLS0gQ3JlYXRlZCB3aXRoIElua3NjYXBlIChodHRwOi8vd3d3Lmlua3NjYXBlLm9yZy8pIC0tPgoKPHN2ZwogICB3aWR0aD0iMjEuOTY2ODUiCiAgIGhlaWdodD0iMy4yNDg2MTg2IgogICB2aWV3Qm94PSIwIDAgMjEuOTY2ODUxIDMuMjQ4NjE4NiIKICAgdmVyc2lvbj0iMS4xIgogICBpZD0ic3ZnMSIKICAgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIgogICB4bWxuczpzdmc9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KICA8ZGVmcwogICAgIGlkPSJkZWZzMSIgLz4KICA8ZwogICAgIGlkPSJsYXllcjEiCiAgICAgdHJhbnNmb3JtPSJ0cmFuc2xhdGUoLTUuMDE2NTc0NiwtMTIuODYxODc5KSI+CiAgICA8cGF0aAogICAgICAgc3R5bGU9ImZpbGw6IzAwMDAwMCIKICAgICAgIGQ9Ik0gMjMuOTc3ODk5LDEzLjM0ODA2OCAyMy45MzM3MDEsMTIuOTk0NDc3IDkuNjc5NTU4LDEyLjkyODE3NyA1LjA4Mjg3MjksMTQuMzg2NzQgOS42MzUzNTkxLDE2IDIzLjk3NzksMTUuOTc3OTAxIGwgLTFlLTYsLTAuMzk3NzkgMi44OTUwMjksMC4wMjIxIDAuMDIyMSwtMi4yNTQxNDQgYyAtMS4yNDk4NzgsLTAuMDAzMyAtMS44OTcxMzEsMC4wMTgyNyAtMi45MTcxMjcsMmUtNiB6IgogICAgICAgaWQ9InBhdGgyIiAvPgogICAgPHBhdGgKICAgICAgIHN0eWxlPSJmaWxsOiMwMDAwMDAiCiAgICAgICBkPSJtIDIzLjk3Nzg5OSwxMy4zNDgwNjggLTAuMDQ0MiwtMC4zNTM1OTEgLTE0LjI1NDE0MywtMC4wNjYzIC00LjA2NjI5ODQsMS41NjkwNiBMIDkuNjM1MzU5MSwxNiAyMy45Nzc5LDE1Ljk3NzkwMSBsIC0xZS02LC0wLjM5Nzc5IDIuODk1MDI5LDAuMDIyMSAwLjAyMjEsLTIuMjU0MTQ0IGMgLTEuMjQ5ODc4LC0wLjAwMzMgLTEuODk3MTMxLDAuMDE4MjcgLTIuOTE3MTI3LDJlLTYgeiIKICAgICAgIGlkPSJwYXRoMyIgLz4KICA8L2c+Cjwvc3ZnPgo=";
const String mineIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String clipIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iI2ZmMDAwMCIvPjwvc3ZnPg==";
const String springIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String buttonIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String tipIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String ringIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";

namespace {
const String icons[] = {shaftIcon, mineIcon, clipIcon, springIcon, buttonIcon, tipIcon, ringIcon};

String getIconMarkup(const char *iconId) {
  const int index = String(iconId).toInt();
  if (index < 0 || index >= static_cast<int>(sizeof(icons) / sizeof(icons[0]))) {
    return "";
  }

  return "<img src='" + icons[index] + "' width='70' alt='icon'>";
}
}  // namespace

void initWebServer() {
  initDeviceApi();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/order", HTTP_POST, handleOrder);
  server.on("/admin", HTTP_GET, handleAdmin);
  server.on("/update-stock", HTTP_POST, handleUpdateStock);
  server.onNotFound(handleNotFound);

  server.begin();
  logInfo("Webserver started on port 80");
}

void handleWebServer() {
  server.handleClient();
  delay(2);

  if (!isWiFiConnected()) {
    dnsServer.processNextRequest();
  }
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Pen Commissioning System</title>";
  html += "<style>";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f7fa; color: #333; }";
  html += ".container { max-width: 800px; margin: 0 auto; }";
  html += ".card { background: white; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 20px; margin-bottom: 20px; }";
  html += "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }";
  html += "h2 { color: #3498db; margin-top: 30px; margin-bottom: 15px; }";
  html += ".component-row { display: flex; align-items: center; margin-bottom: 10px; padding: 8px; border-radius: 4px; transition: background-color 0.2s; }";
  html += ".component-row:hover { background-color: #f8f9fa; }";
  html += ".component-icon { margin-right: 15px; width: 80px; text-align: center; }";
  html += ".component-name { flex-grow: 1; font-weight: 500; }";
  html += ".form-input { padding: 8px; border: 1px solid #ddd; border-radius: 4px; width: 80px; }";
  html += "button { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; font-size: 16px; transition: background-color 0.3s; }";
  html += "button:hover { background-color: #2980b9; }";
  html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; }";
  html += "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }";
  html += "th { background-color: #f2f2f2; }";
  html += ".low-stock { color: #e74c3c; font-weight: bold; }";
  html += ".status-ok { color: #27ae60; }";
  html += ".component-icon-table { font-size: 20px; margin-right: 10px; }";
  html += ".nav-button { display: inline-block; margin-top: 20px; }";
  html += ".component-icon img, .component-icon-table img { max-width: 70px; max-height: 70px; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Pen Commissioning System</h1>";

  html += "<div class='card'>";
  html += "<h2>New Order</h2>";
  html += "<form method='POST' action='/order'>";

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    html += "<div class='component-row'>";
    html += "<div class='component-icon'>" + getIconMarkup(components[index].iconId) + "</div>";
    html += "<div class='component-name'>" + String(components[index].name) + "</div>";
    html += "<input class='form-input' type='number' min='0' name='" + String(components[index].name) + "' value='0'>";
    html += "</div>";
  }

  html += "<div style='text-align: center; margin-top: 20px;'>";
  html += "<button type='submit'>Submit Order</button>";
  html += "</div></form></div>";

  html += "<div class='card'>";
  html += "<h2>Current Stock</h2>";
  html += "<table>";
  html += "<tr><th>Component</th><th>Stock</th><th>Status</th></tr>";

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    html += "<tr>";
    html += "<td><span class='component-icon-table'>" + getIconMarkup(components[index].iconId) + "</span>" + String(components[index].name) + "</td>";
    html += "<td>" + String(components[index].stock) + "</td>";
    html += "<td class='";
    html += components[index].stock <= components[index].reorderLevel ? "low-stock" : "status-ok";
    html += "'>";
    html += components[index].stock <= components[index].reorderLevel ? "LOW!" : "OK";
    html += "</td></tr>";
  }

  html += "</table></div>";
  html += "<div style='text-align: center;'>";
  html += "<a href='/admin' class='nav-button'><button>Admin Panel</button></a>";
  html += "</div></div></body></html>";

  server.send(200, "text/html", html);
}

void handleOrder() {
  String keys[COMPONENT_COUNT];
  int quantities[COMPONENT_COUNT];
  int lineCount = 0;

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    if (!server.hasArg(components[index].name)) {
      continue;
    }

    const int ordered = server.arg(components[index].name).toInt();
    if (ordered <= 0) {
      continue;
    }

    keys[lineCount] = components[index].key;
    quantities[lineCount] = ordered;
    ++lineCount;
  }

  String response = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Order Received</title>";
  response += "<style>";
  response += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f7fa; color: #333; }";
  response += ".container { max-width: 800px; margin: 0 auto; }";
  response += ".card { background: white; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 20px; margin-bottom: 20px; }";
  response += "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }";
  response += "button { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; font-size: 16px; transition: background-color 0.3s; }";
  response += "button:hover { background-color: #2980b9; }";
  response += ".order-item { display: flex; align-items: center; margin-bottom: 10px; padding: 8px; border-radius: 4px; }";
  response += ".order-icon { margin-right: 15px; width: 80px; text-align: center; }";
  response += ".order-icon img { max-width: 70px; max-height: 70px; }";
  response += ".error { color: #e74c3c; font-weight: 600; }";
  response += "</style></head><body>";
  response += "<div class='container'><h1>Order Processed</h1><div class='card'>";

  if (lineCount == 0) {
    response += "<p>No components ordered.</p>";
  } else {
    String error;
    if (!fulfillOrder(keys, quantities, static_cast<size_t>(lineCount), error)) {
      response += "<p class='error'>" + error + "</p>";
    } else {
      for (int index = 0; index < lineCount; ++index) {
        Component *component = findComponentByKey(keys[index]);
        response += "<div class='order-item'>";
        response += "<div class='order-icon'>" + getIconMarkup(component->iconId) + "</div>";
        response += "<div>Dispensed " + String(quantities[index]) + " " + String(component->name) + "(s)</div>";
        response += "</div>";
      }
    }
  }

  response += "<div style='text-align: center; margin-top: 20px;'>";
  response += "<a href='/'><button>Back to Main Page</button></a>";
  response += "</div></div></div></body></html>";

  server.send(200, "text/html", response);
}

void handleAdmin() {
  String html = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Admin Panel</title>";
  html += "<style>";
  html += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f7fa; color: #333; }";
  html += ".container { max-width: 800px; margin: 0 auto; }";
  html += ".card { background: white; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 20px; margin-bottom: 20px; }";
  html += "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }";
  html += "h2 { color: #3498db; margin-top: 30px; margin-bottom: 15px; }";
  html += ".form-row { display: flex; align-items: center; margin-bottom: 10px; }";
  html += ".form-label { width: 200px; font-weight: 500; }";
  html += ".form-input { padding: 8px; border: 1px solid #ddd; border-radius: 4px; width: 80px; }";
  html += "button { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; font-size: 16px; transition: background-color 0.3s; }";
  html += "button:hover { background-color: #2980b9; }";
  html += ".log-entry { padding: 8px; border-bottom: 1px solid #eee; }";
  html += ".log-time { color: #7f8c8d; font-size: 0.9em; }";
  html += "</style></head><body>";
  html += "<div class='container'><h1>Admin Panel</h1>";

  html += "<div class='card'><h2>Update Stock</h2><form method='POST' action='/update-stock'>";

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    html += "<div class='form-row'>";
    html += "<div class='form-label'>" + String(components[index].name) + ":</div>";
    html += "<input class='form-input' type='number' min='0' name='" + String(components[index].name) + "_stock' value='" + String(components[index].stock) + "'>";
    html += "<span> (Current: " + String(components[index].stock) + ")</span>";
    html += "</div>";
  }

  html += "<div style='text-align: center; margin-top: 20px;'><button type='submit'>Update Stock</button></div>";
  html += "</form></div>";

  html += "<div class='card'><h2>Activity Log</h2><div style='max-height: 300px; overflow-y: auto;'>";

  for (int index = logCount - 1; index >= 0; --index) {
    html += "<div class='log-entry'>";
    html += "<div class='log-time'>" + String(logEntries[index].timestampSeconds) + "s</div>";
    html += "<div>" + logEntries[index].action + " " + logEntries[index].item + " (" + String(logEntries[index].quantity) + ")</div>";
    if (logEntries[index].details.length() > 0) {
      html += "<div>" + logEntries[index].details + "</div>";
    }
    html += "</div>";
  }

  html += "</div></div>";
  html += "<div style='text-align: center;'><a href='/'><button>Back to Main Page</button></a></div>";
  html += "</div></body></html>";

  server.send(200, "text/html", html);
}

void handleUpdateStock() {
  String summary;
  String errors;

  for (int index = 0; index < COMPONENT_COUNT; ++index) {
    const String fieldName = String(components[index].name) + "_stock";
    if (!server.hasArg(fieldName)) {
      continue;
    }

    const int newStock = server.arg(fieldName).toInt();
    if (newStock < 0) {
      errors += "<p>Invalid stock for " + String(components[index].name) + "</p>";
      continue;
    }

    if (newStock == components[index].stock) {
      continue;
    }

    String error;
    const String details = "Bestand: " + String(components[index].stock) + " -> " + String(newStock);
    if (!setComponentStock(components[index].key, newStock, "Bestand angepasst", details, error)) {
      errors += "<p>" + error + " (" + String(components[index].name) + ")</p>";
      continue;
    }

    summary += "<p>" + String(components[index].name) + " updated to " + String(newStock) + ".</p>";
  }

  if (summary.length() == 0 && errors.length() == 0) {
    summary = "<p>No stock values changed.</p>";
  }

  String response = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Stock Updated</title>";
  response += "<style>";
  response += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f7fa; color: #333; }";
  response += ".container { max-width: 800px; margin: 0 auto; }";
  response += ".card { background: white; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 20px; margin-bottom: 20px; }";
  response += "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }";
  response += "button { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; font-size: 16px; transition: background-color 0.3s; }";
  response += "button:hover { background-color: #2980b9; }";
  response += ".error { color: #e74c3c; font-weight: 600; }";
  response += "</style></head><body><div class='container'><h1>Stock Updated</h1><div class='card'>";
  response += summary;
  if (errors.length() > 0) {
    response += "<div class='error'>" + errors + "</div>";
  }
  response += "<div style='text-align: center; margin-top: 20px;'><a href='/admin'><button>Back to Admin Panel</button></a></div>";
  response += "</div></div></body></html>";

  server.send(200, "text/html", response);
}

void handleNotFound() {
  server.send(404, "text/html", "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>404 Not Found</title><style>body { font-family: Arial; text-align: center; padding: 50px; } h1 { color: #e74c3c; }</style></head><body><h1>404 Not Found</h1><p>The page you requested was not found</p><a href='/'>Return to home page</a></body></html>");
}
