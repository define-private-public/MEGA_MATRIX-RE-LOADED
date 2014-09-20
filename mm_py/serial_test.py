#!/usr/bin/env python2

# Using the PySerial package for python 2.7, this is a simple program to test serial connections
# And for wrting "images," to the MEGA_MATRIX

import sys, time
import serial

if len(sys.argv) < 2:
    exit(1)

ser = serial.Serial(sys.argv[1], 115200, timeout=1)

# current HIGH bit
row = 0
col = 0

# Loop forever
while True:

    r = 0
    while r < 24:

        c = 0
        while c < 3:

#            if (r == row) and ((col // 8) == c):
#                byte = 0b10000000 >> (col % 8)
#                ser.write('%c'%byte)
            if (r == 0) and (c == 0):
                ser.write('%c'%0b10100000)
            else:
                ser.write('\x00')

            print('r:%i, c:%i'%(r, c))
            c += 1

        r += 1


    # Move it back down
    col += 1

    if col == 24:
        col = 0
        row += 1

        if row == 24:
            row = 0
            print('Cycle Done!')
