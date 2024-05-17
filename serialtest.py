import serial
import numpy
import numpy as np
import random
done = False
arduino = serial.Serial("COM7",19200,timeout=10)

while not done:
    numbers = np.random.randint(0,400,3)
    print(numbers)
    data_string = ','.join(map(str, numbers)) + '\n'
    print(data_string)
    arduino.write(data_string.encode())
    print(arduino.readline())