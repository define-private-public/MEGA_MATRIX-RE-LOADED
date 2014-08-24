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
------------------+-----------------
// (n-1):     0   |   x   x   x   x
// (n):       1   |   0   0   1   0
// (n+1):     0   |   x   x   x   x

// x = Don't care
// 0 = LOW
// 1 = HIGH


// Row: Acts as the emmiter


// Colum: Acts as the Reicver
