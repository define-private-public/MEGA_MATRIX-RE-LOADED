// MEGA_MATRIX RE:LOADED, An NxM LED Matrix
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
int rData = 2;      // PD2
int rClock = 3;     // PD3
int rLatch = 4;     // PD4
int rEnable = 5;    // PD5
int rReset = 6;     // PD6

// Colunm: Acts as the Reicver, all pins on port B of atmega
int cData = 8;      // PB0
int cClock = 9;     // PB1
int cLatch = 10;    // PB2
int cEnable = 11;   // PB3
int cReset = 12;    // PB4

// The need for speed:
// For the Row controller
#define R_DATA_HIGH PORTD |= _BV(PD2)
#define R_DATA_LOW PORTD &= ~_BV(PD2)
#define R_CLOCK_HIGH PORTD |= _BV(PD3)
#define R_CLOCK_LOW PORTD &= ~_BV(PD3)
#define R_LATCH_HIGH PORTD |= _BV(PD4)
#define R_LATCH_LOW PORTD &= ~_BV(PD4)
#define R_ENABLE_HIGH PORTD |= _BV(PD5)
#define R_ENABLE_LOW PORTD &= ~_BV(PD5)
#define R_RESET_HIGH PORTD |= _BV(PD6)
#define R_RESET_LOW PORTD &= ~_BV(PD6)

// For the Column controller
#define C_DATA_HIGH PORTB |= _BV(PB0)
#define C_DATA_LOW PORTB &= ~_BV(PB0)
#define C_CLOCK_HIGH PORTB |= _BV(PB1)
#define C_CLOCK_LOW PORTB &= ~_BV(PB1)
#define C_LATCH_HIGH PORTB |= _BV(PB2)
#define C_LATCH_LOW PORTB &= ~_BV(PB2)
#define C_ENABLE_HIGH PORTB |= _BV(PB3)
#define C_ENABLE_LOW PORTB &= ~_BV(PB3)
#define C_RESET_HIGH PORTB |= _BV(PB4)
#define C_RESET_LOW PORTB &= ~_BV(PB4)


#define NUM_BYTES 144  				// 24 * 6
#define NUM_ROWS 24					// (in LEDs)
#define NUM_COLS 48					// (in LEDs)
#define NUM_ROWS_PER_COL 3			// (in Matrices)
#define NUM_COLS_PER_ROW 6			// (in Matrices)
boolean bufferReady = false;
byte bytesRead = 0;
byte image[NUM_BYTES];           // Image that's currently being displayed
byte image_buffer[NUM_BYTES];    // Buffer for next image


// Some patterns
byte CSH_logo[NUM_BYTES] = {
   B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
   B00111111, B11111111, B10011110, B00111111, B11111111, B10011110,
   B01111111, B11111111, B11011110, B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110, B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110, B01111111, B11111111, B11011110,
   B01111000, B00000011, B11011110, B01111000, B00000011, B11011110,
   B01111011, B11111011, B11011110, B01111011, B11111011, B11011110,
   B01111011, B11111000, B00011110, B01111011, B11111000, B00011110,
   B01111011, B11111011, B11011110, B01111011, B11111011, B11011110,
   B01111011, B10111011, B11011110, B01111011, B10111011, B11011110,
   B01111011, B10000011, B11111110, B01111011, B10000011, B11111110,
   B01111011, B11111011, B11111110, B01111011, B11111011, B11111110,
   B01111011, B11111011, B11111110, B01111011, B11111011, B11111110,
   B01111000, B00111011, B11111110, B01111000, B00111011, B11111110,
   B01111011, B10111011, B11011110, B01111011, B10111011, B11011110,
   B01111011, B11111011, B11011110, B01111011, B11111011, B11011110,
   B01111011, B11111000, B00011110, B01111011, B11111000, B00011110,
   B01111011, B11111011, B11011110, B01111011, B11111011, B11011110,
   B01111000, B00000011, B11011110, B01111000, B00000011, B11011110,
   B01111111, B11111111, B11011110, B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110, B01111111, B11111111, B11011110,
   B01111111, B11111111, B11011110, B01111111, B11111111, B11011110,
   B00111111, B11111111, B10011110, B00111111, B11111111, B10011110,
   B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
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
  R_RESET_LOW;
  R_RESET_HIGH;
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
  
  // XOR the whole image (pre-process)
  for (int i = 0; i < NUM_BYTES; i++)
    image[i] = CSH_logo[i];

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
//  byte bytesReady = Serial.available();
//  while (bytesReady > 0) {
//    // Get the next byte
//    
//    image_buffer[bytesRead] = (byte)Serial.read();
//    
//    bytesRead += 1;
//    
//    if (bytesRead == NUM_BYTES) {
//      // Read in a whole image, set it to display
//      memcpy(image, image_buffer, bytesRead);
//      bytesRead = 0;
//      bytesReady = 0;
//    } else
//      bytesReady = Serial.available();  // Loop again
//  }
  
  // Put up the image
  for (int r = 0; r < NUM_ROWS; r += 2) {
//    unsigned long start = micros();
    int row = r * NUM_COLS_PER_ROW;
    
    // Put up the column
    for (int c = (NUM_COLS_PER_ROW - 1); c >= 0; c--) {      
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
