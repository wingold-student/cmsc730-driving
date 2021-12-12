import time
import csv
import serial


def readArduino():
    arduino = serial.Serial(port='/dev/cu.usbserial-14440', baudrate=115200, timeout=0.5)
    while True:
        line = arduino.readline().strip()

        if line:
            print(line)

def readCsv(filepath):
    with open(filepath, newline='') as csvfile:
        reader = csv.DictReader(csvfile, fieldnames=["speed"])

        for row in reader:
            print(row["speed"])

readCsv("test.csv")