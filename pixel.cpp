#include <Arduino.h>
#include "luts.h"
#include "utils.h"
#include "pixel.h"


uint8_t clampColorValueUint16(uint16_t v) {
  if (v > UINT8_MAX) return UINT8_MAX;
  if (v < 0) return 0;
  return v;
}

uint8_t clampColorValueInt32(int32_t v) {
  if (v > UINT8_MAX) return UINT8_MAX;
  if (v < 0) return 0;
  return v;
}

/**
 * multiply two bytes as though they were floats from 0 to 1.
 * @param  a operand
 * @param  b operand
 * @return   a * b
 */
uint8_t multiply(uint8_t a, uint8_t b) {
  return (uint8_t)((uint16_t)a * (uint16_t)b >> 8);
}


// Pick a color randomly along the color wheel using the given value, saturation,
// and gamma correction option.
struct PixelColor PixelColor::random(uint8_t s = 255, uint8_t v = 255, boolean gc = false) {
  return PixelColor::hsv(::random(1530), s, v, gc);
}


// TODO: Should I make this a method?  "setFromHSV"?
// Found here: https://learn.adafruit.com/sipping-power-with-neopixels/demo-code
// HSV (hue-saturation-value) to RGB function with optional gamma correction.
struct PixelColor PixelColor::hsv(int32_t h, uint8_t s, uint8_t v, boolean gc = false) {
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

  if (gc) { // Gamma correct?
    r = pgm_read_byte(&gammaTable[r]);
    g = pgm_read_byte(&gammaTable[g]);
    b = pgm_read_byte(&gammaTable[b]);
  }

  // return ((uint32_t)r << 16) | ((uint16_t)g << 8) | b;
  return {r, g, b};
}

struct PixelColor PixelColor::gammaCorrected() {
  return {
    pgm_read_byte(&gammaTable[r]),
    pgm_read_byte(&gammaTable[g]),
    pgm_read_byte(&gammaTable[b])
  };
}

struct PixelColor PixelColor::valueScaled(uint8_t v) {
  return {
    clampColorValueUint16((uint16_t)r * (uint16_t)v / 255),
    clampColorValueUint16((uint16_t)g * (uint16_t)v / 255),
    clampColorValueUint16((uint16_t)b * (uint16_t)v / 255),
  };
}

struct PixelColor PixelColor::convolveColor3(ColorTransferMatrix3x3 &transfer) {
  return {
    .r = clampColorValueInt32((
      (int32_t)r * transfer.r.r
      + (int32_t)g * transfer.r.g
      + (int32_t)b * transfer.r.b
    ) / 255),
    .g = clampColorValueInt32((
      (int32_t)r * transfer.g.r
      + (int32_t)g * transfer.g.g
      + (int32_t)b * transfer.g.b
    ) / 255),
    .b = clampColorValueInt32((
      (int32_t)r * transfer.b.r
      + (int32_t)g * transfer.b.g
      + (int32_t)b * transfer.b.b
    ) / 255),
  };
}

struct PixelColor PixelColor::multiply(struct PixelColor &other) {
  return {
    .r = ::multiply(r, other.r),
    .g = ::multiply(g, other.g),
    .b = ::multiply(b, other.b),
  };
}
