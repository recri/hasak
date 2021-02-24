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
   effect_fade.h and effect_fade.cpp rather than 
*/
#include <Arduino.h>
#include "effect_ramp.h"
#include "utility/dspinst.h"

extern "C" {
extern const int16_t fader_table[257];
};

void AudioEffectRamp::update(void)
{
  audio_block_t *iblock, *oblock;
  uint32_t i, pos, inc, index, scale;
  int32_t val1, val2, val, sample;
  uint8_t dir;

  pos = position;
  iblock = receiveReadOnly(0);
  if (pos == 0) {
    // output is silent
    if ( ! iblock)
      return;
    if (allzeros(iblock)) {
      release(iblock);
      return;
    }
    // scan iblock from 0 for the transition
  } else if (pos == 0xFFFFFFFF) {
    // output is 100%
    if (iblock && allones(iblock)) {
      transmit(iblock);
      release(iblock);
    }
    // scan iblock from 0 for the transition
  }
  // scan iblock for transition / implement ramp
  // if we can't allocate output, we should run
  // the ramp steps that would have been made
  oblock = allocate();
  if ( ! oblock) {
    if (iblock) release(iblock);
    return;
  }
  // initialize 
  dir = direction; // 0 fade out, 1 fade in
  inc = dir == 0 ? ramp_off_rate : ramp_on_rate;
  for (i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
    val = iblock->data[i];
    if (pos == 0) {
      if (val != 0) {
	// start ramp on
	pos = 1;
	dir = 1;
	inc = ramp_on_rate;
      } else {
	// continue scan
      }
    } else if (pos == 0xFFFFFFFF) {
      if (val == 0) {
	// start ramp off
	pos = 0xFFFFFFFE;
	dir = 0;
	inc = ramp_off_rate;
      } else {
	// continue scan
      }
    } else {
      // implement ramp on/off
      // compute factor
      index = pos >> 24;
      val1 = fader_table[index];
      val2 = fader_table[index+1];
      scale = (pos >> 8) & 0xFFFF;
      val2 *= scale;
      val1 *= 0x10000 - scale;
      val = (val1 + val2) >> 16;
      // advance position
      if (dir > 0) {
	// output is increasing
	if (inc < 0xFFFFFFFF - pos) pos += inc;
	else {
	  pos = 0xFFFFFFFF;
	  // end of ramp_on
	  // scan block from i for a transtion
	  break;
	}
      } else {
	// output is decreasing
	if (inc < pos) pos -= inc;
	else {
	  pos = 0;
	  // end of ramp_off
	  // scan block from i for a transtion
	  break;
	}
      }
    }
    oblock->data[i] = val;
  }
  position = pos;
  direction = dir;
  transmit(oblock);
  release(oblock);
  release(iblock);
}
