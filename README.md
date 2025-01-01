# Air-Monitor-Project

This is a project which measures certain characteristics present in the air.
Current Measurements: Temperature(°C and °F) and Humidity(%)

The MCU board used is an STM32 NUCLEO-F103RB. This board takes the readings from a DHT22 sensor, and sends
it over through SPI interface to another NodeMCU-32S board. The NodeMCU board then sends the data to a 
Python bleak client using Bluetooth Low Energy (BLE).
