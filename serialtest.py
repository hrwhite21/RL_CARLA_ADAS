import serial
import numpy
done = False
arduino = serial.Serial("COM7",115200,timeout=10)

while not done:
    numbers = input('Input a set of numbers less than 600 in the form [xyz,xyz,xyz]')
    data_string = ','.join(map(str, numbers)) + '\n'
    arduino.write(data_string.encode())
