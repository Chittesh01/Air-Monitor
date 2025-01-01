#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "driver/spi_slave.h"
#include "freertos/FreeRTOS.h"
#include <Arduino.h>
#include <BLE2902.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <stdio.h>
#include <string>

#define MOSI_PIN 23 // Master Out, Slave In
#define MISO_PIN 19 // Master In, Slave Out
#define SCLK_PIN 18 // Serial Clock
#define CS_PIN 5    // Chip Select

volatile bool dataReceived = false, deviceConnected = false;
uint8_t sendbuf = 0;
float recvbuf[3] = {0};
char values[20] = {0};

BLECharacteristic *pCharacteristic;
BLEAdvertising *pAdvertising;
String string;

spi_slave_transaction_t t;
spi_device_handle_t handle;

#define SERVICE_UUID "12345678-1234-1234-1234-123456789012"
#define CHARACTERISTIC_UUID "87654321-4321-4321-4321-210987654321"

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) { deviceConnected = true; };

  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
    pAdvertising->start();
    gpio_set_level(GPIO_NUM_2, LOW);
  }
};

void setup() {
  gpio_config_t io_conf = {
      .pin_bit_mask = (1U << GPIO_NUM_2),
      .mode = GPIO_MODE_INPUT_OUTPUT,
  };
  gpio_config(&io_conf);

  // Common baud rate, using a faster rate could introduce more errors
  Serial.begin(115200);

  spi_bus_config_t spi_slave_config = {
      .mosi_io_num = MOSI,
      .sclk_io_num = SCK,
  };

  //  No need to setup device as slave synchronizes according to SCLK
  // spi_device_interface_config_t spi_stm32_confg = {
  //     .mode = 0,
  //     .duty_cycle_pos = 128,
  //     .clock_speed_hz = // prescaler of ESP32 can be set from 2 to 65536,
  //         2250000,      // allowing for flexible clock frequencies
  //     .queue_size = 1};

  spi_slave_interface_config_t spi_slave_confg = {
      .spics_io_num = SS, .queue_size = 1, .mode = 0};

  spi_slave_initialize(SPI2_HOST, &spi_slave_config, &spi_slave_confg,
                       SPI_DMA_DISABLED);
  // spi_bus_add_device(SPI2_HOST, &spi_stm32_confg, &handle);

  memset(&t, 0, sizeof(t));
  t.length = sizeof(recvbuf) * 8;
  t.tx_buffer = &sendbuf;
  t.rx_buffer = recvbuf;

  BLEDevice::init("ESP32_BLE");

  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
      CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ |       // Send a notification and allow
          BLECharacteristic::PROPERTY_NOTIFY); // receiver to read

  pCharacteristic->addDescriptor(new BLE2902());

  pService->start();
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->start();
}

void loop() {
  // Logic to periodically send data

  spi_slave_transmit(SPI2_HOST, &t,  // Blocks until data is received through
                     portMAX_DELAY); // SPI, never times out
  // sprintf(values, "%.2f-%.2f-%.2f", recvbuf[0], recvbuf[1], recvbuf[2]);
  // Serial.println(values);

  if (deviceConnected) {
    pCharacteristic->setValue((uint8_t *)recvbuf, sizeof(recvbuf));
    pCharacteristic->notify();

    gpio_set_level(GPIO_NUM_2, !gpio_get_level(GPIO_NUM_2));
  }
}