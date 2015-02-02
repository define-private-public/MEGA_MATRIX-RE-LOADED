
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <wiringPi.h>

#define PIN 25


int main(int argc, char *argv[]) {
	// setup
	wiringPiSetup();
	pinMode(PIN, OUTPUT);

	int i;
	for (i = 0; i < 2; i++) {
		digitalWrite(PIN, HIGH);
		delay(500);
		digitalWrite(PIN, LOW);
		delay(500);
	}

	
	return 0;
}

