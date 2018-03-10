#ifndef __PIXEL_H__
#define __PIXEL_H__ yay


struct PixelColor {
  uint8_t: r;
  uint8_t: g;
  uint8_t: b;

  static struct PixelColor random();
  static struct PixelColor hsv(int32_t h, uint8_t s, uint8_t v, boolean gc = false);
};

struct PixelState {
  struct AnimationTimingModel timing;
  struct PixelColor color;

  void init();
};


#endif
