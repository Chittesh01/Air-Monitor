#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "freertos/FreeRTOS.h"
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <SPI.h>
#include <stdio.h>
#include <string>


#define MOSI_PIN 23 // Master Out, Slave In
#define MISO_PIN 19 // Master In, Slave Out
#define SCLK_PIN 18 // Serial Clock
#define CS_PIN 5    // Chip Select

volatile bool dataReceived = false, deviceConnected = false;
byte receivedData[12]; // Buffer to store received data

BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising;
String string;

#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    pAdvertising->start();
  }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  SPI.begin(SCK, MISO, MOSI, SS); // Initialize SPI with SS
  // attachInterrupt(digitalPinToInterrupt(CS_PIN),
  //                 FALLING); // CS low means data is ready
  SPI.beginTransaction(SPISettings(281250, MSBFIRST, SPI_MODE0));

  Serial.begin(115200);

  BLEDevice::init("ESP32_BLE");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
}

void loop() {
  // Logic to periodically send data

  // digitalWrite(LED_BUILTIN, HIGH);
  // float value1 = 27.2, value2 = 28.2;
  //  pCharacteristic->setValue((uint8_t *)data, sizeof(data));
  //  pCharacteristic->notify();
  //  digitalWrite(LED_BUILTIN, LOW);

  if (digitalRead(CS_PIN) == LOW) {
    for (int i = 0; i < 11; i++) {
      receivedData[i] = SPI.transfer(0x00); // Dummy transfer to receive data
      Serial.print(receivedData[i], HEX);
      //    memcpy(&string[i], &receivedData[i], sizeof(char));
    }
  }

  // dataReceived = false;
}