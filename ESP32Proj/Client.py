import socketio
import msvcrt

# Create a SocketIO client instance
sio = socketio.Client()
values = []

# Event handler for updates
@sio.on("update")
def on_update(data):
    values = data["values"]
    print(f"Temperature: {values[0]:.1f} °C")
    print(f"Temperature: {values[1]:.1f} °F")
    print(f"Humidity: {values[2]:.1f}%\n")

# Event handler when the client disconnects from the server
@sio.on("disconnect")
def on_disconnect():
    print("Disconnected from the server.")

# Connect to the SocketIO server
server_url = "http://localhost:12000"
try:
    sio.connect(server_url)
    print("Connected to the server!\n")
except Exception as e:
    print("Failed to connect to the server:", e)
    exit(1)

while sio.connected:
    if msvcrt.kbhit():
        if (msvcrt.getch() == b'\r'):
            sio.disconnect()