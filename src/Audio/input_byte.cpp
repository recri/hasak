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
#include "input_byte.h"

audio_block_t *AudioInputByte::ones = NULL;
void AudioInputByte::update(void)
{
  int32_t write_position = tell();
  /* 
  ** if we have debounce greater or equal to block length
  ** just send a block of last and be done with it.
  */
  if (debounce >= AUDIO_BLOCK_SAMPLES) {
    debounce -= AUDIO_BLOCK_SAMPLES;
    seek(write_position);
    if (last) transmit(block_of_ones());
    return;
  }
  /*
  ** debounce is less than block length, possibly zero.
  ** construct a block from the inputs.
  */
  audio_block_t *block = allocate();
  if (block) {
    uint16_t *bp, *endp;
    int32_t sum;
    bp = (uint16_t *)block->data;
    endp = bp+AUDIO_BLOCK_SAMPLES;
    sum = 0;
    seek(write_position-AUDIO_BLOCK_SAMPLES);
    while (bp < endp) {
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
      sum += *bp++ = bool2fix(get());
    }
    /* we are in the last partial block of debounce or free running */
    if (last == bool2fix(0) && sum == 0) {
      debounce = 0;
      release(block);
      /* transmit(block_of_zeros()) by transmitting nothing */
      return;
    }
    if (last == bool2fix(1) && sum == bool2fix(1)*AUDIO_BLOCK_SAMPLES) {
      debounce = 0;
      release(block);
      transmit(block_of_ones());
      return;
    }
    /* 
    ** the block is not pure last
    ** apply debounce and transition detection
    */
    bp = (uint16_t *)block->data;
    endp = bp+AUDIO_BLOCK_SAMPLES;
    sum = 0;
    while (bp < endp) {
      if (debounce > 0) {
	debounce -= 1;
	sum += *bp++ = last;
      } else if (*bp != last) {
	sum += last = *bp++;
	debounce = get_debounce();
      } else {
	sum += *bp++;
      }
    }
    if (sum != 0) transmit(block);
    release(block);
  }
}
