from bleak import BleakClient
import struct
import msvcrt

# Define the ESP32 address and characteristic UUID
address = "CC:DB:A7:9B:5D:76"  # MAC address of ESP32_BLE
characteristic_uuid = "87654321-4321-4321-4321-210987654321"

async def on_data_received(sender: int, value: bytearray):
    values = struct.unpack('fff', value)  # 'fff' represents 3 floats
    print(f"Temperature: {values[0]:.1f} °C")
    print(f"Temperature: {values[1]:.1f} °F")
    print(f"Humidity: {values[2]:.1f}%\n")

async def main():
    async with BleakClient(address) as client:
        # Subscribe to notifications for the characteristic
        await client.start_notify(characteristic_uuid, on_data_received)

        while(1):
            # Read the characteristic value
            await client.read_gatt_char(characteristic_uuid)

            # Check if key is pressed
            if msvcrt.kbhit():
            # Check if the key is Enter
                if (msvcrt.getch() == b'\r'):
                    print("Exiting loop.")
                    break
            
        
        # Stop notifications after done
        await client.stop_notify(characteristic_uuid)

import asyncio
asyncio.run(main())