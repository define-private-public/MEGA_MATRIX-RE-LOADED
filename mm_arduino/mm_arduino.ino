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


#define bit_get(p,m) ((p) & (m))
#define bit_set(p,m) ((p) |= (m))
#define bit_clear(p,m) ((p) &= ~(m))
#define bit_flip(p,m) ((p) ^= (m))
#define bit_write(c,p,m) (c ? bit_set(p,m) : bit_clear(p,m))
#define BIT(x) (0x01 << (x))

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

// Some handiness here
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
#define C_DATA_HIGH PORTB |= _BV(PB2)
#define C_DATA_LOW PORTB &= ~_BV(PB2)
#define C_CLOCK_HIGH PORTB |= _BV(PB3)
#define C_CLOCK_LOW PORTB &= ~_BV(PB3)
#define C_LATCH_HIGH PORTB |= _BV(PB4)
#define C_LATCH_LOW PORTB &= ~_BV(PB4)
#define C_ENABLE_HIGH PORTB |= _BV(PB5)
#define C_ENABLE_LOW PORTB &= ~_BV(PB5)
#define C_RESET_HIGH PORTB |= _BV(PB6)
#define C_RESET_LOW PORTB &= ~_BV(PB6)



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

byte test_pattern[24 * 3] = {
   B01001100, B00111011, B00111011,
   B01110000, B11000100, B11000100,
   B11110000, B00111011, B00111011,
   B01111100, B11000100, B11000100,
   B00001111, B00111011, B00111011,
   B11000000, B11000100, B11000100,
   B01111111, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
   B00111011, B00111011, B00111011,
   B11000100, B11000100, B11000100,
};

// Image that will be displayed
byte image[24 * 3] = {
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1,
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1,
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1,
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1,
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1,
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1, 
   1, 1, 1,
};


void clsCols() {
  // Clears out all of the columns (Turns them off by setting their values to 1)
  for (int c = 0; c < 3; c++)
    shiftOut(cData, cClock, LSBFIRST, B11111111);
  digitalWrite(cLatch, HIGH);
  digitalWrite(cLatch, LOW);
}

void clsRows() {
  // Clear out the shift register
  digitalWrite(rReset, LOW);
  digitalWrite(rReset, HIGH);
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
  digitalWrite(rEnable, LOW);
  digitalWrite(cEnable, LOW);
  digitalWrite(rReset, HIGH);
  digitalWrite(cReset, HIGH);
  
  // XOR the whole image (pre-process)
  for (int i = 0; i < (24 * 3); i++)
    image[i] = ~CSH_logo[i];

  clsRows();
  clsCols();
}

byte miniImage[8] = {
  B00000000,
  B11111110,
  B00000011,
  B11111000,
  B00001111,
  B11100000,
  B00111111,
  B10000000,
};

int ledMapRow[] = {
B10000000,
B01000000,
B00100000,
B00010000,
B00001000,
B00000100,
B00000010,
B00000001,
};

int ledMapColumn[] = {
B01111111,
B10111111,
B11011111,
B11101111,
B11110111,
B11111011,
B11111101,
B11111110,
};

void loop() {
 
/*
    for (int r = 0; r < 24; r++) {
      for (int c = 0; c < 24; c++) {
        digitalWrite(cData, (c == 0) ? LOW : HIGH);
        digitalWrite(cClock, HIGH);
        digitalWrite(cClock, LOW);
        digitalWrite(cLatch, HIGH);
        digitalWrite(cLatch, LOW);
        
        delay(5);
      }
      
      digitalWrite(rData, (r == 0) ? HIGH : LOW);
      digitalWrite(rClock, HIGH);
      digitalWrite(rClock, LOW);
      digitalWrite(rLatch, HIGH);
      digitalWrite(rLatch, LOW);
    }
    
    delay(50);
*/    
    
/*  
    for (int i = 0; i < 64; i++) {
    
    // take the latchPin low so 
    // the LEDs don't change while you're sending in bits:
    digitalWrite(rLatch, LOW);
    digitalWrite(cLatch, LOW);
    
    // shift out the bits:
    shiftOut(rData, rClock, LSBFIRST, ledMapRow[i / 8]);  
    shiftOut(cData, cClock, LSBFIRST, ledMapColumn[i % 8]);  

    //take the latch pin high so the LEDs will light up:
    digitalWrite(rLatch, HIGH);
    digitalWrite(cLatch, HIGH);
    
    // pause before next value
    delay(50);
  }
*/
  
  
  
/*
  // Set columns
  clsRows();
  for (int r = 0; r < 8; r++) {
    clsCols();
    byte row = B10000000 >> r;
    
    // The Column
    digitalWrite(cEnable, HIGH);
    shiftOut(cData, cClock, LSBFIRST, miniImage[r]);
    digitalWrite(cLatch, LOW);
    digitalWrite(cLatch, HIGH);
    digitalWrite(cEnable, LOW);
    
    // The row
    digitalWrite(rEnable, HIGH);
    shiftOut(rData, rClock, LSBFIRST, row);  
    digitalWrite(rLatch, LOW);
    digitalWrite(rLatch, HIGH);
    digitalWrite(rEnable, LOW);
  }
  // Turn off
  clsCols();
  clsRows();
  delay(0);
*/  
  
  // Put up the image
//  clsRows();
//  delay(10);
  // Rows
//    clsCols();
  unsigned long start = micros();
  for (int r = 0; r < 24; r += 2) {
//    digitalWrite(rEnable, HIGH); 
    
    // Columns
//    digitalWrite(cEnable, HIGH);
    for (int c = 2; c >= 0; c--)
      shiftOut(cData, cClock, LSBFIRST, image[(r * 3) + c ]);
//    digitalWrite(cEnable, LOW);
  
    // the Row (need if block because of interlacting)
    if (r == 0) {
      // First pass, row 1 should be high, row 2 low
      digitalWrite(rData, LOW);
      digitalWrite(rClock, HIGH);
      digitalWrite(rClock, LOW);
      digitalWrite(rData, HIGH);
      digitalWrite(rClock, HIGH);
      digitalWrite(rClock, LOW);
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
//      digitalWrite(rData, HIGH);
//      digitalWrite(rClock, HIGH);
//      digitalWrite(rClock, LOW);
//      digitalWrite(rData, LOW);
//      digitalWrite(rClock, HIGH);
//      digitalWrite(rClock, LOW);
    } else {
      // Shift down by two
      // Everything else should be low
//      digitalWrite(rData, LOW);
//      digitalWrite(rClock, HIGH);
//      digitalWrite(rClock, LOW);
//      digitalWrite(rData, LOW);
//      digitalWrite(rClock, HIGH);
//      digitalWrite(rClock, LOW);
      R_DATA_LOW;
      R_CLOCK_HIGH;
      R_CLOCK_LOW;
      R_DATA_LOW;
      R_CLOCK_HIGH;
      R_CLOCK_LOW;
    }
    
    // Hold for a bit until we're ready to write again (don't change anything yet).
//    while ((micros() - start) < 1000) {
//      delayMicroseconds(25);
//    }
    
    
    // Send te buffers to output
    digitalWrite(rLatch, LOW);
    digitalWrite(rLatch, HIGH);
    digitalWrite(cLatch, LOW);
    digitalWrite(cLatch, HIGH);
//    digitalWrite(rEnable, LOW);
//    Serial.println(micros() - start);
//    delayMicroseconds(250);
    
    // for interlacting
    if (r == 22)
      r = -1;
  }
  Serial.println(micros() - start);
//  clsCols();
//  clsRows();

//  delay(1);
}
