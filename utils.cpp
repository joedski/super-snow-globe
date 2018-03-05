#define MAX_UINT16 0xffff
#define MAX_UINT32 0xffffffff

int16_t lerp_16(int32_t a, int32_t b, uint32_t t) {
  return (int16_t)(((MAX_UINT16 - t) * a + t * b) / t);
}
