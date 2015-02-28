#!/usr/bin/env python2

# Using the PySerial package, this will read in a text file that will play an animaiton on the
# MEGA_MATRIX hardware device
#
# Usage:
#   python3 mm.py <serial_port> <folder>/animation.txt
#
# Example
#   python3 mm.py /dev/ttyACM0 banner/animation.txt
#
# An "image," is a NxM text file that contains only 1's and 0's.  A 1 means to turn the LED on,
# and 0 means to turn it off.  Behaviour is undefined if you do anything else. 
#
# The language is as follows
#
# <R>x<C>: (or NxM)  This is how many rows by how many colums of LEDS are there.  <R> and <C>
#          should be postive integers that are multiples of 24.  This is always on the first
#          line of the file format.  E.g. (24x24, 24x48, 240x480).
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


# Globals
ser = None


class Frame:
    # Basic class for a Frame to be displayed

    __slots__ = (
        'fid',          # File ID (i.e. the name
        'data',         # Strings of (text) ones and zeros, can also be some newlines, just a dump of a file
        'delay',        # Delay value in milliseconds
        'nextFrame',    # Pointer to next frame
    )

    def __init__(self, fid, data, delay):
        self.fid = fid
        self.data = data
        self.delay = delay
        self.nextFrame = None


def startup():
    global ser

    ser = serial.Serial(sys.argv[1], 115200, timeout=1)
    time.sleep(2)
    ser.flush()


def shutdown(signal=None, frame=None):
    # Use this to shutdown the serial applicaiton
    global ser

    time.sleep(1)
    ser.flush()
    ser.close()
    sys.exit(0)


def main():
    # Main routine for program
    print('Running Animation...')
    print('Press ^C to stop at any time.')

    # We assume everyhing is A-okay here!
    port = sys.argv[1]
    seqFilename = sys.argv[2]
    seqDir = os.path.abspath(os.path.dirname(seqFilename))


    # Read in the image file
    seqFile = open(seqFilename, 'r')

	# get the dimensions
    dim = seqFile.readline().split('x')
    dimRows= int(dim[0])
    dimCols = int(dim[1])
    # TODO, swap the order from RowxColumn, to WidthxHeight (in all of the project)

    # Get the image files
    imageFiles = seqFile.readline()[7:]		# TODO bad constant splitting, figure out something better
    imageFiles = imageFiles.split(',')
    for i in range(len(imageFiles)):
        imageFiles[i] = imageFiles[i].strip(' \n\r')
        imageFiles[i] = os.path.join(seqDir, imageFiles[i])


    # Read in each of the files, then get their data, and put it into a dict
    frameData = {}
    for imgFile in imageFiles:
        # Open a file and create a buffer
        f = open(imgFile, 'r')
        data = ''
        
        # Convert that data to a 72 byte thingy
		# TODO need to make this here work for other sizes, have to adjust file format
        for line in f:
            line = line.strip(' \n\r')

            for i in range(0, dimCols, 8):
                data += '%c'%int(line[i:(i + 8)], 2)
            
#            b1 = line[:8]
#            b2 = line[8:16]
#            b3 = line[16:24]
#            b4 = line[24:32]
#            b5 = line[32:40]
#            b6 = line[40:]
#            data += '%c'%int(b1, 2)
#            data += '%c'%int(b2, 2)
#            data += '%c'%int(b3, 2)
#            data += '%c'%int(b4, 2)
#            data += '%c'%int(b5, 2)
#            data += '%c'%int(b6, 2)

        # Cleanup
        frameData[os.path.basename(imgFile)] = data
        f.close()


    # Check for start of loop
    if seqFile.readline().strip(' \n\r') != 'start':
        print('Error, no start given')
        sys.exit(1)


    # Make the first one
    line = seqFile.readline().strip(' \n\r')
    params = line.split(':')
    imgFile = params[0]         # String ID
    delay = float(params[1])    # delay value (integer)

    cur = Frame(imgFile, frameData[imgFile], delay / 1000.0)
    head = cur


    # And now create the animation strucutre
    line = seqFile.readline().strip(' \n\r')
    while line != 'loop':
        # Assume that it's of the format `[file]:[delay]`
        params = line.split(':')
        imgFile = params[0]         # String ID
        delay = float(params[1])    # delay value (integer)

        newFrame = Frame(imgFile, frameData[imgFile], delay / 1000.0)
        cur.nextFrame = newFrame
        cur = newFrame

        # Keep looing through until we rea
        line = seqFile.readline().strip(' \n\r')


    # All done, point the tail to the head for a nice loop
    cur.nextFrame = head

    # Cleanup
    seqFile.close()


    # Now loop through the animation
    cur = head
    while True:
        print('Displaying: %s'%cur.fid)
        ser.write(cur.data)
        time.sleep(cur.delay)
        cur = cur.nextFrame

     

# Main program
if len(sys.argv) < 3:
    print('Please provide a serial device and a sequence file')
    sys.exit(0)

signal.signal(signal.SIGINT, shutdown)
startup()
main()
shutdown()




