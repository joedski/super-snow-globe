#include <Adafruit_NeoPixel.h>

#include "animation.h"
#include "luts.h"
#include "pixel.h"


// 60fps = ~17ms (16.67ms), 30fps = ~33ms (33.33ms)
#define RUNLOOP_DELAY_MS 10
#define PIXEL_COUNT 6
#define DEBUG_PIN LED_BUILTIN
#define NEOPIXEL_PIN 6


Adafruit_NeoPixel neoPixelStrip = Adafruit_NeoPixel(PIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);


// This works pretty well to take a fully saturated random hue and making it nice and warm
// almost like the incandescent lights of yore.
// Red and Gold are the richest, while blues are very muted.
struct ColorTransferMatrix3x3 warmingTransfer = {
  {255*0.75, 255*0.25, 255*0.375},
  {255*0.0, 255*0.675, 255*0.125},
  {255*0.0, 255*0.0, 255*0.55}
};

struct GlobalAnimationState {
  struct AnimationTimingModel timing;
  unsigned long millis;
} globalState = {
  .timing = {0, 10000, 100}
};
// struct AnimationTimingModel globalTiming = {0, 10000, 100};
struct PixelState pixels[PIXEL_COUNT] = {};
// Give them a nice sequence that's not just in a circle.
uint8_t pixelOrder[PIXEL_COUNT] = {0, 2, 4, 1, 5, 3};


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Initialize anything timing-sensitive in the global state.
  globalState.millis = millis();

  // Initialize our pixels for the first time.
  char i;

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
      2000, // 500,
      100
    };
  }

  // Initialize the NeoPixel library.
  neoPixelStrip.begin();
}

void loop_read();
void loop_update();
void loop_write();

void loop() {
  // ======== Read
  // TODO: This!
  loop_read();

  // ======== Update
  loop_update();

  // ======== Write
  loop_write();

  // ======== Wait
  // I do this to keep a steady ~60fps.
  long nextMillis = millis();
  long runloopDuration = nextMillis - globalState.millis;
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
  globalState.millis = nextMillis;
}


// ======== Loop Parts.

/**
 * Here's where we read all our inputs and set any trigger states.
 * I chose to do it this way because we can't use interrupts
 * due to the timing sensitive nature of driving NeoPixels.
 *
 * Since all of the state is global, we don't need to pass anything
 * out in order for it to reach loop_update.
 */
void loop_read() {
  // TODO: This!
}


/**
 * In here, animation parameters are updated, and pixels are
 * started on new animations when necessary.
 */
void loop_update() {
  char i;

  // -------- Handle Trigger Conditions
  // TODO: Handle any set trigger states!

  // -------- Update Paramters
  // TODO: Integrate global timing based params.
  // globalState.timing.step(RUNLOOP_DELAY_MS);
  for (i = 0; i < PIXEL_COUNT; ++i) {
    if (pixels[i].timing.progress == ANIMATION_PRORGESS_MAX) {
      pixels[i].timing.progress = 0;
      pixels[i].color = PixelColor::random().convolveColor3(warmingTransfer);
    }
    else {
      pixels[i].timing.increment(RUNLOOP_DELAY_MS);
    }
  }
}

void loop_write() {
  char i;
  struct PixelColor renderedColor;

  for (i = 0; i < PIXEL_COUNT; ++i) {
    // Our actual pixel animation:
    // going from t=0.0 to t=1.0, we first ramp up the sine curve then back down.
    // If we were calculating sines directly, we cauld just scale by 2pi, but eh.
    // NOTE: I originally thought the sine table was a half cosine for some reason. Herp.
    // uint8_t valueScaleLookup = pixels[i].timing.progress > 32767
    //   ? 256 - pixels[i].timing.progress8() * 2
    //   : pixels[i].timing.progress8() * 2
    //   ;
    // uint8_t valueScale =  pgm_read_byte(&sineTable[valueScaleLookup]);
    uint8_t valueScale = 255 - pgm_read_byte(&cosineTable[pixels[i].timing.progress8()]);
    // A bit inefficient since we're creating a couple new pixels in memory each time,
    // but we're not the most timing critical here.
    renderedColor = pixels[i].color.valueScaled(valueScale).gammaCorrected();
    neoPixelStrip.setPixelColor(i, neoPixelStrip.Color(renderedColor.r, renderedColor.g, renderedColor.b));
  }

  neoPixelStrip.show();

  // Blank out the pixels after showing them to prepare for next time. (I think...?)
  for (i = 0; i < PIXEL_COUNT; ++i) {
    neoPixelStrip.setPixelColor(i, 0);
  }
}
