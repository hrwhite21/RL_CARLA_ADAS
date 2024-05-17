import serial
import time

# Configure the serial port
serial_port = 'COM7'  # Change this to the correct port for your system
baud_rate = 19200

# Connect to the Arduino
ser = serial.Serial(serial_port, baud_rate, timeout=1)

# Function to send command to Arduino
def send_command(command):
    ser.write((command + '\n').encode())

# Main loop
try:
    while True:
        # Send a command to the Arduino
        command = input("Enter command (e.g., '100,200,300'): ")
        send_command(command)

        time.sleep(1)  # Wait for Arduino to process command

except KeyboardInterrupt:
    print("Exiting...")
    ser.close()
