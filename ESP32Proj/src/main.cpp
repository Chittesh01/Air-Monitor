#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <string>

BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising;
bool deviceConnected = false;

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
  if (deviceConnected) {
    float value1 = 27.2, value2 = 28.2;
    float data[2] = {value1, value2};
    pCharacteristic->setValue((uint8_t *)data, sizeof(data));
    pCharacteristic->notify();
    delay(1000);
  }
}