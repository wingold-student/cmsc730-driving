import serial
import time

arduino = serial.Serial(port='/dev/cu.usbserial-14440', baudrate=115200, timeout=0.5)

def readArduino():
    data = arduino.readline()
    return data

while True:
    line = readArduino().strip()

    if line:
        print(line)
