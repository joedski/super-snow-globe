#ifndef __PIXEL_H__
#define __PIXEL_H__ yay


struct PixelColor {
  uint8_t: r;
  uint8_t: g;
  uint8_t: b;
};

struct PixelState {
  struct AnimationTimingModel timing;
  struct PixelColor color;

  void init();
};


#endif
