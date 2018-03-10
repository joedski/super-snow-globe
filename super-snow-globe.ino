#include "animation.h"
#include "luts.h"
#include "pixel.h"


// 60fps = ~17ms (16.67ms), 30fps = ~33ms (33.33ms)
#define RUNLOOP_DELAY_MS 17
#define PIXEL_COUNT 6
// #define DEBUG_LED_PIN 13


struct AnimationTimingModel globalTiming = {0, 10000, 100};
struct PixelState pixels[PIXEL_COUNT] = {};
// Give them a nice sequence that's not just in a circle.
uint8_t pixelOrder[PIXEL_COUNT] = {0, 2, 4, 1, 5, 3};
unsigned long lastMillis;


void setup() {
  // Initialize our pixels for the first time.

  int i;

  for (i = 0; i < PIXEL_COUNT; ++i) {
    // Just to make things start out nicely, we initialize all but the first one
    // to black.
    if (pixelOrder[i] == 0) {
      pixels[i].color = PixelColor::random();
    }
    else {
      pixels[i].color = {0, 0, 0};
    }

    pixels[i].timing = {
      (uint16_t)((uint32_t)ANIMATION_PRORGESS_MAX * (uint32_t)pixelOrder[i] / PIXEL_COUNT),
      500,
      100
    };
  }

  lastMillis = millis();
}

void loop() {
  // ... stuff.
  // TODO: Test loop.

  long runloopDuration = millis() - lastMillis;
  long remainingDelay = RUNLOOP_DELAY_MS - runloopDuration;
  int animationTimeOverran = remainingDelay < 0;

#ifdef DEBUG_LED_PIN
  if(animationTimeOverran) {
    digitalWrite(DEBUG_LED_PIN, HIGH);
  }
  else {
    digitalWrite(DEBUG_LED_PIN, LOW);
  }
#endif

  delay(animationTimeOverran ? 0 : remainingDelay);
}
