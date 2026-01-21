#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();  // Standard-I²C-Pins (GPIO6=SDA, GPIO7=SCL)
  Serial.println("ESP32 Master - Zwei Slaves mit unterschiedlichen Takten");
}

void loop() {
  // Slave 0x10 (5-Sekunden-Takt)
  static unsigned long lastTime = 0;
  if (millis() - lastTime >= 5000) {
    lastTime = millis();
    byte command = 1;
    Wire.beginTransmission(0x10);
    Wire.write(command);
    Wire.endTransmission();
    Serial.println("0x10: Sent dispensing command");
    delay(500);
  }
}
