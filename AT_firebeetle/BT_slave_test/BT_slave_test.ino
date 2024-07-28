//This example code is in the Public Domain (or CC0 licensed, at your option.)
//By Evandro Copercini - 2018
//
//This example creates a bridge between Serial and Classical Bluetooth (SPP)
//and also demonstrate that SerialBT have the same functionalities of a normal Serial

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("AT_slave"); //Bluetooth device name
  // SerialBT.
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  float value = 12345.12;
  sendData_ble(value);
  delay(1000);
}

void sendData_ble(float data){
  SerialBT.write((byte*)&data, sizeof(float));
}