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

#ifndef effect_arbiter_h_
#define effect_arbiter_h_
#include "Arduino.h"
#include "AudioStream.h"

class AudioEffectArbiter : public AudioStream
{
#define NVOX 4
public:
 AudioEffectArbiter() : AudioStream(NVOX, inputQueueArray), active_vox(0) { }
  virtual void update(void);
  void define_vox(int index, int _vox, int _priority) {
    if (index >= 0 && index < NVOX) {
      vox[index] = _vox; priority[index] = _priority;
    }
  }
  uint8_t get_active_vox(void) { return active_vox; }
private:
  uint8_t active_vox;
  uint16_t active_tail, ptt_tail;
  uint8_t vox[NVOX], priority[NVOX];
  audio_block_t *inputQueueArray[NVOX];
};

#endif
