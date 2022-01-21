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
/* 
   keyed tone derived from Audio Library for Teensy 3.X
   effect_fade.* mashed up with synth_sine.*
*/
#ifndef synth_keyed_tone_h_
#define synth_keyed_tone_h_

#include "../../config.h"
#include <Arduino.h>
#include <AudioStream.h>
#include "sample_value.h"
#include "../../linkage.h"
#include "data_table.h"

class AudioSynthKeyedTone : public AudioStream
{
public:
  AudioSynthKeyedTone(int channels=1) : AudioStream(1, inputQueueArray), channels(channels) {
    position = 0;
  }
  // fetch the current parameters for the rise ramp, and fall ramp
  // oscillatar may vary
  void start_rise() {
    vox = get_active_vox();		// current vox, vox is consistent across in the arbiter
    rate = 0xFFFFFFFFu / tenth_ms_to_samples(get_nrpn(KYRP_RISE_TIME));
    fall_rate = 0xFFFFFFFFu / tenth_ms_to_samples(get_nrpn(KYRP_FALL_TIME));
    table = get_table(get_nrpn(KYRP_RISE_RAMP));
    fall_table = get_table(get_nrpn(KYRP_FALL_RAMP));
    // int16_t iq_adjust = get_nrpn(KYRP_IQ_ADJUST);
    // phase_increment = get_vox_nrpn(vox, KYRP_TONE) * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
    // don't know what the units of iqph are, yet, 
    // needs to be positive or negative offset from exact 90 phase.
    // have 14bits, so 13bits and sign, thats +/-8192
    // not sure that USB and LSB aren't actually the other way around
    // there should be a level balance here, too.
    // IQ might need its own frequency, too.
    // I may have USB and LSB swapped
    switch (get_nrpn(KYRP_IQ_ENABLE)) {
    default:
    case KYRV_IQ_NONE: 
    case KYRV_IQ_USB: 
      phase_I = +45.0 * (4294967296.0 / 360.0);
      phase_Q = (360-45.0) * (4294967296.0 / 360.0);
      break;
    case KYRV_IQ_LSB: 
      phase_I = (360-45.0) * (4294967296.0 / 360.0);
      phase_Q = +45.0 * (4294967296.0 / 360.0); 
      break;
    }
  }
  // fetch the current parameters for the fall ramp
  void start_fall() {
    rate = fall_rate;
    table = fall_table;
  }
  // end of rise ramp
  void end_rise(void) {
    // Serial.printf("end_rise position %d\n", position);
  }
  // end of fall ramp
  void end_fall(void) {
    // Serial.printf("end_fall position %d\n", position);
  }
  // get the precalculated table for the specified ramp
  // yes, they're all the same table right now, fix.me
  const int16_t *get_table(int16_t ramp) {
    switch (ramp) {
    default:
    case KYRV_RAMP_HANN: return hann_ramp;
    case KYRV_RAMP_BLACKMAN_HARRIS: return blackman_harris_ramp;
    case KYRV_RAMP_LINEAR: return linear_ramp;
    }
  }
  // get the interpolated ramp value at pos
  int32_t get_ramp_value(uint32_t pos) {
    uint32_t index = pos >> 24;
    int32_t val1 = table[index];
    int32_t val2 = table[index+1];
    uint32_t scale = (pos >> 8) & 0xFFFF;
    val2 *= scale;
    val1 *= 0x10000 - scale;
    return (val1 + val2) >> 16;
  }
  // get the interpolated sine value at phase
  int32_t get_sine_value(uint32_t phase) {
    uint32_t index, scale;
    int32_t val1, val2;
    index = phase >> 24;
    val1 = sine_table[index];
    val2 = sine_table[index+1];
    scale = (phase >> 8) & 0xFFFF;
    val2 *= scale;
    val1 *= 0x10000 - scale;
    return dbdown7bit(get_vox_nrpn(vox, KYRP_LEVEL))*((val1+val2)>>7); // 7 bit level applied to 31 bit fractions
    // return val1+val2;
  }
  uint32_t phase_increment(void) {
    return get_vox_nrpn(vox, KYRP_TONE) * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
  }
  virtual void update(void);
private:
  const uint8_t channels;
  uint8_t vox;
  uint32_t position; // 0 = off, 0xFFFFFFFF = on
  uint32_t fall_rate, rate;
  uint8_t direction;
  const int16_t *fall_table, *table;
  uint32_t phase_I, phase_Q;
  // uint31_t phase_increment; converted to method to allow live tuning
  audio_block_t *inputQueueArray[1];
};

#endif
