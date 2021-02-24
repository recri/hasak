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

#ifndef input_sample_h_
#define input_sample_h_
#include "Arduino.h"
#include "AudioStream.h"

/*
** Teensy Audio Library component for sample input
** samples are stored into successive slots in a sample buffer
** empty slots at update are filled with the last sample received
*/

class AudioInputSample : public AudioStream
{
public:
  AudioInputSample() : AudioStream(0, NULL) {
    block = allocate();
    if (block) {
      wptr = &block->data[0];
      n = AUDIO_BLOCK_SAMPLES;
      nzero = 0;
    }
    last = 0;
  }
  void send(int16_t value) {
    last = value;
    if (block && n > 0) {
      *wptr++ = last;
      n -= 1;
      nzero += last == 0 ? 1 : 0;
    } else
      overrun += 1;
  }
  uint16_t overruns() { return overrun; }
  uint16_t underruns() { return underrun; }
  void overrunsReset() { overrun = 0; }
  void underrunsReset() { underrun = 0; }
  virtual void update(void);
private:
  audio_block_t *block;
  int16_t *wptr, n, last, nzero;
  uint16_t overrun, underrun;
};

#endif
