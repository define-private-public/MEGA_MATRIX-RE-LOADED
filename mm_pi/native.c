// Mega Matrix: A 24x24 LED Matrix
//
// Native edition
//
// Our matrices are Command cathode; a 3x3 array of 8x8 matrices.
// The row controllers will act as emitters, and the columns as
// receivers.  (Using 74HC595 shift registers)
//
// To denote patterns for ourselves (not the device) we'll have
//	 0 = LED Off
//	 1 = LED On
// 
// Though the machine will flip the bits for us to turn the correct
// LEDs on.
//
// Say we want to light up a row like this:	1 1 0 1
// We Have to do this:
//          Cols: |   1   2   3   4 
// Rows:          |
// ---------------+-----------------
// (n-1):     0   |   x   x   x   x
// (n):       1   |   0   0   1   0
// (n+1):     0   |   x   x   x   x
//
// x = Don't care
// 0 = LOW
// 1 = HIGH


// Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

// Raspberry Pi stuff
#define BCM2708_PERI_BASE 0x20000000
#define GPIO_BASE (BCM2708_PERI_BASE + 0x200000) /* GPIO Controler */
#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

// Give us bools
#define bool unsigned char
#define true 1
#define false 0

int mem_fd;
void *gpio_map;
volatile unsigned *gpio;

// GPIO setup macros, use INP_GPIO before using OUT_GPIO or SET_GPIO_ALT
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO(g) *(gpio + ((g) / 10)) |=	(1 << (((g) % 10) * 3))
#define SET_GPIO_ALT(g,a) *(gpio + (((g) / 10))) |= (((a) <= 3 ? (a) + 4 : (a) == 4 ? 3 : 2) << (((g) % 10) * 3))

#define GPIO_SET *(gpio + 7)	// Sets bits which are 1, ignore those are 0
#define GPIO_CLR *(gpio + 10)	// clears bits which are 1, ignore those are 0

#define GET_GPIO(g) (*(gpio + 13) & (1 << g))	// 0 if LOW, (1 << g) if HIGH

#define GPIO_PULL *(gpio + 37) 		// Pull up/pull down
#define GPIO_PULLCLK0 *(gpio + 38) 	// Pull up/pull down clock


// Row: Acts as the emmiter, all pins on port D of atmega
#define rData 5
#define rClock 6
#define rLatch 13
#define rEnable 19
#define rReset 26

// Colunm: Acts as the Reicver, all pins on port B of atmega
#define cData 17
#define cClock 27
#define cLatch 22
#define cEnable 23
#define cReset 26

int pins[10];

// The need for speed:
// For the Row controller
#define R_DATA_HIGH GPIO_SET = 1 << rData
#define R_DATA_LOW GPIO_CLR = 1 << rData
#define R_CLOCK_HIGH GPIO_SET = 1 << rClock
#define R_CLOCK_LOW GPIO_CLR = 1 << rClock
#define R_LATCH_HIGH GPIO_SET = 1 << rLatch
#define R_LATCH_LOW GPIO_CLR = 1 << rLatch 
#define R_ENABLE_HIGH GPIO_SET = 1 << rEnable
#define R_ENABLE_LOW GPIO_CLR = 1 << rEnable
#define R_RESET_HIGH GPIO_SET = 1 << rReset
#define R_RESET_LOW GPIO_CLR = 1 << rReset

// For the Column controller
#define C_DATA_HIGH GPIO_SET = 1 << cData
#define C_DATA_LOW GPIO_CLR = 1 << cData
#define C_CLOCK_HIGH GPIO_SET = 1 << cClock
#define C_CLOCK_LOW GPIO_CLR = 1 << cClock
#define C_LATCH_HIGH GPIO_SET = 1 << cLatch
#define C_LATCH_LOW GPIO_CLR = 1 << cLatch 
#define C_ENABLE_HIGH GPIO_SET = 1 << cEnable
#define C_ENABLE_LOW GPIO_CLR = 1 << cEnable
#define C_RESET_HIGH GPIO_SET = 1 << cReset
#define C_RESET_LOW GPIO_CLR = 1 << cReset


#define NUM_BYTES 72	// 24 * 3
#define NUM_ROWS 24
#define NUM_COLS 24
bool bufferReady = false;
unsigned char bytesRead = 0;
unsigned char image[NUM_BYTES];		 // Image that's currently being displayed
unsigned char image_buffer[NUM_BYTES];		// Buffer for next image


// Function prorotypes
void clsCols();
void clsRows();
void setup_gpio();
void setup();
void display();


//// Some patterns
//unsigned char CSH_logo[NUM_BYTES] = {
//	 B00000000, B00000000, B00000000,
//	 B00111111, B11111111, B10011110,
//	 B01111111, B11111111, B11011110,
//	 B01111111, B11111111, B11011110,
//	 B01111111, B11111111, B11011110,
//	 B01111000, B00000011, B11011110,
//	 B01111011, B11111011, B11011110,
//	 B01111011, B11111000, B00011110,
//	 B01111011, B11111011, B11011110,
//	 B01111011, B10111011, B11011110,
//	 B01111011, B10000011, B11111110,
//	 B01111011, B11111011, B11111110,
//	 B01111011, B11111011, B11111110,
//	 B01111000, B00111011, B11111110,
//	 B01111011, B10111011, B11011110,
//	 B01111011, B11111011, B11011110,
//	 B01111011, B11111000, B00011110,
//	 B01111011, B11111011, B11011110,
//	 B01111000, B00000011, B11011110,
//	 B01111111, B11111111, B11011110,
//	 B01111111, B11111111, B11011110,
//	 B01111111, B11111111, B11011110,
//	 B00111111, B11111111, B10011110,
//	 B00000000, B00000000, B00000000,
//};

unsigned char test_pattern[NUM_BYTES] = {
	'a', 'b', 'c',
	'b', 'c', 'd',
	'c', 'd', 'e',
	'd', 'e', 'f',
	'e', 'f', 'g',
	'f', 'g', 'h',
	'g', 'h', 'i',
	'h', 'i', 'j',
	'i', 'j', 'k',
	'j', 'k', 'l',
	'k', 'l', 'm',
	'l', 'm', 'n',
	'm', 'n', 'o',
	'n', 'o', 'p',
	'o', 'p', 'q',
	'p', 'q', 'r',
	'q', 'r', 's',
	'r', 's', 't',
	's', 't', 'u',
	't', 'u', 'v',
	'u', 'v', 'w',
	'v', 'w', 'x',
	'w', 'x', 'y',
	'x', 'y', 'z'
};


void clsCols() {
	// Clears out all of the columns (Turns them off by setting their values to 1)
	int c;
	C_DATA_HIGH;
	for (c = 0; c < NUM_COLS; c++) {
		C_CLOCK_HIGH;
		C_CLOCK_LOW;
	}
	C_DATA_LOW;
		
	C_LATCH_HIGH;
	C_LATCH_LOW;
}


void clsRows() {
	// Clear out the shift register
	R_RESET_LOW;
	R_RESET_HIGH;
}


// Setup memory regions to access GPIO
void setup_gpio() {
	// Init the pin list
	pins[0] = rData;
	pins[1] = rClock;
	pins[2] = rLatch;
	pins[3] = rEnable;
	pins[4] = rReset;
	pins[5] = cData;
	pins[6] = cClock;
	pins[7] = cLatch;
	pins[8] = cEnable;
	pins[9] = cReset;

	// open /dev/mem
	if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("Can't open /dev/mem\n");
		exit(-1);
	}

	// mmap GPIO
	gpio_map = mmap(
		NULL,						// Any address will do
		BLOCK_SIZE,					// Map Length
		PROT_READ | PROT_WRITE,		// Enable reading and writing to mapped mem
		MAP_SHARED,					// Shared with other processes
		mem_fd,						// File to map
		GPIO_BASE					// Offset to GPIO peripheral
	);

	close(mem_fd);		// Can destroy mem_fd now

	if (gpio_map == MAP_FAILED) {
		printf("mmap error 0x%X\n", (int)gpio_map);	// errno also set!
		exit(-1);
	}

	// Always use a volatile pointer!
	gpio = (volatile unsigned *)gpio_map;
}


void setup() {
	int i;
	int p;
	
	// Set everything to output
	for (p = 0; p < 10; p++) {
		INP_GPIO(pins[p]);
		OUT_GPIO(pins[p]);
		GPIO_CLR = 1 << pins[p];			// SET it LOW
	}
	
	// Turn on output, disable reset
	R_ENABLE_LOW;
	C_ENABLE_LOW;
	R_RESET_HIGH;
	C_RESET_HIGH;
	
	// XOR the whole image (pre-process)
	for (i = 0; i < (24 * 3); i++)
		image[i] = test_pattern[i];

	// Start with a clean slate
	R_ENABLE_HIGH;
	C_ENABLE_HIGH;
	clsRows();
	clsCols();
	C_ENABLE_LOW;
	R_ENABLE_LOW;
}


void display() {	
//	unsigned long start = micros();
	// Vars
	register short p, c, row, b;
	register unsigned char subRow;
	
	
	// Put up the image
	for (p = 0; p < NUM_ROWS; p += 2) {
//		unsigned long start = micros();
		row = p * 3;
		
		// Put up the column
		for (c = 2; c >= 0; c--) {			
		subRow = image[row + c];

			// Do a shift out, LSB First
			for (b = 0; b < 8; b++) {
				// If a bit is 1, make it HIGH, else LOW
				if (subRow & (1 << b))
					C_DATA_LOW;
				else
					C_DATA_HIGH;
				
				// Send the bit to the shift register
				C_CLOCK_HIGH;
				C_CLOCK_LOW;
			}
		}
		
		// the Row (need if block because of interlacting)
		if (p == 0) {
			// First pass, row 1 should be high, row 2 low
			R_DATA_LOW;
			R_CLOCK_HIGH;
			R_CLOCK_LOW;
			R_DATA_HIGH;
			R_CLOCK_HIGH;
			R_CLOCK_LOW;
		} else if (p == 1) {
			// Second pass, row 1 low, row 2 high
			R_DATA_HIGH;
			R_CLOCK_HIGH;
			R_CLOCK_LOW;
			R_DATA_LOW;
			R_CLOCK_HIGH;
			R_CLOCK_LOW;
		} else {
			// Shift down by two
			// Everything else should be low
			R_DATA_LOW;
			R_CLOCK_HIGH;
			R_CLOCK_LOW;
			R_DATA_LOW;
			R_CLOCK_HIGH;
			R_CLOCK_LOW;
		}
		
		// Send the buffers to output
		R_ENABLE_HIGH;
		C_ENABLE_HIGH;
		
		R_LATCH_LOW;
		R_LATCH_HIGH;
		C_LATCH_LOW;
		C_LATCH_HIGH;
		
		C_ENABLE_LOW;
		R_ENABLE_LOW;
		
		// for interlacing
		if (p == (NUM_ROWS - 2))
			p = -1;
	}
	
	//NOTE: This code was meant to clear out the last row, not sure if necessary or not.
//	R_DATA_LOW;
//	R_CLOCK_HIGH;
//	R_CLOCK_LOW;
//	R_LATCH_LOW;
//	R_LATCH_HIGH;
}


int main(int argc, char *argv[]) {
	unsigned int i;
	unsigned int secs = 15;
	clock_t cur;
	clock_t end;
	clock_t frameTime, avg;

	setup_gpio();
	setup();

	cur = clock();
	end = cur + (CLOCKS_PER_SEC * secs);
	while (cur < end) {
		// Display a frame and take the time
//		frameTime = clock();
		display();
//		frameTime = clock() - frameTime;
//		if (avg == 0)
//			avg = frameTime;
//		else {
//			avg += frameTime;
//			avg /= 2;
//		}
//		printf("%i\n", frameTime);

		// The main loop timer
		cur = clock();
		i += 1;
	}

	clsCols();
	clsRows();
	usleep(50000);

//	printf("average FPS: %f\n", (double)i / 15.0);
	printf("average FPS: %i\n", i / 15);

	return 0;
}

