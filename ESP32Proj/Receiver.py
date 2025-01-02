from bleak import BleakClient
from flask import Flask, render_template
from flask_socketio import SocketIO
import threading
import asyncio
import struct
import msvcrt

app = Flask(__name__)
socketio = SocketIO(app, async_mode='threading', cors_allowed_origins=["http://localhost:12000"])
values = []


@app.route('/')
def index():
    return render_template("index.html")

# Define the ESP32 address and characteristic UUID
address = "CC:DB:A7:9B:5D:76"  # MAC address of ESP32_BLE
characteristic_uuid = "87654321-4321-4321-4321-210987654321"

async def on_data_received(sender: int, value: bytearray):
    values = struct.unpack('fff', value)  # 'fff' represents 3 floats
    print(f"Temperature: {values[0]:.1f} °C")
    print(f"Temperature: {values[1]:.1f} °F")
    print(f"Humidity: {values[2]:.1f}%\n")

    socketio.emit('update', {'values': values})

async def main():
    async with BleakClient(address) as client:
        # Subscribe to notifications for the characteristic
        await client.start_notify(characteristic_uuid, on_data_received)

        while True:
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

def run_socketio():
   socketio.run(app, host="127.0.0.1", port=12000)

if __name__ == "__main__":
    #thread = threading.Thread(target=run_flask, daemon=True).start();
    threading.Thread(target=run_socketio, daemon=True).start()
    asyncio.run(main())
