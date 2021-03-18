/* -*- mode: c++; tab-width: 8 -*- */
/*
 * hasak (ham and swiss army knife) keyer for Teensy 4.X, 3.X
 * Copyright (c) 2021 by Roger Critchlow, Charlestown, MA, USA
 * ad5dz, rec@elf.org
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
#include <Arduino.h>
#include "effect_button.h"
#include "utility/dspinst.h"

void AudioEffectButton::update(void)
{
  audio_block_t *block;

  block = receiveWritable(0);
  if (initial_skip > 0) {
      initial_skip -= 1;
      if (block) release(block);
      return;
  }
  if (block) {
    int16_t *bp, *endp;
    int32_t sum;
    bp = block->data;
    endp = bp+AUDIO_BLOCK_SAMPLES;
    sum = 0;
    while (bp < endp) {
      sum += *bp++;
      sum += *bp++;
      sum += *bp++;
      sum += *bp++;
      sum += *bp++;
      sum += *bp++;
      sum += *bp++;
      sum += *bp++;
    }
    int avg = sum / AUDIO_BLOCK_SAMPLES;
    int dist[N_BUTTONS], ibest = 0;    
    for (int i = 0; i < N_BUTTONS; i += 1) {
      dist[i] = abs(avg-centers[i]);
      if (dist[i] < dist[ibest])
	ibest = i;
    }
    if (last_best[0] == ibest && last_best[1] == ibest) {
      centers[ibest] = (centers[ibest]+avg)/2;
      if (ibest != 0) {
	bp = block->data;
	endp = bp+AUDIO_BLOCK_SAMPLES;
	while (bp < endp) {
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	  *bp++ = bool2fix(1);
	}
	transmit(block, ibest-1);
      }
    }      
    last_best[0] = last_best[1];
    last_best[1] = ibest;
    release(block);
  }
}
