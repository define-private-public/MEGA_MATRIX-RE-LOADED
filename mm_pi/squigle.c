// A Sample application for the MEGA_MATRIX that runs on a RPi
// Author: Benjamin Summerton (define-private-public)
//
// Most of this code is based off of the elinux.org RPI Low Level Perirpherals
// page.


#define BCM2708_PERI_BASE   0x20000000
#define GPIO_BASE           (BCM2708_PERI_BASE + 0x2000000) /* GPIO Controler */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_SIZE (4 * 1024)
#define BLOCK_SIZE (4 * 1024)

int mem_fd;
void *gpio_map;

// I/O Access
volatile unsigned *gpio;

// GPIO setup macros, use INP_GPIO before using OUT_GPIO or SET_GPIO_ALT
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO(g) *(gpio + ((g) / 10)) |=  (1 << (((g) % 10) * 3))
#define SET_GPIO_ALT(g,a) *(gpio + (((g) / 10))) |= (((a) <= 3 ? (a) + 4 : (a) == 4 ? 3 : 2) << (((g) % 10) * 3))

#define GPIO_SET *(gpio + 7)	// Sets bits which are 1, ignore those are 0
#define GPIO_CLR *(gpio + 10)	// clears bits which are 1, ignore those are 0

#define GET_GPIO(g) (*(gpio + 13) & (1 << g))	// 0 if LOW, (1 << g) if HIGH


// Function prototypes
void setup_io();

// Setup memory regions to access GPIO
void setup_io() {
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
		printf("mmap error %d\n", (int)gpio_map);	// errno also set!
		exit(-1);
	}

	// Always use a volatile pointer!
	gpio = (volatile unsigned *)gpio_map;
}


int main(int argc, char *argv[]) {
	int g, rep;

	// Setup the gpio pointer
	setup_io();

	// Swtich GPIOs 7 though 11 to output mode
	for (g = 7; g <= 11; g++) {
		INP_GPIO(g);
		OUT_GPIO(g);
		GPIO_CLR = 1 << g;			// SET it LOW
	}

	// Toggle the GPIOs
	for (rep = 0; rep < 10; rep++) {
		
		printf("Repetition: %i\n", rep);

		for (g = 7; g <= 11; g++) {
			GPIO_SET = 1 << g;
			printf("  HIGH: %i\n", g);
			sleep(1);
			GPIO_CLR = 1 << g;
			printf("  LOW: %i\n", g);
			sleep(1);
		}
	}
	
	// Ran file
	return 0;
}

