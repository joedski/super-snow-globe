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

  // Factories
  static struct PixelColor random(uint8_t s = 255, uint8_t v = 255, boolean gc = false);
  static struct PixelColor hsv(int32_t h, uint8_t s, uint8_t v, boolean gc = false);

  // Derivers

  /**
   * Get a gamma corrected version of a color.
   */
  struct PixelColor gammaCorrected();

  /**
   * Scale a value from 0% to 100%. (0 to 255)
   * @param  v Value to scale by.
   */
  struct PixelColor valueScaled(uint8_t v);

  /**
   * Convolve a color by a transfer matrix.
   * @param  transfer A ColorTransferMatrix3x3.
   */
  struct PixelColor convolveColor3(ColorTransferMatrix3x3 &transfer);
};

struct PixelState {
  struct AnimationTimingModel timing;
  struct PixelColor color;
};


#endif
