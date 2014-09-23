#!/usr/bin/env python2

# Using the PySerial package, this will read in a text file that will play an animaiton on the
# MEGA_MATRIX hardware device

# An "image," is a 24x24 text file that contains only 1's and 0's.  A 1 means to turn the LED on,
# and 0 means to turn it off.  Behaviour is undefined if you do anything else. 

# The language is as follows
#
# frames: [0], [1], [2], ... [n]
#  -- The stuff here in the brackets should be filenames for valid MEGA_MATRIX image files
#  -- This should be comma separated values all on one line
#
# start
#  -- Start of the animation sequence
#
# [filename]:[delay_ms]
#  -- [filename] should be one of the frames defined in the begining
#  -- [delay_ms] an integer delay value that should be in milliseconds
#  -- You can line up multiples of these calls to create your animation
#
# loop
#  -- Signifies the end of the animation sequence, and will loop all of the images


import sys, os, signal, time
import serial


class Frame:
    # Basic class for a Frame to be displayed

    __slots__ = (
        'data',         # Strings of (text) ones and zeros, can also be some newlines, just a dump of a file
        'delay',        # Delay value in milliseconds
        'nextFrame',    # Pointer to next frame
    )

    def __init__(self, data, delay):
        self.data = data
        self.delay = delay
        self.nextFrame = None




