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

#ifndef rmixer_h_
#define rmixer_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "../../config.h"
#include "../../linkage.h"
#include "responsive.h"

class RAudioMixer4 : public AudioStream
{
public:
  const int level_nrpn;
  const int enable_nrpn;
  RAudioMixer4(int16_t level, int16_t enable, int16_t ramp_nrpn, int16_t time_nrpn) :
    AudioStream(4, inputQueueArray), level_nrpn(level), enable_nrpn(enable) {
    for (int i=0; i<4; i++) ramp[i] = new ResponsiveGainRamp(level_nrpn+i, enable_nrpn+i, ramp_nrpn, time_nrpn);
  }
  virtual void update(void);
  uint32_t last_value(int i) { if (i >= 0 && i < 4) return ramp[i]->last_value; return 0xffffffffu; }
private:
  ResponsiveGainRamp *ramp[4];
  audio_block_t *inputQueueArray[4];
};

class RAudioAmplifier : public AudioStream
{
public:
  RAudioAmplifier(int16_t level, int16_t enable, int16_t ramp_nrpn, int16_t time_nrpn) :
    AudioStream(1, inputQueueArray) {
    ramp = new ResponsiveGainRamp(level, enable, ramp_nrpn, time_nrpn);
  }
  virtual void update(void);
private:
  ResponsiveGainRamp *ramp;
  audio_block_t *inputQueueArray[1];
};

#endif
