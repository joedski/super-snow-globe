#include "animation.h"


// See here for an explanation: https://learn.adafruit.com/sipping-power-with-neopixels/demo-code
// Because these are PROGRMEM, you have to read them with `pgm_read_byte(&sineTable[i])`.
// 8-bit Sine table.
const int _SBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _S1_ (sin((__COUNTER__ - _SBASE_) / 128.0 * M_PI) + 1.0) * 127.5 + 0.5,
#define _S2_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ _S1_ // Expands to 8 items
#define _S3_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ _S2_ // Expands to 64 items
const uint8_t PROGMEM sineTable[] = { _S3_ _S3_ _S3_ _S3_ }; // 256 items

// Similar to above, but for an 8-bit gamma-correction table.
#define _GAMMA_ 2.6
const int _GBASE_ = __COUNTER__ + 1; // Index of 1st __COUNTER__ ref below
#define _G1_ pow((__COUNTER__ - _GBASE_) / 255.0, _GAMMA_) * 255.0 + 0.5,
#define _G2_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ _G1_ // Expands to 8 items
#define _G3_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ _G2_ // Expands to 64 items
const uint8_t PROGMEM gammaTable[] = { _G3_ _G3_ _G3_ _G3_ }; // 256 items


struct PixelColor {
  uint8_t: r;
  uint8_t: g;
  uint8_t: b;
};

struct PixelState {
  struct AnimationTimingModel timing;
  struct PixelColor color;

  void init() {
    timing.progress = 0;
    timing.normalDuration = 500;
    timing.rate100 = lerp_16(100, 50, globalTiming.progress);
    // ...?  Pick color here?
  }
};


#define PIXEL_COUNT 6

struct AnimationTimingModel globalTiming = {0, 10000, 100};
struct PixelState[PIXEL_COUNT];


void setup() {
  // Initialize our pixels for the first time.
}
