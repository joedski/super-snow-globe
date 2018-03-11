#ifndef __PIXEL_H__
#define __PIXEL_H__ yay

#include <stdint.h>
#include "animation.h"


struct PixelColor {
  uint8_t r;
  uint8_t g;
  uint8_t b;

  // Factories
  static struct PixelColor random(uint8_t s = 255, uint8_t v = 255, boolean gc = false);
  static struct PixelColor hsv(int32_t h, uint8_t s, uint8_t v, boolean gc = false);

  // Derivers
  struct PixelColor gammaCorrected();
  struct PixelColor valueScaled(uint8_t v);
};

struct PixelState {
  struct AnimationTimingModel timing;
  struct PixelColor color;
};


#endif
