import serial
import time

# Configure the serial port
serial_port = 'COM7'  # Change this to the correct port for your system
# 7 for the real arduino, 8 for the elegoo clone
baud_rate = 115200

# Connect to the Arduino
ser = serial.Serial(serial_port, baud_rate, timeout=0.1)

# Function to send command to Arduino
def send_command(command):
    ser.write((command + '\n').encode())

# Main loop
try:
    while True:
        # Send a command to the Arduino
        command = input("Enter command (e.g., '100,200,300'): ")
        send_command(command)
        
        ser.reset_input_buffer()
        time.sleep(1)  # Wait for Arduino to process command
                        # Needs to be longer than a second if
                        # DOing full wheel roattion at 750 microseconds
        print(ser.readline().decode())
        

except KeyboardInterrupt:
    print("Exiting...")
    ser.close()
