#!/usr/bin/env python2

# Using the PySerial package for python 2.7, this is a simple program to test serial connections
# And for wrting "images," to the MEGA_MATRIX

import sys, time, signal
import serial

if len(sys.argv) < 3:
    exit(1)

ser = serial.Serial(sys.argv[1], 115200, timeout=1)
ser.flush()


def shutdown(signal=None, frame=None):
    # Use this to shutdown the serial applicaiton
    global ser
    dir(ser)

    time.sleep(1)
    ser.flush()
    ser.close()
    sys.exit(0)


# connect up the signals
signal.signal(signal.SIGINT, shutdown)


# Open up a file and send it to the matrix
files = [None, None, None, None]
files[0] = open('csh_logo/1.txt', 'r')
files[1] = open('csh_logo/2.txt', 'r')
files[2] = open('csh_logo/3.txt', 'r')
files[3] = open('csh_logo/4.txt', 'r')
i = 0

time.sleep(2)

# Infinite loop:
while True:

    if i == 4:
        i = 0
   
    # Spit out the stuff for the file
    files[i].seek(0)
    for line in files[i]:
        line = line.strip('\r\n')
        c1 = line[:8]
        c2 = line[8:16]
        c3 = line[16:]
    
        ser.write('%c'%int(c1, 2))
        ser.write('%c'%int(c2, 2))
        ser.write('%c'%int(c3, 2))

    time.sleep(100.0 / 1000.0)
    i += 1


shutdown()
