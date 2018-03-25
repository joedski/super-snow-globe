#ifndef __PIXEL_H__
#define __PIXEL_H__ yay

#include <stdint.h>
#include "animation.h"


struct ColorTransferVector3 {
  // int32 because if we have only int16, we can't even get up to 2x.
  int32_t r;
  int32_t g;
  int32_t b;
};

struct ColorTransferMatrix3x3 {
  struct ColorTransferVector3 r;
  struct ColorTransferVector3 g;
  struct ColorTransferVector3 b;
};


struct PixelColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct PixelState {
  struct AnimationTimingModel timing;
  struct PixelColor color;
};


#endif
