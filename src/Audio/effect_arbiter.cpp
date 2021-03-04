#include "../../config.h"
#include <Arduino.h>
#include "../../linkage.h"
#include "effect_arbiter.h"
#include "sample_value.h"
#include "buffer_value.h"

/* 
** simplified arbiter that ors together any number of inputs
*/
void AudioEffectArbiter::update(void)
{
  audio_block_t *block[NVOX], *blockk, *blockp;
  int16_t *p[NVOX], run[NVOX], *pk, *pp, *end;
  int i;
  for (i = 0; i < NVOX; i += 1) {
    block[i] = receiveReadOnly(i);
    run[i] = run_length(block[i]);
    p[i] = block[i] ? block[i]->data : (int16_t *)zeros;
  }
  blockk = allocate();		// key out
  blockp = allocate();		// ptt out
  if (blockk && blockp) {
    int sumk = 0, sump = 0;;
    pk = blockk->data;
    end = pk + AUDIO_BLOCK_SAMPLES;
    pp = blockp->data;
    while (pk < end) {
      int16_t val = 0;
      for (i = 0; i < NVOX; i += 1) val |= *p[i]++;
      sumk += *pk++ = val;
      if (val) ptt_tail = get_vox_ptt_tail(active_vox);
      if (ptt_tail) {
	ptt_tail -= 1;
	sump += *pp++ = 1;
      } else
	sump += *pp++ = 0;
    }
    if (sumk != 0) transmit(blockk, 0);
    if (sump != 0) transmit(blockp, 1);
    release(blockk);
    release(blockp);
  }
  for (i = 0; i < NVOX; i += 1) 
    if (block[i])
      release(block[i]);
}

