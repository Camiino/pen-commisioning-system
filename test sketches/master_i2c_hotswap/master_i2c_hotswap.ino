#include <Wire.h>
#include <vector>  // Für dynamische Liste der Slaves

// Struktur für Slave-Informationen
struct SlaveInfo {
  byte address;
  String moduleName;
};

std::vector<SlaveInfo> slaves;  // Liste der bekannten Slaves

void setup() {
  Serial.begin(115200);
  Wire.begin();  // I²C als Master starten
  scanForSlaves();  // Initialen Scan durchführen
}

void loop() {
  // Alle 5 Sekunden nach neuen Slaves scannen
  static unsigned long lastScan = 0;
  if (millis() - lastScan > 5000) {
    lastScan = millis();
    scanForSlaves();
  }

  // Beispiel: Alle Slaves ansteuern
  for (const auto& slave : slaves) {
    Serial.print("Steuere ");
    Serial.print(slave.moduleName);
    Serial.print(" (0x");
    Serial.print(slave.address, HEX);
    Serial.println(")");

    Wire.beginTransmission(slave.address);
    Wire.write(1);  // LED an
    Wire.endTransmission();
    delay(500);

    Wire.beginTransmission(slave.address);
    Wire.write(0);  // LED aus
    Wire.endTransmission();
  }
  delay(1000);
}

// Scan nach neuen Slaves
void scanForSlaves() {
  Serial.println("Scanne nach Slaves...");

  // Scanne alle I²C-Adressen
  for (byte address = 0x08; address <= 0x77; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {  // Gerät gefunden
      bool known = false;
      for (const auto& slave : slaves) {
        if (slave.address == address) {
          known = true;
          break;
        }
      }

      if (!known) {
        // Neuer Slave gefunden – frage Modul-Name ab
        Wire.requestFrom(address, (byte)16);  // Max. 16 Bytes für Modul-Name
        String moduleName = "";
        while (Wire.available()) {
          char c = Wire.read();
          if (c == '\0') break;  // Nullterminator
          moduleName += c;
        }

        if (moduleName.length() > 0) {
          SlaveInfo newSlave = {address, moduleName};
          slaves.push_back(newSlave);
          Serial.print("Neuer Slave gefunden: ");
          Serial.print(moduleName);
          Serial.print(" (0x");
          Serial.print(address, HEX);
          Serial.println(")");
        }
      }
    }
  }

  // Liste aller bekannten Slaves
  Serial.println("Bekannte Slaves:");
  for (const auto& slave : slaves) {
    Serial.print("- 0x");
    Serial.print(slave.address, HEX);
    Serial.print(": ");
    Serial.println(slave.moduleName);
  }
}
