//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Victor Tchistiak - 2019
//
//This example demostrates master mode bluetooth connection and pin 
//it creates a bridge between Serial and Classical Bluetooth (SPP)
//this is an extention of the SerialToSerialBT example by Evandro Copercini - 2018
//

#include "BluetoothSerial.h"

BluetoothSerial SerialBT;

// String MACadd = "54:0A:11:60:62:EC";
// uint8_t address[6]  = {0x54, 0x0A, 0x11, 0x60, 0x62, 0xEC};
// uint8_t address[6]  = {0x00, 0x1D, 0xA5, 0x02, 0xC3, 0x22};
String name = "AT_slave";
char *pin = "1234"; //<-  standard pin would be provided by default
bool connected;
byte receivedBytes[4];
float value_BT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("AT_master", true); 
  Serial.println("The device started in master mode, make sure remote BT device is on!");
  
  connected = SerialBT.connect(name);
  
  if(connected) {
    Serial.println("Connected Succesfully!");
  } 
  else {
    while(!SerialBT.connected(10000)) {
      Serial.println("Failed to connect. Make sure remote device is available and in range, then restart app."); 
    }
  }
  // // disconnect() may take upto 10 secs max
  // if (SerialBT.disconnect()) {
  //   Serial.println("Disconnected Succesfully!");
  // }
  // this would reconnect to the name(will use address, if resolved) or address used with connect(name/address).
  // SerialBT.connect();
}

void loop() {
  if (SerialBT.available() >= 4){
    value_BT = readBT_value();
    Serial.println(value_BT);
  }
}


float readBT_value(){
  for (int i = 0; i < 4; i++) {
    receivedBytes[i] = SerialBT.read();
  }
  
  float receivedFloat;
  memcpy(&receivedFloat, &receivedBytes, sizeof(receivedFloat));

  return receivedFloat;
}

