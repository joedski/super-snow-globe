#ifndef __ANIMATION_H__
#define __ANIMATION_H__ yay


// Progress max value.
#define ANIMATION_PRORGESS_MAX 65535
// Max Progress as an Unsigned Long to prevent undesired int casting.
#define ANIMATION_PRORGESS_MAX_UL 65535ul

// Represents the linear progress of an animation.
struct AnimationTimingModel {
  // Current progress of this animation.
  // Varies between 0 and ANIMATION_PRORGESS_MAX in normal use.
  uint16_t progress;

  // Duration in milliseconds when playing at 1x rate (rate100 = 100).
  uint32_t normalDuration;

  // Animation rate, stored as an integer 100 times the rate multiplier.
  // Thus:
  // - for 1x (forward), rate100 = 100;
  // - for -1x (backward), rate100 = -100;
  // - for stopped, rate100 = 0;
  // etc.
  int16_t rate100;


  // ======== Mutations

  // Increments the timing model by some time delta, according to its rate.
  void increment(uint32_t timeDelta);

  // Same as increment, but rather than stopping at ANIMATION_PRORGESS_MAX or 0,
  // the animation will instead overflow to the other end.
  // i.e. if progress == ANIMATION_PRORGESS_MAX - 1,
  //   then progress += 2 will result in progress == 2.
  void incrementCyclic(uint32_t timeDelta);


  // ======== Derivers

  // Convenience method returning the progress as an unsigned 8-bit,
  // usually for LUTs and such.
  uint8_t progress8() {
    return progress / 256;
  }
};


#endif
