#include <Adafruit_NeoPixel.h>
#include <stdint.h>

#include "animation.h"
#include "luts.h"
#include "pixel.h"


// 60fps = ~17ms (16.67ms), 30fps = ~33ms (33.33ms)
#define RUNLOOP_DELAY_MS 10
#define PIXEL_COUNT 6
#define DEBUG_PIN LED_BUILTIN
#define NEOPIXEL_PIN 6

// #define COLOR_WHITE  {0xFE, 0xE6, 0x8D} // A little too neutral...
// #define COLOR_WHITE  {0xFA, 0xDC, 0x6C} // A little too yellow???
#define COLOR_WHITE  {0xFF, 0xE4, 0x7E}
// #define COLOR_BLUE   {0x2D, 0x3E, 0x88} // Too dark and saturated?
// #define COLOR_BLUE   {0x1E, 0xC0, 0xE8} // Too bright.
// #define COLOR_BLUE   {0x00, 0x6F, 0xA6} // A little too bright, still.
// #define COLOR_BLUE   {0x3E, 0x4E, 0x95} // Still too blue.
// #define COLOR_BLUE   {0x2D, 0x4E, 0x88} // still to saturated?
// #define COLOR_BLUE   {0x35, 0x4A, 0x6E} // A little dark.
#define COLOR_BLUE   {0x39, 0x53, 0x7F}
// #define COLOR_GREEN  {0x0D, 0x93, 0x42} // Too blue?
// #define COLOR_GREEN  {0x0D, 0x93, 0x14} // Too saturated?
#define COLOR_GREEN  {0x4F, 0x9E, 0x60}
#define COLOR_YELLOW {0xFE, 0xB4, 0x00}
#define COLOR_ORANGE {0xF1, 0x5D, 0x00}
#define COLOR_RED    {0xEF, 0x2B, 0x00}
#define COLOR_PINK   {0xF6, 0x35, 0x84}


// Globals
// ================================
// These are, well, the globals in the project.  This is where we keep track of
// all of our state and such.

Adafruit_NeoPixel neoPixelStrip = Adafruit_NeoPixel(PIXEL_COUNT, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// I opted to cycle through colors deterministically to prevent over-prevalence
// of any one side of the color wheel, as well as to control the proportions of each side.
// Notice that the cool colors only show up 2/7, while the warm colors show up 3/7.
// The remaining 2/7 are of course white.
#define COLOR_CYCLE_LENGTH 7
struct PixelColor colorCycle[] = {
  COLOR_WHITE,
  COLOR_RED,
  COLOR_YELLOW,
  COLOR_WHITE,
  COLOR_BLUE,
  COLOR_PINK,
  COLOR_GREEN,
};

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
  char nextColor;

  void incrNextColor() {
    nextColor = (nextColor + 1) % COLOR_CYCLE_LENGTH;
  }
} globalState = {
  .timing = {0, 10000, 100},
  .millis = 0,
  .nextColor = 0,
};
// struct AnimationTimingModel globalTiming = {0, 10000, 100};
struct PixelState pixels[PIXEL_COUNT] = {};
// Give them a nice sequence that's not just in a circle.
uint8_t pixelOrder[PIXEL_COUNT] = {0, 2, 4, 1, 5, 3};


// Fun Times!
// ================================
// Here's all the actual interesting bits, the manipulators we use to do everything.
//
// NOTE: If there are any you don't use, either omit them entirely or at least comment them out!

// Utils
// --------
// These are basic utils used by the other sections.

/**
 * Perform a basic linear interpolation between two 8-bit values
 * parametrized by a third 8-bit progress value.
 * @param  a Starting value of the interpolation.
 * @param  b Ending value of the interpolation.
 * @param  t Progress along the interpolation. (0-255 = 0%-100%)
 * @return   The interpolated value.
 */
uint8_t lerp(uint8_t a, uint8_t b, uint8_t t) {
  uint16_t aa = a, bb = b, tt = t, itt = UINT8_MAX - t;
  uint16_t r = ((itt * aa >> 8) + (tt * bb >> 8));
  r = r > UINT8_MAX ? UINT8_MAX : r;
  return r;
}

/**
 * 8-bit Clamped-Ramp, or Cramp.  Sort of related to a Lerp, but rather than specifying two values
 * to interpolate between, you specify the two ends of the ramp between 0% (=0) and 100% (=255).
 * That is, if you pass a=64 and b=191 then t-values between 64 and 191 get mapped to an output
 * of 0 to 255, while t-values below 64 are mapped to 0 and t-values above or at 191
 * are mapped to 255.
 *
 * NOTE: If a >= b then you just have an abrupt change from 0 to 255 at t>a.
 * @param  a Lower-bound of the input that maps to 0 (0%) output.
 * @param  b Upper-bound of the input that maps to 255 (100%) output.
 * @param  t Input value to be remapped.
 * @return   Remapped clamped-ramped value.
 */
uint8_t cramp(uint8_t a, uint8_t b, uint8_t t) {
  if (t < a) return 0;
  if (t >= b) return UINT8_MAX;
  uint16_t aa = a, bb = b, tt = t;
  return (uint8_t)((tt - aa) * UINT8_MAX / (bb - aa));
}

/**
 * Convenience function to map a linear t-value to an inverted-half-cosine,
 * such that ihcs(t=0)=>0, ihcs(t=255)=>255, and ihcs(t=127)=>127 (or 128, but close enough).
 * It's an inverted half cosine because the cosine is flipped upside down and we only take
 * half the period, from 0 to pi.
 * @param  t Linear input value.
 * @return   Output value along the inverted cosine curve.
 */
uint8_t ihcs(uint8_t t) {
  return pgm_read_byte(&invHalfCosineTable[t]);
}

/**
 * Multiply two 8-bit values, treating them as percentages.
 * i.e. 255 (=100%) * 255 (=100%) = 255 (=100%),
 * or 64 (25%) * 128 (50%) = 32 (12.5%)
 * etc.
 * @param  a First operand.
 * @param  b Second operand.
 * @return   The two operands multiplied together.
 */
uint8_t multiply(uint8_t a, uint8_t b) {
  return (uint8_t)((uint16_t)a * (uint16_t)b >> 8);
}

/**
 * Convert a t-value into a simple triangle form that from an input of 0 to 127
 * rises from 0 to 255 on the output and from an input 128 to 255 falls from 255 back down
 * to 0 on the output.  Put another way, the input is scaled to twice its value
 * then any value above 255 is reflected back downards.
 *
 * Combining this with ihcs produces a full period of an inverted cosine.
 * @param  t Input progress value.
 * @return   Triangle wave.
 */
uint8_t triangle(uint8_t t) {
  return t < 128
    ? t << 1
    : (UINT8_MAX - t) << 1
  ;
}

uint8_t clamp(int32_t v) {
  if (v > UINT8_MAX) return UINT8_MAX;
  if (v < 0) return 0;
  return v;
}



// Mutators
// --------------
// This sounds fancier than it really is: All these functions are are functions which mutate
// a given data object.  Some are quite fancy, like mut_pseudoIncandescentRamp, while others are
// very simple, like mut_ramp or mut_gammaCorrect.
//
// These along with clone() allow for both flexible but efficient pipelines.  Not quite as flexible
// as pure inline code (though the compiler might take care of some of that) but not as hard to edit
// as pure inline code either.
//
// That said, if you wanted, you could easily inline everything once settling on a pipeline.

void mut_ihcs(PixelColor &color) {
  color.r = ihcs(color.r);
  color.g = ihcs(color.g);
  color.b = ihcs(color.b);
}

void mut_ramp(PixelColor &color, uint8_t t) {
  color.r = multiply(color.r, t);
  color.g = multiply(color.g, t);
  color.b = multiply(color.b, t);
}

void mut_pseudoIncandescentRamp(PixelColor &color, uint8_t t) {
  color.r = multiply(color.r, ihcs(cramp(0, 255, t)));
  color.g = multiply(color.g, ihcs(cramp(32, 255, t)));
  color.b = multiply(color.b, ihcs(cramp(64, 255, t)));
}

void mut_multiply(PixelColor &colorA, PixelColor &colorB) {
  colorA.r = multiply(colorA.r, colorB.r);
  colorA.g = multiply(colorA.g, colorB.g);
  colorA.b = multiply(colorA.b, colorB.b);
}

void mut_gammaCorrect(PixelColor &color) {
  color.r = pgm_read_byte(&gammaTable[color.r]);
  color.g = pgm_read_byte(&gammaTable[color.g]);
  color.b = pgm_read_byte(&gammaTable[color.b]);
}


// Derivers
// ---------------
// Creat new colors entirely rather than mudifying an existing one.
// Given that most of them can be treated as applying a mutator to a clone,
// there's not going to be many of these.

PixelColor clone(PixelColor &color) {
  return { .r = color.r, .g = color.g, .b = color.b };
}

PixelColor der_convolve(PixelColor &color, ColorTransferMatrix3x3 &transfer) {
  return {
    .r = clamp((
      (int32_t)color.r * transfer.r.r
      + (int32_t)color.g * transfer.r.g
      + (int32_t)color.b * transfer.r.b
    ) / 255),
    .g = clamp((
      (int32_t)color.r * transfer.g.r
      + (int32_t)color.g * transfer.g.g
      + (int32_t)color.b * transfer.g.b
    ) / 255),
    .b = clamp((
      (int32_t)color.r * transfer.b.r
      + (int32_t)color.g * transfer.b.g
      + (int32_t)color.b * transfer.b.b
    ) / 255),
  };
}


// Creators
// --------------
// Create colors from whole cloth!  Or at least from parameters.

// Found here: https://learn.adafruit.com/sipping-power-with-neopixels/demo-code
// Create an RGB color using HSV parameters.
PixelColor fromHSV(int32_t h, uint8_t s, uint8_t v, boolean gc = false) {
  uint8_t n, r, g, b;

  // Hue circle = 0 to 1530 (NOT 1536!)
  h %= 1530;           // -1529 to +1529
  if (h < 0) h += 1530; //     0 to +1529
  n  = h % 255;        // Angle within sextant; 0 to 254 (NOT 255!)
  switch (h / 255) {    // Sextant number; 0 to 5
    case 0 : r = 255    ; g =   n    ; b =   0    ; break; // R to Y
    case 1 : r = 254 - n; g = 255    ; b =   0    ; break; // Y to G
    case 2 : r =   0    ; g = 255    ; b =   n    ; break; // G to C
    case 3 : r =   0    ; g = 254 - n; b = 255    ; break; // C to B
    case 4 : r =   n    ; g =   0    ; b = 255    ; break; // B to M
    default: r = 255    ; g =   0    ; b = 254 - n; break; // M to R
  }

  uint32_t v1 =   1 + v; // 1 to 256; allows >>8 instead of /255
  uint16_t s1 =   1 + s; // 1 to 256; same reason
  uint8_t  s2 = 255 - s; // 255 to 0

  r = ((((r * s1) >> 8) + s2) * v1) >> 8;
  g = ((((g * s1) >> 8) + s2) * v1) >> 8;
  b = ((((b * s1) >> 8) + s2) * v1) >> 8;

  if (gc) { // Apply gamma correction?
    r = pgm_read_byte(&gammaTable[r]);
    g = pgm_read_byte(&gammaTable[g]);
    b = pgm_read_byte(&gammaTable[b]);
  }

  // return ((uint32_t)r << 16) | ((uint16_t)g << 8) | b;
  return {r, g, b};
}

PixelColor randomHue(uint8_t s = 255, uint8_t v = 255, boolean gc = false) {
  return fromHSV(random(1530), s, v, gc);
}

PixelColor renderScaledIHCS(PixelState &pixel) {
  uint8_t pixelProgress = pixel.timing.progress8();
  uint8_t heatT = pixelProgress < 128 ? pixelProgress << 1 : (UINT8_MAX - pixelProgress) << 1;

  PixelColor renderedColor = {
    .r = ihcs(cramp(0, 255, heatT)),
    .g = ihcs(cramp(32, 255, heatT)),
    .b = ihcs(cramp(64, 255, heatT)),
  };

  return renderedColor;
}



// Implementation
// ================================
// Do the things!

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
      pixels[i].color = colorCycle[globalState.nextColor];
      globalState.incrNextColor();
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
  // -------- Handle Trigger Conditions
  // TODO: Handle any set trigger states!

  // -------- Update Paramters
  // TODO: Integrate global timing based params.
  // globalState.timing.step(RUNLOOP_DELAY_MS);
  for (char i = 0; i < PIXEL_COUNT; ++i) {
    if (pixels[i].timing.progress == ANIMATION_PRORGESS_MAX) {
      pixels[i].timing.progress = 0;
      pixels[i].color = colorCycle[globalState.nextColor];
      globalState.incrNextColor();
      // pixels[i].color = randomHue();
      // pixels[i].color = der_convolve(pixels[i].color, warmingTransfer);
    }
    else {
      pixels[i].timing.increment(RUNLOOP_DELAY_MS);
    }
  }
}



void loop_write() {
  for (char i = 0; i < PIXEL_COUNT; ++i) {
    // Our actual pixel animation.
    PixelColor renderedColor = clone(pixels[i].color);
    uint8_t pixelProgress = pixels[i].timing.progress8();
    uint8_t pixelValueLinear = triangle(pixelProgress);

    mut_pseudoIncandescentRamp(renderedColor, pixelValueLinear);
    mut_gammaCorrect(renderedColor);

    neoPixelStrip.setPixelColor(i, neoPixelStrip.Color(renderedColor.r, renderedColor.g, renderedColor.b));
  }

  neoPixelStrip.show();

  // Blank out the pixels after showing them to prepare for next time. (I think...?)
  for (char i = 0; i < PIXEL_COUNT; ++i) {
    neoPixelStrip.setPixelColor(i, 0);
  }
}
