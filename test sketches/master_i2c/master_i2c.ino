#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();  // Standard-I²C-Pins (GPIO6=SDA, GPIO7=SCL)
  Serial.println("ESP32 Master - Zwei Slaves mit unterschiedlichen Takten");
}

void loop() {
  // Slave 0x10 (1-Sekunden-Takt)
  static bool state10 = false;
  state10 = !state10;
  Wire.beginTransmission(0x10);
  Wire.write(state10 ? 1 : 0);
  Wire.endTransmission();
  Serial.print("0x10: ");
  Serial.println(state10 ? "LED an" : "LED aus");

  // Slave 0x11 (2-Sekunden-Takt)
  static unsigned long lastTime11 = 0;
  if (millis() - lastTime11 >= 2000) {
    lastTime11 = millis();
    static bool state11 = false;
    state11 = !state11;
    Wire.beginTransmission(0x11);
    Wire.write(state11 ? 1 : 0);
    Wire.endTransmission();
    Serial.print("0x11: ");
    Serial.println(state11 ? "LED an" : "LED aus");
  }

  delay(1000);  // 1-Sekunden-Takt für Slave 0x10
}
