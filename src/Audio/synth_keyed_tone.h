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

extern "C" {
  extern const int16_t fader_table[257];	// raised cosine ramp from effect_fade
  extern const int16_t AudioWaveformSine[257];  // sine from synth_sine
};


class AudioSynthKeyedTone : public AudioStream
{
public:
  AudioSynthKeyedTone() : AudioStream(1, inputQueueArray), position(0) { }
  // fetch the current parameters for the rise ramp, fall ramp, and oscillator
  void start_rise() {
    uint8_t vox = get_active_vox();		// current vox in the arbiter
    int16_t rise_time = get_vox_rise_time(vox);
    int16_t rise_ramp = get_vox_rise_ramp(vox);
    int16_t fall_time = get_vox_fall_time(vox);
    int16_t fall_ramp = get_vox_fall_ramp(vox);
    rise_rate = 0xFFFFFFFFu / (rise_time * 1e-4 * AUDIO_SAMPLE_RATE);
    fall_rate = 0xFFFFFFFFu / (fall_time * 1e-4 * AUDIO_SAMPLE_RATE);
    rise_table = get_table(rise_ramp);
    fall_table = get_table(fall_ramp);
    rate = rise_rate;
    table = rise_table;
    int16_t tone = get_vox_tone(vox);
    iq_enable = get_iq_enable();
    // int16_t iq_adjust = get_iq_adjust();
    phase_increment = tone * (4294967296.0 / AUDIO_SAMPLE_RATE_EXACT);
    phase_I = 90.0 * (4294967296.0 / 360.0);
    // don't know what the units of iqph are, yet, 
    // needs to be positive or negative offset from exact 90 phase
    // not sure that USB and LSB aren't actually the other way around
    // there should be a level balance here, too.
    switch (iq_enable) {
    default:
    case KYRP_IQ_NONE: 
    case KYRP_IQ_USB: phase_Q = 0.0 * (4294967296.0 / 360.0); break;
    case KYRP_IQ_LSB: phase_Q = 180.0 * (4294967296.0 / 360.0); break;
    }
    /* Serial.printf("start_rise vox %d, time %d, rate %d, ramp %d", 
       vox, time, rate, ramp);
       Serial.printf(" tone %d, iq_enable %d, phase_increment %d\n",
       tone, iq_enable, phase_increment); */
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
    case KYRP_RAMP_HANN: return fader_table;
    case KYRP_RAMP_BLACKMAN_HARRIS: return fader_table;
    case KYRP_RAMP_LINEAR: return fader_table;
    case KYRP_RAMP_SLINEAR: return fader_table;
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
  // get the interpolated sine value at phi
  int32_t get_sine_value(uint32_t phase) {
    uint32_t index, scale;
    int32_t val1, val2;
    index = phase >> 24;
    val1 = AudioWaveformSine[index];
    val2 = AudioWaveformSine[index+1];
    scale = (phase >> 8) & 0xFFFF;
    val2 *= scale;
    val1 *= 0x10000 - scale;
    return val1+val2;
  }
  virtual void update(void);
private:
  uint32_t position; // 0 = off, 0xFFFFFFFF = on
  uint32_t rise_rate, fall_rate, rate;
  uint8_t direction;
  const int16_t *rise_table, *fall_table, *table;
  uint8_t iq_enable;
  uint32_t phase_I, phase_Q, phase_increment;
  audio_block_t *inputQueueArray[1];
};

#endif
