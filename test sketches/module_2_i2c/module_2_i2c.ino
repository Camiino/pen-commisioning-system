// receiver
#include <Wire.h>

byte mydata;

void receiver(int n_bytes) {
  while(Wire.available()) {
    mydata = Wire.read();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("\nI2C PINS"));
  Serial.print(F("\tSDA = ")); Serial.println(SDA);
  Serial.print(F("\tSCL = ")); Serial.println(SCL);

  pinMode(LED_BUILTIN, OUTPUT);
  Wire.begin(0x11);
  Wire.onReceive(receiver);
}

void loop() {
  if(mydata==0) {
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println("LED off");
  }
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("LED on");
  }

}