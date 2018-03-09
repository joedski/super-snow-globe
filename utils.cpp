#include "utils.h"

#define MAX_UINT16 0xffff
#define MAX_UINT32 0xffffffff

int16_t lerp_16(int16_t a, int16_t b, uint16_t t) {
  // t also gets converted to a _signed_ 32-bit integer here
  // just for less ambiguity in the calculation below.
  int32_t aa = a, bb = b, tt = t;
  return (int16_t)(((MAX_UINT16 - tt) * aa + tt * bb) / tt);
}
