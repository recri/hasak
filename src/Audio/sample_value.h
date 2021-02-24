#ifndef sample_value_h_
#define sample_value_h_
/*
	void amplitude(float n) {
		if (n > 1.0) n = 1.0;
		else if (n < -1.0) n = -1.0;
		int32_t m = (int32_t)(n * 2147418112.0);
*/

static inline int16_t bool2fix(uint8_t b) { return (((uint16_t)b)<<15); }
static inline uint8_t fix2bool(uint16_t f) { return (f>>15); }

extern const int16_t zeros[AUDIO_BLOCK_SAMPLES];
extern const int16_t ones[AUDIO_BLOCK_SAMPLES];

static inline int allzeros(audio_block_t *bp) {
  return (! bp) || (bcmp(bp->data, zeros, sizeof(zeros)) == 0);
}

static inline int allones(audio_block_t *bp) {
  return bp && (bcmp(bp->data, ones, sizeof(ones)) == 0);
}
#endif
