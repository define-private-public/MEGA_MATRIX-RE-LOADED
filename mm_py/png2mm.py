#!/usr/bin/env python3
#
# Small little application that will Take in a Black and White PNG image, a
# amount of seconds to display the sequence, and a destianion direction.
# Image must be NxM pixels, N and M are specified in the command line arguments
# as <RxC>. It's recommended to keep them as positive multiples of 24.
#
# If any of the values are bad, no image will get saved
#
# Usage:
#   python3 png2mm.py <RxC> <png_file> <seconds> <destination>
#
# Example:
#   python3 png2mm.py 24x48 banner.png 4.5 banner_message

import sys, os
from PySide.QtGui import *
from PySide.QtCore import *


def main():
	# Main function
	app = QApplication(sys.argv)

	# Get some values
	argc = len(sys.argv)
	if argc < 5:
		# Exit
		print('Need at least three arguements.')
		print('Usage:')
		print('  python3 mm2png.py <RxC> <png_file> <seconds> <destination>')
		sys.exit(0)
	
	# Peel some data out
	dim = sys.argv[1].split('x')
	pngFilename = sys.argv[2]
	seconds = float(sys.argv[3])
	dest = sys.argv[4]
	dimRows = 0
	dimCols = 0



	# Checks
	if len(dim) != 2:
		print('Need an NxM value, that is positive integers')

	if not pngFilename[-4:].lower().endswith('.png'):
		print('Image needs to be a PNG file.')
		sys.exit(0)

	if seconds < 0:
		print('Seconds needs to be a positive real number.')
		sys.exit(0)
	
	# Get the dimensions
	dimRows = int(dim[0])
	dimCols = int(dim[1])
	
	# Read in the Image data
	png = QImage(pngFilename)
	width = png.width()

	# Size check
	if width < dimCols:
		print('PNG width too small')
		sys.exit(1)

	# Make directory
	os.mkdir(dest)

	# Read the stuff into text files
	col = 1
	blackPixel = qRgb(0x00, 0x00, 0x00)
	while ((col -  1) + dimCols) <= width:
		textFile = open('%s/%i.txt'%(dest, col), 'w')

		# read pixels
		for r in range(0, dimRows):
			for c in range(col - 1, (col - 1) + dimCols):
				# inspect pixel
				pixel = png.pixel(c, r)
				if pixel == blackPixel:
					textFile.write('0')
				else:
					textFile.write('1')

			# Write a newline
			textFile.write('\n')
		
		# Close the file and increment			
		textFile.close()
		col += 1
	
	# Write the animation file
	animFile = open('%s/animation.txt'%dest, 'w')
	animFile.write('%ix%i\n'%(dimRows, dimCols))
	animFile.write('frames: 1.txt')
	for i in range(2, col):
		animFile.write(', %i.txt'%i)
	animFile.write('\n')
	
	# Write the sequence
	ms = int(seconds * 1000) // col
	animFile.write('start\n')
	for i in range(1, col):
			animFile.write('%i.txt:%i\n'%(i, ms))
	animFile.write('loop')
	
	# Finish up
	animFile.close()


main()

