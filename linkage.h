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
#ifndef linkage_h_
#define linkage_h_

int16_t get_active_vox(void);

extern int16_t kyr_nrpn[KYRP_LAST];

/* fetch a nrpn */
static inline int16_t get_nrpn(const int16_t nrpn) { 
  return nrpn >= 0 && nrpn <= KYRP_LAST ? kyr_nrpn[nrpn] : -1;
}

/* fetch a vox specialized nrpn, not yet implemented */
static inline int16_t get_vox_nrpn(const int16_t vox, const int16_t nrpn) {
  const int16_t value = get_nrpn(KYRP_KEYER+vox*KYRP_VOX_OFFSET+(nrpn-KYRP_KEYER));
  return (value >= 0) ? value : get_nrpn(nrpn);
}

/***************************************************************
** Parameter fetching
** keyers, oscillators, ramps, and ptt fetch parameters as required
** which allows them to vary as you twiddle the ctrlr dials
***************************************************************/
/* button voltages */
static inline int16_t get_button(int button) { return (((int16_t)get_nrpn(KYRP_BUTTON_0+button))<<2)>>2; } // sign extended 14 bit value

/* unit conversions */
static inline int ms_to_samples(int ms) { return ms * (AUDIO_SAMPLE_RATE*0.001); }
static inline float samples_to_ms(int samples) { return samples / (AUDIO_SAMPLE_RATE*0.001); }
static inline int tenth_ms_to_samples(int tenthms) { return tenthms * (AUDIO_SAMPLE_RATE*0.0001); }
static inline float samples_to_tenth_ms(int samples) { return samples / (AUDIO_SAMPLE_RATE*0.0001); }
static inline float int_to_127ths(int val) { return val*0.007874f; }

#endif
