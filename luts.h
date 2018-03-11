#ifndef __LUTS_H__
#define __LUTS_H__ yay

#include <stdint.h>


// I placed these here in this header because C++ is nice and lets you do that.
// (without using extern, that is.)

// See here for an explanation: https://learn.adafruit.com/sipping-power-with-neopixels/demo-code
// Because these are PROGRMEM, you have to read them with `pgm_read_byte(&sineTable[i])`.

// 8-bit Sine table.
const int _CSBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _S1_ (cos((__COUNTER__ - _CSBASE_) / 128.0 * M_PI) + 1.0) * 127.5 + 0.5,
#define _S2_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ // Expands to 8 items
#define _S3_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ // Expands to 64 items
const uint8_t PROGMEM cosineTable[] = { _S3_ _S3_ _S3_ _S3_ }; // 256 items

// NOTE: I don't need a size table.  Or, rather, the cosine table is more convenient for my needs.
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
