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

#ifndef input_byte_h_
#define input_byte_h_
#include "../../config.h"
#include "Arduino.h"
#include "../../linkage.h"
#include "AudioStream.h"

#include "sample_value.h"
/*
** Teensy Audio Library component for sample input
*/


class AudioInputByte : public AudioStream
{
public:
  AudioInputByte() : AudioStream(0, NULL) {
    rptr = wptr = 0;
    last = bool2fix(0);
    debounce = 0;
  }
  void send(uint8_t value) { buffer[wptr++%(2*AUDIO_BLOCK_SAMPLES)] = value; }
  int16_t get(void) { return buffer[rptr++%(2*AUDIO_BLOCK_SAMPLES)]; }
  uint32_t tell(void) { return wptr; }
  void seek(uint32_t to_rptr) { rptr = to_rptr; }
  /* should be safe as only called from within update */
  audio_block_t *block_of_ones(void) {
    if ( ! ones) {
      ones = allocate();
      if (ones) {
	int16_t *bp, *endp;
	bp = ones->data;
	endp = bp+AUDIO_BLOCK_SAMPLES;
	while (bp < endp) *bp++ = bool2fix(1);
      }
    }
    return ones;
  }
  virtual void update(void);
private:
  static audio_block_t *ones;
  int16_t last;
  uint32_t rptr, wptr, debounce;
  uint8_t buffer[2*AUDIO_BLOCK_SAMPLES];
};

#endif
