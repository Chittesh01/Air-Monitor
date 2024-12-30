#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <SPI.h>
#include <string>

#define MOSI_PIN 23 // Master Out, Slave In
#define SCLK_PIN 18 // Serial Clock
#define CS_PIN 5    // Chip Select

volatile bool dataReceived = false, deviceConnected = false;
byte receivedData[12]; // Buffer to store received data

BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising;

#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

void IRAM_ATTR onSPIReceive() {
  // Read data from SPI
  for (int i = 0; i < sizeof(receivedData); i++) {
    receivedData[i] = SPI.transfer(0x00); // Dummy transfer to receive data
  }
  dataReceived = true;
}

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    pAdvertising->start();
  }
};

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  SPI.begin(SCLK_PIN, -1, MOSI_PIN, CS_PIN); // Initialize SPI with CS
  attachInterrupt(digitalPinToInterrupt(CS_PIN), onSPIReceive,
                  FALLING); // CS low means data is ready

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
  if (dataReceived) {
    // digitalWrite(LED_BUILTIN, HIGH);
    // float value1 = 27.2, value2 = 28.2;
    Serial.println((char *)receivedData);
    // pCharacteristic->setValue((uint8_t *)data, sizeof(data));
    // pCharacteristic->notify();
    // digitalWrite(LED_BUILTIN, LOW);
    dataReceived = false;
  }
}