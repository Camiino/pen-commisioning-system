#include <Wire.h>

const byte MY_ADDRESS = 0x10;  // I²C-Adresse dieses Slaves
const char* MODULE_NAME = "Shaft";  // Modul-Name (z. B. "Shaft", "Mine", etc.)

void setup() {
  Serial.begin(9600);
  Wire.begin(MY_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendModuleName);  // Callback für Master-Anfragen

  // Meldung an den Master senden (nach dem Start)
  delay(1000);  // Warte, bis I²C stabil ist
  announceToMaster();
}

void loop() {
  // Leer (alles im Interrupt)
}

// Sende Modul-Name an den Master (wenn angefragt)
void sendModuleName() {
  Wire.write(MODULE_NAME, strlen(MODULE_NAME) + 1);  // String inkl. Nullterminator
}

// Meldung an den Master (bei Start)
void announceToMaster() {
  Wire.beginTransmission(0x01);  // Master-Adresse (ESP32)
  Wire.write('I');  // 'I' = Identifikation
  Wire.write(MY_ADDRESS);  // Eigene I²C-Adresse
  Wire.write(MODULE_NAME, strlen(MODULE_NAME) + 1);  // Modul-Name
  Wire.endTransmission();
}

// Empfange Befehle vom Master
void receiveEvent(int bytes) {
  if (bytes >= 1) {
    byte cmd = Wire.read();
    if (cmd == 1) {
      digitalWrite(LED_BUILTIN, HIGH);
    } else if (cmd == 0) {
      digitalWrite(LED_BUILTIN, LOW);
    }
  }
}
