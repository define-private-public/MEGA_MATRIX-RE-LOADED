#!/usr/bin/env python2

# Using the PySerial package for python 2.7, this is a simple program to test serial connections
# And for wrting "images," to the MEGA_MATRIX

import sys, time, signal
import serial

if len(sys.argv) < 3:
    exit(1)

ser = serial.Serial(sys.argv[1], 115200, timeout=1)


def shutdown(signal=None, frame=None):
    # Use this to shutdown the serial applicaiton
    global ser
    dir(ser)

    time.sleep(1)
    ser.close()
    sys.exit(0)


# connect up the signals
signal.signal(signal.SIGINT, shutdown)


# Open up a file and send it to the matrix
f = open(sys.argv[2], 'r')
b = 0

time.sleep(2)

for line in f:
    line = line.strip('\r\n')
    c1 = line[:8]
    c2 = line[8:16]
    c3 = line[16:]

#    print('%c'%int(c1, 2))
#    print('%c'%int(c2, 2))
#    print('%c'%int(c3, 2))

    ser.write('%c'%int(c1, 2))
    ser.write('%c'%int(c2, 2))
    ser.write('%c'%int(c3, 2))
#    ser.write('%c'%0b10100011)
#    ser.write('%c'%0b10100011)
#    ser.write('%c'%0b10100011)
#    ser.write('a' * 3)


shutdown()
