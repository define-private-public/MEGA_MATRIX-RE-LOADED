// Mega Matrix: A 24x24 LED Matrix
//
// Our matrices are Command cathode; a 3x3 array of 8x8 matrices.
// The row controllers will act as emitters, and the columns as
// receivers.  (Using 74HC595 shift registers)
// 
// To Say to light up a row like this:  0  0 . 0
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
}

void loop() {
  
}
