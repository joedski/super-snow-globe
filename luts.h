#ifndef __LUTS_H__
#define __LUTS_H__ yay

#include <stdint.h>


// I placed these here in this header because C++ is nice and lets you do that.
// (without using extern, that is.)

// See here for an explanation: https://learn.adafruit.com/sipping-power-with-neopixels/demo-code
// See here for a description of the counter macro: https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
// Because these are PROGRMEM, you have to read them with `pgm_read_byte(&sineTable[i])`.

// 8-bit Cosine table.
const int _CSBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _CS1_ (cos((__COUNTER__ - _CSBASE_) / 128.0 * M_PI) + 1.0) * 127.5 + 0.5,
#define _CS2_ _CS1_ _CS1_ _CS1_ _CS1_ _CS1_ _CS1_ _CS1_ _CS1_ // Expands to 8 items
#define _CS3_ _CS2_ _CS2_ _CS2_ _CS2_ _CS2_ _CS2_ _CS2_ _CS2_ // Expands to 64 items
const uint8_t PROGMEM cosineTable[] = { _CS3_ _CS3_ _CS3_ _CS3_ }; // 256 items

// 8-bit Half Inverted Cosine table.
const int _IHCSBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _IHCS1_ (-cos((__COUNTER__ - _IHCSBASE_) / 256.0 * M_PI) + 1.0) * 127.5 + 0.5,
#define _IHCS2_ _IHCS1_ _IHCS1_ _IHCS1_ _IHCS1_ _IHCS1_ _IHCS1_ _IHCS1_ _IHCS1_ // Expands to 8 items
#define _IHCS3_ _IHCS2_ _IHCS2_ _IHCS2_ _IHCS2_ _IHCS2_ _IHCS2_ _IHCS2_ _IHCS2_ // Expands to 64 items
const uint8_t PROGMEM invHalfCosineTable[] = { _IHCS3_ _IHCS3_ _IHCS3_ _IHCS3_ }; // 256 items

// NOTE: I don't need a sine table.  Or, rather, the cosine table is more convenient for my needs.
// // 8-bit Sine table.
// const int _SBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
// #define _S1_ (sin((__COUNTER__ - _SBASE_) / 128.0 * M_PI) + 1.0) * 127.5 + 0.5,
// #define _S2_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ // Expands to 8 items
// #define _S3_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ // Expands to 64 items
// const uint8_t PROGMEM sineTable[] = { _S3_ _S3_ _S3_ _S3_ }; // 256 items

// Similar to above, but for an 8-bit gamma-correction table.
#define _GAMMA_ 2.6
const int _GBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _G1_ pow((__COUNTER__ - _GBASE_) / 255.0, _GAMMA_) * 255.0 + 0.5,
#define _G2_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ // Expands to 8 items
#define _G3_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ // Expands to 64 items
const uint8_t PROGMEM gammaTable[] = { _G3_ _G3_ _G3_ _G3_ }; // 256 items


#endif
