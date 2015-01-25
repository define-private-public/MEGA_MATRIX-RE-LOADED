#!/usr/bin/env python3
#
# Small little application that will Take in a Black and White PNG image, a
# amount of seconds to display the sequence, and a destianion direction.
# Image must be exactly 24 pixels high.  For a pixel to be on, paint white for
# off, paint black.  Must also be minimu 24 pixels wide
#
# If any of the values are bad, no image will get saved
#
# Usage:
#   python3 png2mm.py <png_file> <seconds> <destination>
#
# Example:
#   python3 png2mm.py banner.png 4.5 banner_message

import sys, os
from PySide.QtGui import *
from PySide.QtCore import *


def main():
	# Main function
	app = QApplication(sys.argv)

	# Get some values
	argc = len(sys.argv)
	if argc < 4:
		# Exit
		print('Need at least three arguements.')
		print('Usage:')
		print('  python3 mm2png.py <png_file> <seconds> <destination>')
		sys.exit(0)
	
	# Peel some data out
	pngFilename = sys.argv[1]
	seconds = float(sys.argv[2])
	dest = sys.argv[3]

	# Checks
	if not pngFilename[-4:].lower().endswith('.png'):
		print('Image needs to be a PNG file.')
		sys.exit(0)

	if seconds < 0:
		print('Seconds needs to be a positive real number.')
		sys.exit(0)
	
	# Read in the Image data
	png = QImage(pngFilename)
	size = 24
	width = png.width()

	# Size check
	if width < 24:
		print('PNG width too small')
		sys.exit(1)

	# Make directory
	os.mkdir(dest)

	# Read the stuff into text files
	col = 1
	blackPixel = qRgb(0x00, 0x00, 0x00)
	while ((col -  1) + size) <= width:
		textFile = open('%s/%i.txt'%(dest, col), 'w')

		# read pixels
		for r in range(0, size):
			for c in range(col - 1, (col - 1) +size):
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
	animFile.write('frames: 1.txt')
	for i in range(2, col + 1):
		animFile.write(', %i.txt'%i)
	animFile.write('\n')
	
	# Write the sequence
	ms = int(seconds * 1000) // col
	animFile.write('start\n')
	for i in range(1, col + 1):
			animFile.write('%i.txt:%i\n'%(i, ms))
	animFile.write('loop')
	
	# Finish up
	animFile.close()


main()

