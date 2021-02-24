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

#include <Arduino.h>
#include "effect_merge.h"

void AudioEffectMerge::update(void)
{
  audio_block_t *blocka, *blockb;
  uint32_t *pa, *pb, *end, v;

  blocka = receiveWritable(0);
  blockb = receiveReadOnly(1);
  if (!blocka) {
    if (blockb) release(blockb);
    return;
  }
  if (!blockb) {
    release(blocka);
    return;
  }
  pa = (uint32_t *)(blocka->data);
  pb = (uint32_t *)(blockb->data);
  end = pa + AUDIO_BLOCK_SAMPLES/2;
  // presuming that the inputs consist of 1<<15 and 0<<15
  while (pa < end) {
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
#if AUDIO_BLOCK_SAMPLES > 4
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
#if AUDIO_BLOCK_SAMPLES > 8
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
#if AUDIO_BLOCK_SAMPLES > 16
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
    v = *pa | *pb++; *pa++ = v;
#endif
#endif
#endif
  }
  transmit(blocka);
  release(blocka);
  release(blockb);
}

