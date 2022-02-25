/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice, development funding notice, and this permission
 * notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/* Ramp code derived from Audio Library for Teensy 3.X
   effect_fade.h and effect_fade.cpp rather than written
   from scratch.  Should use ramps computed for ramp time
   to eliminate the interpolation.
*/
#include <Arduino.h>
#include "synth_keyed_tone.h"
#include "utility/dspinst.h"

void AudioSynthKeyedTone::update(void)
{
  audio_block_t *blockk, *blocki, *blockq;
  int16_t *bp, *pend, *ip, *qp;

  blockk = receiveReadOnly();
  if (channels == 1) {
    if (! get_nrpn(KYRP_ST_ENABLE)) {
      if (blockk) release(blockk);
      return;
    }
    blocki = allocate();
    blockq = NULL;
    if ( ! blocki) {
      if (blockk) release(blockk);
      return;
    }
  } else if (channels == 2) {
    if ( ! get_nrpn(KYRP_IQ_ENABLE)) {
      if (blockk) release(blockk);
      return;
    }
    blocki = allocate();
    blockq = allocate();
    if ( ! blocki || ! blockq) {
      if (blockk) release(blockk);
      if (blocki) release(blocki);
      if (blockq) release(blockq);
      return;
    }
  } else {
    /* throw an exception? */
    if (blockk) release(blockk);
    return;
  }
  if (blockk) bp = blockk->data;
  else bp = (int16_t *)zeros;
  pend = bp+AUDIO_BLOCK_SAMPLES;
  ip = blocki->data;
  if (channels == 2) qp = blockq->data;
  while (bp < pend) {
    int32_t key = *bp++;
    uint32_t phase_inc = phase_increment();
    if (position == 0) {
      if (key == 0) {
	// output is silent
	*ip++ = 0;
	if (channels == 2) *qp++ = 0;
	continue;
      }
      // ramp on
      position = 1;
      direction = 1;
      start_rise();
      /* fall through */
    } else if (position == 0xFFFFFFFF) {
      if (key != 0) {
	// output is 100%
	*ip++ = get_sine_value(phase_I) >> 17;
	phase_I += phase_inc;
	if (channels == 2) {
	  *qp++ = get_sine_value(phase_Q) >> 17;
	  phase_Q += phase_inc;
	}
	continue;
      }
      // ramp off
      position = 0xFFFFFFFE;
      direction = 0;
      start_fall();
      /* fall through */
    }
    int32_t magnitude = get_ramp_value(position);
#if defined(__ARM_ARCH_7EM__)
    *ip++ = multiply_32x32_rshift32(get_sine_value(phase_I), magnitude);
#elif defined(KINETISL)
    *ip++ = ((get_sine_value(phase_I) >> 16) * magnitude) >> 16;
#endif
    phase_I += phase_inc;
    if (channels == 2) {
#if defined(__ARM_ARCH_7EM__)
      *qp++ = multiply_32x32_rshift32(get_sine_value(phase_Q), magnitude);
#elif defined(KINETISL)
      *qp++ = ((get_sine_value(phase_Q) >> 16) * magnitude) >> 16;
#endif
      phase_Q += phase_inc;
    }
    if (direction > 0) {
      // output is increasing
      if (rate < 0xFFFFFFFF - position) {
	position += rate; // continue ramp
      } else {
	// end of rise
	position = 0xFFFFFFFF;
	end_rise();
      }
    } else {
      // output is decreasing
      if (rate < position) {
	position -= rate;
      } else {
	// end of fall
	position = 0;
	end_fall();
      }
    }
  }
  transmit(blocki, 0);
  release(blocki);
  if (channels == 2) {
    transmit(blockq, 1);
    release(blockq);
  }
  if (blockk) release(blockk);
}
