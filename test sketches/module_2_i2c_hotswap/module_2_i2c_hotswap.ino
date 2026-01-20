#include <Wire.h>
#include <EEPROM.h>

// module info
const char* MODULE_TYPE = "clip";
byte START_ADDRESS = 0x11;  // start address
byte MY_ADDRESS;  // new permanent address
uint32_t uniqueID;  // 4-Byte UID

// stock
unsigned int STOCK_ADDRESS = 0;
int stock = 0;  // module-specific data

void setup() {
  // initialize serial interface
  Serial.begin(115200);
  Serial.println("Initialzed serial comunication");
  
  // initialize EEPROM
  if (!EEPROM.length()) {
    Serial.println("EEPROM initialization failed!");
  } else {
    EEPROM.get(0, stock);  // read stock from EEPROM
    Serial.println("Loaded stock from EEPROM");
    Serial.print("Stock: ");
    Serial.println(stock);
  }

  // initialize uniqueID
  generateUniqueID();

  // initialize I²C communication
  Serial.println("Initialize I²C communication");
  Serial.print("Starting on default address: 0x");
  Serial.println(START_ADDRESS, HEX);
  Wire.begin(START_ADDRESS);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendModuleInfo);
}

void loop() {
  // empty (everything handled in interrupt)
}

void generateUniqueID() {
  randomSeed(analogRead(0));
  //uniqueID = random(0xFFFFFFFF);
  uniqueID = random(2147483647);  // highest 32-bit decimal number
  Serial.print("Generated new uniqueID: ");
  Serial.println(uniqueID, HEX);
}

// send module info (on request)
void sendModuleInfo() {
  Serial.println("Module info requested");
  Wire.write(MODULE_TYPE, strlen(MODULE_TYPE) + 1);
  Serial.print("Announced module type: ");
  Serial.println(MODULE_TYPE);
  // send uniqueID as packages
  Wire.write((uniqueID >> 24) & 0xFF);
  Wire.write((uniqueID >> 16) & 0xFF);
  Wire.write((uniqueID >> 8) & 0xFF);
  Wire.write(uniqueID & 0xFF);
  Serial.print("Announced module UID: ");
  Serial.println(uniqueID, HEX);
}

void receiveEvent(int bytes) {
  if (bytes >= 1) {
    byte cmd = Wire.read();
    if (cmd == 'A') {  // address assignment cmd
      byte newAddress = Wire.read();
      uint32_t targetID = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8)  | Wire.read();
      if (targetID == uniqueID) {  // check if this module is requested
        MY_ADDRESS = newAddress;
        Wire.begin(MY_ADDRESS);  // restart I²C on new address 
        Serial.print("New address received: 0x");
        Serial.println(MY_ADDRESS, HEX);
      }
    } else if (cmd == 'C') {  // collision detected cmd
      uint32_t targetID = (Wire.read() << 24) | (Wire.read() << 16) | (Wire.read() << 8)  | Wire.read();
      if (targetID == uniqueID) {
        generateUniqueID();
        // on next scan new UID will be sent
      }
    } else if (cmd == 1 || cmd == 0) {
      digitalWrite(LED_BUILTIN, cmd);
    }
  }
}