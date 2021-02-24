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
   effect_fade.h and effect_fade.cpp
   Take a stream of 0.0 and 1.0 on input and construct smooth ramps from
   0.0 to 1.0 or 1.0 to 0.0 when the stream changes value. This ramp
   stream can be multiplied with other streams to smoothly transition
   them on or off.
   When a transition between 0.0 and 1.0 is detected, the input samples
   are ignored until the ramp runs to completion.  This provides a kind
   of debouncing on the input stream.
*/
#ifndef effect_ramp_h_
#define effect_ramp_h_
#include <Arduino.h>
#include <AudioStream.h>
#include "sample_value.h"

class AudioEffectRamp : public AudioStream
{
public:
  AudioEffectRamp() : AudioStream(1, inputQueueArray), position(0xFFFFFFFF), updated(false) {
    set_ramp_on(5000);
    set_ramp_off(5000);
  }
  void set_ramp_on(uint32_t microseconds) {
    uint32_t rate = 0xFFFFFFFFu / (microseconds * AUDIO_SAMPLE_RATE * 1e-6);
    if (rate != ramp_on_rate) {
      ramp_on_rate = rate;
      updated = true;
    }
  }
  void set_ramp_off(uint32_t microseconds) {
    uint32_t rate = 0xFFFFFFFFu / (microseconds * AUDIO_SAMPLE_RATE * 1e-6);
    if (rate != ramp_off_rate) {
      ramp_off_rate = rate;
      updated = true;
    }
  }
  virtual void update(void);
private:
  uint32_t position; // 0 = off, 0xFFFFFFFF = on
  uint32_t ramp_on_rate, ramp_off_rate;
  uint8_t direction; // 0 = fading out, 1 = fading in
  bool updated;
  audio_block_t *inputQueueArray[1];
};

#endif
