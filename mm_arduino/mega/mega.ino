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


// Row: Acts as the emmiter, all pins on port D of atmega
int rData = 2;      // PE4
int rClock = 3;     // PE5
int rLatch = 4;     // PG5
int rEnable = 5;    // PE3
int rReset = 6;     // PH3

// Colunm: Acts as the Reicver, all pins on port B of atmega
int cData = 8;      // PH5
int cClock = 9;     // PH6
int cLatch = 10;    // PB4
int cEnable = 11;   // PB5
int cReset = 12;    // PB6

// The need for speed:
// For the Row controller
#define R_DATA_HIGH PORTE |= _BV(PE4)
#define R_DATA_LOW PORTE &= ~_BV(PE4)
#define R_CLOCK_HIGH PORTE |= _BV(PE5)
#define R_CLOCK_LOW PORTE &= ~_BV(PE5)
#define R_LATCH_HIGH PORTG |= _BV(PG5)
#define R_LATCH_LOW PORTG &= ~_BV(PG5)
#define R_ENABLE_HIGH PORTE |= _BV(PE3)
#define R_ENABLE_LOW PORTE &= ~_BV(PE3)
#define R_RESET_HIGH PORTH |= _BV(PH3)
#define R_RESET_LOW PORTH &= ~_BV(PH3)

// For the Column controller
#define C_DATA_HIGH PORTH |= _BV(PH5)
#define C_DATA_LOW PORTH &= ~_BV(PH5)
#define C_CLOCK_HIGH PORTH |= _BV(PH6)
#define C_CLOCK_LOW PORTH &= ~_BV(PH6)
#define C_LATCH_HIGH PORTB |= _BV(PB4)
#define C_LATCH_LOW PORTB &= ~_BV(PB4)
#define C_ENABLE_HIGH PORTB |= _BV(PB5)
#define C_ENABLE_LOW PORTB &= ~_BV(PB5)
#define C_RESET_HIGH PORTB |= _BV(PB6)
#define C_RESET_LOW PORTB &= ~_BV(PB6)

// NOTE Could consolidate the Reset and enable pins possibly
//      as well as the latch.  Test this later.





#define NUM_BYTES 72  // 24 * 3
#define NUM_ROWS 24
#define NUM_COLS 24
boolean bufferReady = false;
byte bytesRead = 0;
byte image[NUM_BYTES];           // Image that's currently being displayed
byte image_buffer[NUM_BYTES];    // Buffer for next image


// Some patterns
byte CSH_logo[NUM_BYTES] = {
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
   B01111111, B11111111, B11011110,
   B00111111, B11111111, B10011110,
   B00000000, B00000000, B00000000,
};

byte test_pattern[NUM_BYTES] = {
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
  B01010101, B01010101, B01010101,
  B10101010, B10101010, B10101010,
};



void clsCols() {
  // Clears out all of the columns (Turns them off by setting their values to 1)
  C_DATA_HIGH;
  for (int c = 0; c < NUM_COLS; c++) {
    C_CLOCK_HIGH;
    C_CLOCK_LOW;
  }
  C_DATA_LOW;
    
  C_LATCH_HIGH;
  C_LATCH_LOW;
}


void clsRows() {
  // Clear out the shift register
  C_RESET_LOW;
  C_RESET_HIGH;
}


void setup() {
  Serial.begin(115200);
  
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
  R_ENABLE_LOW;
  C_ENABLE_LOW;
  R_RESET_HIGH;
  C_RESET_HIGH;
  
//  // XOR the whole image (pre-process)
//  for (int i = 0; i < (24 * 3); i++)
//    image[i] = test_pattern[i];

  // Start with a clean slate
  R_ENABLE_HIGH;
  C_ENABLE_HIGH;
  clsRows();
  clsCols();
  C_ENABLE_LOW;
  R_ENABLE_LOW;
}


void loop() {  
//  unsigned long start = micros();
  
  // This is not using buffered IO like it should, will figure that out later
  byte bytesReady = Serial.available();
  while (bytesReady > 0) {
    // Get the next byte
    
    image_buffer[bytesRead] = (byte)Serial.read();
    
    bytesRead += 1;
    
    if (bytesRead == 72) {
      // Read in a whole image, set it to display
      memcpy(image, image_buffer, bytesRead);
      bytesRead = 0;
      bytesReady = 0;
    } else
      bytesReady = Serial.available();  // Loop again
  }
  
  // Put up the image
  for (int r = 0; r < NUM_ROWS; r += 2) {
//    unsigned long start = micros();
    int row = r * 3;
    
    // Put up the column
    for (int c = 2; c >= 0; c--) {      
      // Do a shift out, LSB First
      for (int b = 0; b < 8; b++) {
        // If a bit is 1, make it HIGH, else LOW
        if (image[row + c] & (1 << b))
          C_DATA_LOW;
        else
          C_DATA_HIGH;
        
        // Send the bit to the shift register
        C_CLOCK_HIGH;
        C_CLOCK_LOW;
      }
    }
    
    // the Row (need if block because of interlacting)
    if (r == 0) {
      // First pass, row 1 should be high, row 2 low
      R_DATA_LOW;
      R_CLOCK_HIGH;
      R_CLOCK_LOW;
      R_DATA_HIGH;
      R_CLOCK_HIGH;
      R_CLOCK_LOW;
    } else if (r == 1) {
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
    
//    unsigned long start = micros();
    
    // Send the buffers to output
    R_ENABLE_HIGH;
    C_ENABLE_HIGH;
    
    R_LATCH_LOW;
    R_LATCH_HIGH;
    C_LATCH_LOW;
    C_LATCH_HIGH;
    
    C_ENABLE_LOW;
    R_ENABLE_LOW;
    
//    while ((start + 100) > micros())
//      delayMicroseconds(10);
    
    // for interlacing
    if (r == (NUM_ROWS - 2))
      r = -1;
  }
  
//  NOTE: This code was meant to clear out the last row, not sure if necessary or not.
//  R_DATA_LOW;
//  R_CLOCK_HIGH;
//  R_CLOCK_LOW;
//  R_LATCH_LOW;
//  R_LATCH_HIGH;
  
//  Serial.println(micros() - start);
}
