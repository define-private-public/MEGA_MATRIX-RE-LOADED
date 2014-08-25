// Mega Matrix: A 24x24 LED Matrix
//
// Our matrices are Command cathode; a 3x3 array of 8x8 matrices.
// The row controllers will act as emitters, and the columns as
// receivers.  (Using 74HC595 shift registers)
//
// To denote patterns for ourselves (not the device) we'll have
//   0 = LED Off
//   1 = LED On
// 
// Though the machine will flip the bits for us to turn the correct
// LEDs on.
//
// Say we want to light up a row like this:  1 1 0 1
// We Have to do this:
//           Cols:|   1   2   3   4 
//  Rows:         |
// ---------------+-----------------
// (n-1):     0   |   x   x   x   x
// (n):       1   |   0   0   1   0
// (n+1):     0   |   x   x   x   x

// x = Don't care
// 0 = LOW
// 1 = HIGH

// Row: Acts as the emmiter
int rData = 2;
int rClock = 3;
int rLatch = 4;
int rEnable = 5;
int rReset = 6;

// Colum: Acts as the Reicver
int cData = 8;
int cClock = 9;
int cLatch = 10;
int cEnable = 11;
int cReset = 12;

// NOTE Could consolidate the Reset and enable pins possibly
//      as well as the latch.  Test this later.


// Some patterns
byte CSH_logo[24 * 3] = {
   B00000000, B00000000, B00000000,
   B00111111, B11111111, B10011110,
   B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110,
   B01111000, B00000011, B11011110,
   B01111011, B11111011, B11011110,
   B01111011, B11111000, B00011110,
   B01111011, B11111011, B11011110,
   B01111011, B10111011, B11011110,
   B01111011, B10000011, B11111110,
   B01111011, B11111011, B11111110,
   B01111011, B11111011, B11111110,
   B01111000, B00111011, B11111110,
   B01111011, B10111011, B11011110,
   B01111011, B11111011, B11011110,
   B01111011, B11111000, B00011110,
   B01111011, B11111011, B11011110,
   B01111000, B00000011, B11011110,
   B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110,
   B00111111, B11111111, B10011110,
   B00000000, B00000000, B00000000,
};

// Image that will be displayed
byte image[24 * 3] = {
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


void setup() {
  // Set everything to output
  pinMode(rData, OUTPUT);
  pinMode(rClock, OUTPUT);
  pinMode(rLatch, OUTPUT);
  pinMode(rEnable, OUTPUT);
  pinMode(rReset, OUTPUT);
  pinMode(cData, OUTPUT);
  pinMode(cClock, OUTPUT);
  pinMode(cLatch, OUTPUT);
  pinMode(cEnable, OUTPUT);
  pinMode(cReset, OUTPUT);
  
  // Turn on output, disable reset
  digitalWrite(rEnable, LOW);
  digitalWrite(cEnable, LOW);
  digitalWrite(rReset, HIGH);
  digitalWrite(cReset, HIGH);
  
  // XOR the whole image (pre-process)
  for (int i = 0; i < (24 * 3); i++)
    image[i] ^= CSH_logo[i];
}

void loop() {  
  // Put up the image
  for (int r = 0; r < 32; r++) {
   
   // Latches go LOW
   digitalWrite(rLatch, LOW);
   digitalWrite(cLatch, LOW);
   
   for (int c = 0; c < 3; c++) {
     // Setup the "receivers"
     shiftOut(cData, cClock, MSBFIRST, image[c]);
   }
   
   // Change the "emiiter" row
   byte newBit = (r == 0) ? HIGH : LOW;
   
   digitalWrite(rData, newBit);
   digitalWrite(rClock, HIGH);
   digitalWrite(rClock, LOW);
   
   
   // Turn the latches on
   digitalWrite(rLatch, HIGH);
   digitalWrite(cLatch, HIGH);
  }
  
  delay(10);
}
