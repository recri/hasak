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
#include "output_sample.h"

void AudioOutputSample::update(void)
{
  audio_block_t *block;
  block = receiveReadOnly(0);
  if (block) {
    int16_t *bp = block->data, *end = bp+AUDIO_BLOCK_SAMPLES;
    wptr = (rptr+1)%(2*AUDIO_BLOCK_SAMPLES);
    while (bp < end) {
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
      buffer[wptr++] = *bp++; wptr %= 2*AUDIO_BLOCK_SAMPLES;
    }
    release(block);
  } else {
    wptr = (rptr+1)%(2*AUDIO_BLOCK_SAMPLES);
    for (int i = AUDIO_BLOCK_SAMPLES; --i >= 0; ) {
      buffer[wptr++] = 0; wptr %= 2*AUDIO_BLOCK_SAMPLES;
    }
  }
}
