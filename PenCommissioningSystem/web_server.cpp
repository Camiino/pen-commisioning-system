#include "web_server.h"
#include "i2c_manager.h"

// web server and dns
WebServer server(80);
DNSServer dnsServer;

// Base64 Icons for pen components
const String shaftIcon = "data:image/svg+xml;base64,PD94bWwgdmVyc2lvbj0iMS4wIiBlbmNvZGluZz0iVVRGLTgiIHN0YW5kYWxvbmU9Im5vIj8+CjwhLS0gQ3JlYXRlZCB3aXRoIElua3NjYXBlIChodHRwOi8vd3d3Lmlua3NjYXBlLm9yZy8pIC0tPgoKPHN2ZwogICB3aWR0aD0iMjEuOTY2ODUiCiAgIGhlaWdodD0iMy4yNDg2MTg2IgogICB2aWV3Qm94PSIwIDAgMjEuOTY2ODUxIDMuMjQ4NjE4NiIKICAgdmVyc2lvbj0iMS4xIgogICBpZD0ic3ZnMSIKICAgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIgogICB4bWxuczpzdmc9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KICA8ZGVmcwogICAgIGlkPSJkZWZzMSIgLz4KICA8ZwogICAgIGlkPSJsYXllcjEiCiAgICAgdHJhbnNmb3JtPSJ0cmFuc2xhdGUoLTUuMDE2NTc0NiwtMTIuODYxODc5KSI+CiAgICA8cGF0aAogICAgICAgc3R5bGU9ImZpbGw6IzAwMDAwMCIKICAgICAgIGQ9Ik0gMjMuOTc3ODk5LDEzLjM0ODA2OCAyMy45MzM3MDEsMTIuOTk0NDc3IDkuNjc5NTU4LDEyLjkyODE3NyA1LjA4Mjg3MjksMTQuMzg2NzQgOS42MzUzNTkxLDE2IDIzLjk3NzksMTUuOTc3OTAxIGwgLTFlLTYsLTAuMzk3NzkgMi44OTUwMjksMC4wMjIxIDAuMDIyMSwtMi4yNTQxNDQgYyAtMS4yNDk4NzgsLTAuMDAzMyAtMS44OTcxMzEsMC4wMTgyNyAtMi45MTcxMjcsMmUtNiB6IgogICAgICAgaWQ9InBhdGgyIiAvPgogICAgPHBhdGgKICAgICAgIHN0eWxlPSJmaWxsOiMwMDAwMDAiCiAgICAgICBkPSJtIDIzLjk3Nzg5OSwxMy4zNDgwNjggLTAuMDQ0MiwtMC4zNTM1OTEgLTE0LjI1NDE0MywtMC4wNjYzIC00LjA2NjI5ODQsMS41NjkwNiBMIDkuNjM1MzU5MSwxNiAyMy45Nzc5LDE1Ljk3NzkwMSBsIC0xZS02LC0wLjM5Nzc5IDIuODk1MDI5LDAuMDIyMSAwLjAyMjEsLTIuMjU0MTQ0IGMgLTEuMjQ5ODc4LC0wLjAwMzMgLTEuODk3MTMxLDAuMDE4MjcgLTIuOTE3MTI3LDJlLTYgeiIKICAgICAgIGlkPSJwYXRoMyIgLz4KICA8L2c+Cjwvc3ZnPgo=";
// placeholder icons
const String mineIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String clipIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iI2ZmMDAwMCIvPjwvc3ZnPg==";
const String springIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String buttonIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String tipIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";
const String ringIcon = "data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iODAwIiBoZWlnaHQ9IjQwMCIgdmlld0JveD0iMCAwIDggMCA4MDAiIHZpZXdCb3g9IjAgMCA4MCA4MDAiIGVuYWJsZT0icmVzaXplIj48cGF0aCBkPSJNMTQwMCAwMDUwMDAwMDAwMDAwMDAwMDAgZmlsbD0iIzAwMDAwMCIvPjwvc3ZnPg==";

// define struct for saving stock
struct Component {
  String name;
  String icon;
  int stock;
  int reorderLevel;
};

Component components[7] = {
  {"Shaft", "0", 100, 20},  // Using index 0 to reference shaftIcon
  {"Mine", "1", 150, 15},
  {"Clip", "2", 200, 10},
  {"Spring", "3", 80, 5},
  {"Button", "4", 120, 10},
  {"Tip", "5", 90, 5},
  {"Ring", "6", 250, 30}
};

void initEEPROM() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    logError("EEPROM initialisation failed!");
  }
  delay(200);
  logInfo("EEPROM initialized");
}

void saveStockToEEPROM() {
  for (int i = 0; i < 7; i++) {
    int address = i * sizeof(int);
    EEPROM.writeInt(address, components[i].stock);
    logInfo("Save " + String(components[i].stock) + " item(s) of " + String(components[i].name) + " to EEPROM at address " + String(address));
  }
  EEPROM.commit();
  logInfo("Saved stock quantities to EEPROM");
}

void loadStockFromEEPROM() {
  for (int i = 0; i < 7; i++) {
    int address = i * sizeof(int);
    int value;
    EEPROM.get(address, value);
    if (value >= 0) {  // Only update if there's a valid value
      components[i].stock = value;
    }
    logInfo("Load " + String(components[i].stock) + " item(s) of " + String(components[i].name) + " from EEPROM at address " + String(address));
  }
  logInfo("Loaded stock quantities from EEPROM");
}

// structure for saving logs
struct LogEntry {
  String timestamp;
  String component;
  int quantity;
  String action;
};
LogEntry logEntries[50];
int logCount;

// structure for EEPROM saving of logs (compact)
struct EEPROMLogEntry {
  uint32_t timestamp;  // Millis seit Start
  char component[10]; // Max. 9 Zeichen + Nullterminator
  int quantity;
  char action[10];    // "dispensed" oder "added"
};

void saveLogsToEEPROM() {
  // save only the last MAX_LOG_ENTRIES values
  int startIndex = max(0, logCount - MAX_LOG_ENTRIES);
  for (int i = startIndex; i < logCount; i++) {
    int logIndex = i - startIndex;
    int address = LOG_START_ADDRESS + logIndex * sizeof(EEPROMLogEntry);

    EEPROMLogEntry entry;
    entry.timestamp = logEntries[i].timestamp.substring(0, logEntries[i].timestamp.indexOf('s')).toInt();  // extract millis
    strncpy(entry.component, logEntries[i].component.c_str(), 9);
    entry.component[9] = '\0';  // zero terminator
    entry.quantity = logEntries[i].quantity;
    strncpy(entry.action, logEntries[i].action.c_str(), 9);
    entry.action[9] = '\0';     // zero terminator

    EEPROM.put(address, entry);
  }
  EEPROM.commit();
  logInfo("Saved " + String(min(logCount, MAX_LOG_ENTRIES)) + " logs to EEPROM");
}

void loadLogsFromEEPROM() {
  logCount = 0;  // reset

  for (int i = 0; i < MAX_LOG_ENTRIES; i++) {
    int address = LOG_START_ADDRESS + i * sizeof(EEPROMLogEntry);
    EEPROMLogEntry entry;
    EEPROM.get(address, entry);

    // check if entry is valid (timestamp != 0)
    if (entry.timestamp > 0 && entry.timestamp < 0xFFFFFFFF) {
      logEntries[logCount].timestamp = String(entry.timestamp) + "s";
      logEntries[logCount].component = String(entry.component);
      logEntries[logCount].quantity = entry.quantity;
      logEntries[logCount].action = String(entry.action);
      logCount++;
    }
  }
  logInfo("Loaded " + String(logCount) + " log entries from EEPROM");
}

void addLogEntry(String component, int quantity, String action) {
  if (logCount < 50) {
    logEntries[logCount].timestamp = getCurrentTimestamp();
    logEntries[logCount].component = component;
    logEntries[logCount].quantity = quantity;
    logEntries[logCount].action = action;
    logCount++;
  } else {
    // Shift all entries down by one
    for (int i = 1; i < 50; i++) {
      logEntries[i-1] = logEntries[i];
    }
    // Add new entry at the end
    logEntries[49].timestamp = getCurrentTimestamp();
    logEntries[49].component = component;
    logEntries[49].quantity = quantity;
    logEntries[49].action = action;
  }
  saveLogsToEEPROM();
}

String getCurrentTimestamp() {
  // Simple timestamp for demo purposes
  // In a real application, you would use an RTC module
  return String(millis() / 1000) + "s";
}

void initWebServer() {
  // load from EEPROM
  loadStockFromEEPROM();
  loadLogsFromEEPROM();

  // standard pages
  server.on("/", handleRoot);
  server.on("/order", handleOrder);
  server.on("/admin", handleAdmin);
  server.on("/update-stock", handleUpdateStock);
  server.onNotFound(handleNotFound);

  server.begin();
  logInfo("Webserver started on port 80");
}

void handleWebServer() {
  server.handleClient();
  delay(2);
  if (!isWiFiConnected()) dnsServer.processNextRequest();
}

void handleRoot() {
    // Array of all icons for easy reference
  const String icons[] = {shaftIcon, mineIcon, clipIcon, springIcon, buttonIcon, tipIcon, ringIcon};

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
  html += ".header-icon { font-size: 40px; margin-bottom: 10px; }";
  html += ".component-icon img, .component-icon-table img { max-width: 70px; max-height: 70px; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Pen Commissioning System</h1>";

  // Order Form Card
  html += "<div class='card'>";
  html += "<h2>New Order</h2>";
  html += "<form method='POST' action='/order'>";

  for (int i = 0; i < 7; i++) {
    html += "<div class='component-row'>";
    html += "<div class='component-icon'>";

    // Add icon if available
    if (components[i].icon != "") {
      html += "<img src='" + icons[components[i].icon.toInt()] + "' width='70'>";
    }

    html += "</div>";
    html += "<div class='component-name'>" + components[i].name + "</div>";
    html += "<input class='form-input' type='number' name='" + components[i].name + "' value='0'>";
    html += "</div>";
  }

  html += "<div style='text-align: center; margin-top: 20px;'>";
  html += "<button type='submit'>Submit Order</button>";
  html += "</div></form></div>";

  // Stock Table Card
  html += "<div class='card'>";
  html += "<h2>Current Stock</h2>";
  html += "<table>";
  html += "<tr><th>Component</th><th>Stock</th><th>Status</th></tr>";

  for (int i = 0; i < 7; i++) {
    html += "<tr>";
    html += "<td>";

    // Add icon if available
    if (components[i].icon != "") {
      html += "<span class='component-icon-table'><img src='" + icons[components[i].icon.toInt()] + "' width='70'></span>";
    }

    html += components[i].name + "</td>";
    html += "<td>" + String(components[i].stock) + "</td>";
    html += "<td class='";
    if (components[i].stock < components[i].reorderLevel) {
      html += "low-stock";
    } else {
      html += "status-ok";
    }
    html += "'>";
    if (components[i].stock < components[i].reorderLevel) {
      html += "LOW!";
    } else {
      html += "OK";
    }
    html += "</td></tr>";
  }

  html += "</table></div>";

  // Navigation
  html += "<div style='text-align: center;'>";
  html += "<a href='/admin' class='nav-button'><button>Admin Panel</button></a>";
  html += "</div></div></body></html>";

  server.send(200, "text/html", html);
}

void handleOrder() {
    // Array of all icons for easy reference
  const String icons[] = {shaftIcon, mineIcon, clipIcon, springIcon, buttonIcon, tipIcon, ringIcon};

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
  response += "</style></head><body>";
  response += "<div class='container'>";
  response += "<h1>Order Processed</h1>";
  response += "<div class='card'>";

  bool anyOrdered = false;
  for (int i = 0; i < 7; i++) {
    if (server.hasArg(components[i].name)) {
      int ordered = server.arg(components[i].name).toInt();
      if (ordered > 0) {
        anyOrdered = true;
        response += "<div class='order-item'>";
        response += "<div class='order-icon'>";

        // Add icon if available
        if (components[i].icon != "") {
          response += "<img src='" + icons[components[i].icon.toInt()] + "' width='70'>";
        }

        response += "</div>";
        if (ordered > components[i].stock) {
          response += "<div>Error: Not enough " + components[i].name + " in stock</div>";
        } else {
          components[i].stock -= ordered;
          // Add log entry
          addLogEntry(components[i].name, ordered, "dispensed");
          logInfo("Dispensed " + String(ordered) + " item(s) of " + components[i].name);
          // Send actual dispense command to module
          if (components[i].name == "Shaft") {
              for (int j = 0; j < ordered; j++) {
                  dispenseFromModule(0x11);  // Adresse des Shaft-Moduls
                  delay(2000);  // Wartezeit für den Ausgabemechanismus
              }
          }
          // Save to EEPROM
          saveStockToEEPROM();
          response += "<div>Dispensed " + String(ordered) + " " + components[i].name + "(s)</div>";
        }
        response += "</div>";
      }
    }
  }

  if (!anyOrdered) {
    response += "<p>No components ordered</p>";
  }

  response += "<div style='text-align: center; margin-top: 20px;'>";
  response += "<a href='/'><button>Back to Main Page</button></a>";
  response += "</div></div></div></body></html>";
  server.send(200, "text/html", response);
}

void handleAdmin() {
    // Array of all icons for easy reference
  const String icons[] = {shaftIcon, mineIcon, clipIcon, springIcon, buttonIcon, tipIcon, ringIcon};

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
  html += "table { width: 100%; border-collapse: collapse; margin-top: 20px; }";
  html += "th, td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }";
  html += "th { background-color: #f2f2f2; }";
  html += ".low-stock { color: #e74c3c; font-weight: bold; }";
  html += ".status-ok { color: #27ae60; }";
  html += ".component-icon { margin-right: 15px; width: 80px; text-align: center; }";
  html += ".component-icon img { max-width: 70px; max-height: 70px; }";
  html += ".log-entry { padding: 8px; border-bottom: 1px solid #eee; }";
  html += ".log-time { color: #7f8c8d; font-size: 0.9em; }";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Admin Panel</h1>";

  // Stock Update Form
  html += "<div class='card'>";
  html += "<h2>Update Stock</h2>";
  html += "<form method='POST' action='/update-stock'>";

  for (int i = 0; i < 7; i++) {
    html += "<div class='form-row'>";
    html += "<div class='form-label'>" + components[i].name + ":</div>";
    html += "<input class='form-input' type='number' name='" + components[i].name + "_stock' value='" + String(components[i].stock) + "'>";
    html += "<span> (Current: " + String(components[i].stock) + ")</span>";
    html += "</div>";
  }

  html += "<div style='text-align: center; margin-top: 20px;'>";
  html += "<button type='submit'>Update Stock</button>";
  html += "</div></form></div>";

  // Activity Log
  html += "<div class='card'>";
  html += "<h2>Activity Log</h2>";
  html += "<div style='max-height: 300px; overflow-y: auto;'>";

  for (int i = logCount - 1; i >= 0 && i >= logCount - 20; i--) {
    if (i >= 0) {
      html += "<div class='log-entry'>";
      html += "<div class='log-time'>" + logEntries[i].timestamp + "</div>";
      html += "<div>" + logEntries[i].action + " " + String(logEntries[i].quantity) + " " + logEntries[i].component + "</div>";
      html += "</div>";
    }
  }

  html += "</div></div>";

  // Navigation
  html += "<div style='text-align: center;'>";
  html += "<a href='/'><button>Back to Main Page</button></a>";
  html += "</div></div></body></html>";

  server.send(200, "text/html", html);
}

void handleUpdateStock() {
    for (int i = 0; i < 7; i++) {
    String argName = components[i].name + "_stock";
    if (server.hasArg(argName)) {
      int newStock = server.arg(argName).toInt();
      if (newStock >= 0) {
        // Add log entry for stock change
        if (newStock != components[i].stock) {
          int difference = newStock - components[i].stock;
          if (difference > 0) {
            addLogEntry(components[i].name, difference, "added");
            logInfo("Added " + String(difference) + " item(s) of " + components[i].name);
          } else if (difference < 0) {
            addLogEntry(components[i].name, -difference, "removed");
            logInfo("Removed " + String(-difference) + " item(s) of " + components[i].name);
          }
        }
        components[i].stock = newStock;
      }
    }
  }

  // Save to EEPROM
  saveStockToEEPROM();

  String response = "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>Stock Updated</title>";
  response += "<style>";
  response += "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; margin: 0; padding: 20px; background-color: #f5f7fa; color: #333; }";
  response += ".container { max-width: 800px; margin: 0 auto; }";
  response += ".card { background: white; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); padding: 20px; margin-bottom: 20px; }";
  response += "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }";
  response += "button { background-color: #3498db; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; font-size: 16px; transition: background-color 0.3s; }";
  response += "button:hover { background-color: #2980b9; }";
  response += "</style></head><body>";
  response += "<div class='container'>";
  response += "<h1>Stock Updated</h1>";
  response += "<div class='card'>";
  response += "<p>Stock levels have been updated successfully.</p>";
  response += "<div style='text-align: center; margin-top: 20px;'>";
  response += "<a href='/admin'><button>Back to Admin Panel</button></a>";
  response += "</div></div></div></body></html>";

  server.send(200, "text/html", response);
}

void handleNotFound() {
  server.send(404, "text/html", "<!DOCTYPE html><html><head><meta charset='UTF-8'><title>404 Not Found</title><style>body { font-family: Arial; text-align: center; padding: 50px; } h1 { color: #e74c3c; }</style></head><body><h1>404 Not Found</h1><p>The page you requested was not found</p><a href='/'>Return to home page</a></body></html>");
}
