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
#ifndef _responsive_h_
#define _responsive_h_

#include "Arduino.h"
#include "../../config.h"
#include "../../linkage.h"
#include "data_table.h"

/*
** Plain ramp.
** initialized with a ramp_nrpn and time_nrpn to define the slew shape and time.
** Started with slew(direction) which ramps up from 0 to max, or down from max to 0.
** value() returns the value of the ramp and advances to the next step.
** When not slewing, will simply return the current value of the ramp.
** is_slew() will return true while in the ramp.
*/
class PlainRamp {
public:
  const int16_t ramp_nrpn;	// get_nrpn(ramp_nrpn) -> ramp identifier
  const int16_t time_nrpn;	// get_nrpn(time_nrpn) -> ramp time as samples
  uint32_t position;		// 0 = off end of ramp, 0xFFFFFFFF = on end of ramp
  uint32_t rate;		// d position per sample
  int8_t direction;		// direction of change, 0 for none, +1 for up, -1 for down
  const int16_t *table;		// ramp table
  uint32_t last_value;
  PlainRamp(int16_t ramp, int16_t time) :
    ramp_nrpn(ramp), time_nrpn(time), direction(0), last_value(0) { 
  }

  bool in_slew(void) { return direction != 0; }
  
  void slew(int up) {
    if (in_slew()) {
      // already slewing, ignore request
    } else {
      rate = 0xFFFFFFFFu / (get_nrpn(time_nrpn)-1); // slew time in samples
      table = get_table(get_nrpn(ramp_nrpn));	// slew ramp table
      if (up) {					// slew up
	position = 0;
	direction = +1;
      } else {					// slew down
	position = 0xFFFFFFFF;
	direction = -1;
      }
    }
  }

  uint32_t ramp_value(void) {
    if (direction == 0)
      return last_value;
    // find the ramp value at position
    const uint32_t index = position >> 24;
    int32_t val1 = table[index];
    int32_t val2 = table[index+1];
    const uint32_t scale = (position >> 8) & 0xFFFF;
    val2 *= scale;
    val1 *= 0x10000 - scale;
    last_value = (val1 + val2) >> 16;
    // step to next position in ramp
    if (direction > 0) {
      // output is increasing
      if (rate < 0xFFFFFFFF - position) {
	position += rate; // continue ramp
      } else {
	// end of rise
	position = 0xFFFFFFFF;
	direction = 0;
      }
    } else {
      // output is decreasing
      if (rate < position) {
	position -= rate;
      } else {
	// end of fall
	position = 0;
	direction = 0;
      }
    }
    // return the value
    return last_value;
  }

  const int16_t *get_table(int16_t ramp) {
    switch (ramp) {
    default:
    case KYRV_RAMP_HANN: return hann_ramp;
    case KYRV_RAMP_BLACKMAN_HARRIS: return blackman_harris_ramp;
    case KYRV_RAMP_LINEAR: return linear_ramp;
      // case KYRV_RAMP_GAUSSIAN: return gaussian_ramp; 
    }
  }

};

/*
** Responsive ramp.
** initialized with a ramp_nrpn and time_nrpn to define the slew shape and time.
** Started with slew(target) which ramps from current to target.
** value() returns the value of the slew for the current step and advances to
** the next step.
** When not slewing, will simply return the current value of the ramp.
** Simplified to use the PlainRamp value as an interpolation parameter between
** current and target, the PlainRamp always runs from 0 to 0xffffffff
*/
class ResponsiveRamp {
public:
  const int16_t ramp_nrpn;	// get_nrpn(ramp_nrpn) -> ramp identifier
  const int16_t time_nrpn;	// get_nrpn(time_nrpn) -> ramp time as samples
  uint32_t current;		// current value of ramp
  uint32_t target;		// target value of ramp
  uint32_t position;		// 0 = off end of ramp, 0xFFFFFFFF = on end of ramp
  uint32_t rate;		// d position per sample
  int8_t direction;		// direction of change, 0 for none, +1 for up, -1 for down
  const int16_t *table;		// ramp table

  ResponsiveRamp(int16_t ramp, int16_t time, uint32_t current) :
    ramp_nrpn(ramp), time_nrpn(time), current(current), target(current) { 
  }

  bool in_slew(void) { return direction != 0; }
  
  void slew(uint32_t new_target) {
    if (direction != 0) {
      // already slewing, ignore request
    } else if (new_target == target) {
      // no change, ignore request
    } else {
      rate = 0xFFFFFFFFu / (get_nrpn(time_nrpn)-1); // ramp slew time in samples
      table = get_table(get_nrpn(ramp_nrpn));
      target = new_target;
      position = 0;
      direction = +1;
    }
  }

  uint32_t ramp_value(void) {
    if (direction == 0)
      return current;
    // find the ramp value at position
    const uint32_t index = position >> 24;
    int32_t val1 = table[index];
    int32_t val2 = table[index+1];
    const uint32_t scale = (position >> 8) & 0xFFFF;
    val2 *= scale;
    val1 *= 0x10000 - scale;
    const uint32_t ramp = (val1 + val2) >> 16;
    // interpolate current and target at the ramp value
    val2 = target;
    val2 *= ramp;
    val1 = current;
    val1 *= 0x8000 - ramp;
    const uint32_t value = (val1 + val2) >> 15;
    // step to next position in ramp
    // output is increasing
    if (rate < 0xFFFFFFFF - position) {
      position += rate; // continue ramp
    } else {
      // end of rise
      position = 0xFFFFFFFF;
      current = target;
      direction = 0;
    }
    // return the value
    return value;
  }

  const int16_t *get_table(int16_t ramp) {
    switch (ramp) {
    default:
    case KYRV_RAMP_HANN: return hann_ramp;
    case KYRV_RAMP_BLACKMAN_HARRIS: return blackman_harris_ramp;
    case KYRV_RAMP_LINEAR: return linear_ramp;
      // case KYRV_RAMP_GAUSSIAN: return gaussian_ramp; 
    }
  }

};

/*
** Responsive gain multiplier.
*/
class ResponsiveGainRamp : public ResponsiveRamp {
private:
  const int16_t level_nrpn;	// get_nrpn(level_nrpn) -> level as dB/10
  const int16_t enable_nrpn;	// get_nrpn(enable_nrpn) -> enable if 1, mute if 0
  int16_t current_level = 0;	// start at 0 dB
  int16_t current_enable = 0;	// start muted
public:
  uint32_t last_value;		// debugging
  ResponsiveGainRamp(int16_t _level_nrpn, int16_t _enable_nrpn, int16_t ramp_nrpn, int16_t time_nrpn) :
    ResponsiveRamp(ramp_nrpn, time_nrpn, 0), level_nrpn(_level_nrpn), enable_nrpn(_enable_nrpn) {
    update_target();
  }

  void update_target(void) {
    if ( ! in_slew()) {
      const int16_t target_level = signed_value(get_nrpn(level_nrpn));
      const int16_t target_enable = get_nrpn(enable_nrpn);
      if (target_level != current_level || target_enable != current_enable)  {
	current_level = target_level;
	current_enable = target_enable;
	if ( ! target_enable )
	  slew(0);
	else {
	  float gain = tenthdbtolinear(target_level);
	  if (gain > 32767.0f) gain = 32767.0f;
	  else if (gain < -32767.0f) gain = -32767.0f;
	  slew((uint32_t)(gain * 65536.0f)); // TODO: proper roundoff?
	}
      }
    }
  }
  
  uint32_t value(void) { return last_value = ramp_value(); }
};

/*
** Responsive frequency ramp.
*/
class ResponsiveFreqRamp : public ResponsiveRamp {
private:

  const int16_t freq_nrpn;	// get_nrpn(freq_nrpn) -> frequency as Hz/10
  int16_t current_freq = 0;

public:

  ResponsiveFreqRamp(int16_t freq, int16_t ramp_nrpn, int16_t time_nrpn) : 
    ResponsiveRamp(ramp_nrpn, time_nrpn, 0), freq_nrpn(freq) {
    update_target();
  }

  void update_target(void) {
    if ( ! in_slew()) {
      const int16_t target_freq = get_nrpn(freq_nrpn);
      if (target_freq != current_freq) {
	current_freq = target_freq;
	slew(target_freq);
      }
    }
  }

  // there's a way to avoid this computation, but later
  uint32_t value(void) {
    return ramp_value() * 0.1f * (4294967296.0f / AUDIO_SAMPLE_RATE_EXACT);
  }

};
  
#endif
