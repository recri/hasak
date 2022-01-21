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

/* global parameters for keyers */
// static inline int16_t get_iq_enable(void) { return get_nrpn(KYRP_IQ_ENABLE); }
// static inline int16_t get_iq_adjust(void) { return get_nrpn(KYRP_IQ_ADJUST); }
// static inline int16_t get_tx_enable(void) { return get_nrpn(KYRP_TX_ENABLE); }
// static inline int16_t get_st_enable(void) { return get_nrpn(KYRP_ST_ENABLE); }
// static inline int16_t get_ptt_enable(void) { return get_nrpn(KYRP_PTT_ENABLE); }

/* debounce interval and compensation - voice independent */
// static inline int16_t get_debounce(void) { return get_nrpn(KYRP_DEBOUNCE); }
// static inline int16_t get_comp(void) { return get_nrpn(KYRP_COMP); }

/* paddle modes - voice independent */
//static inline int16_t get_pad_mode(void) { return get_nrpn(KYRP_PAD_MODE); }
//static inline int16_t get_pad_swap(void) { return get_nrpn(KYRP_PAD_SWAP); }
//static inline int16_t get_pad_adapt(void) { return get_nrpn(KYRP_PAD_ADAPT); }
//static inline int16_t get_auto_ils(void) { return get_nrpn(KYRP_AUTO_ILS); }
//static inline int16_t get_auto_iws(void) { return get_nrpn(KYRP_AUTO_IWS); }
//static inline int16_t get_pad_keyer(void) { return get_nrpn(KYRP_PAD_KEYER); }

/* keyer ptt parameters - voice independent */
//static inline int16_t get_ptt_head(void) { return get_nrpn(KYRP_HEAD_TIME); }
//static inline int16_t get_ptt_tail(void) { return get_nrpn(KYRP_TAIL_TIME); }
//static inline int16_t get_ptt_hang(void) { return get_nrpn(KYRP_HANG_TIME); }

/* keyer ramp parameters - voice independent */
//static inline int16_t get_rise_time(void) { return get_nrpn(KYRP_RISE_TIME); }
//static inline int16_t get_rise_ramp(void) { return get_nrpn(KYRP_RISE_RAMP); }
//static inline int16_t get_fall_time(void) { return get_nrpn(KYRP_FALL_TIME); }
//static inline int16_t get_fall_ramp(void) { return get_nrpn(KYRP_FALL_RAMP); }

/* keyed tone parameters - voice dependent */
//static inline int16_t get_vox_tone(const int16_t vox) { return get_vox_nrpn(vox, KYRP_TONE); }
//static inline int16_t get_vox_level(const int16_t vox) { return get_vox_nrpn(vox, KYRP_LEVEL); }

/* raw key timing - voice dependent */
//static inline int16_t get_vox_speed(const int16_t vox) { return get_vox_nrpn(vox, KYRP_SPEED); }
//static inline int16_t get_vox_weight(const int16_t vox) { return get_vox_nrpn(vox, KYRP_WEIGHT); }
//static inline int16_t get_vox_ratio(const int16_t vox) { return get_vox_nrpn(vox, KYRP_RATIO); }
//static inline int16_t get_vox_farns(const int16_t vox) { return get_vox_nrpn(vox, KYRP_FARNS); }

/* keyer timing - voice dependent */
//static inline int16_t get_vox_dit(const int16_t vox) { return get_vox_nrpn(vox, KYRP_PER_DIT)*(AUDIO_SAMPLE_RATE/1000); }
//static inline int16_t get_vox_dah(const int16_t vox) { return get_vox_nrpn(vox, KYRP_PER_DAH)*(AUDIO_SAMPLE_RATE/1000); }
//static inline int16_t get_vox_ies(const int16_t vox) { return get_vox_nrpn(vox, KYRP_PER_IES)*(AUDIO_SAMPLE_RATE/1000); }
//static inline int16_t get_vox_ils(const int16_t vox) { return get_vox_nrpn(vox, KYRP_PER_ILS)*(AUDIO_SAMPLE_RATE/1000); }
//static inline int16_t get_vox_iws(const int16_t vox) { return get_vox_nrpn(vox, KYRP_PER_IWS)*(AUDIO_SAMPLE_RATE/1000); }

/* unit conversions */
static inline int ms_to_samples(int ms) { return ms * (AUDIO_SAMPLE_RATE*0.001); }
static inline float samples_to_ms(int samples) { return samples / (AUDIO_SAMPLE_RATE*0.001); }
static inline int tenth_ms_to_samples(int tenthms) { return tenthms * (AUDIO_SAMPLE_RATE*0.0001); }
static inline float samples_to_tenth_ms(int samples) { return samples / (AUDIO_SAMPLE_RATE*0.0001); }

/* translate 7 bit dbdown code to linear 7 bit integer multiplier */
static int16_t dbdown7bit(const int16_t level) {
  /* echo 'set dbd {}; for {set i 0} {$i < 128} {incr i} { lappend dbd [format %3d [expr {int(127*($i ? pow(10, ($i-127)/8.0/20.0) : 0.0))}]] }; puts [join $dbd ,]' | tclsh */
  int8_t table[128] = {
     0, 20, 21, 21, 21, 21, 22, 22, 22, 23, 23, 23, 24, 24, 24, 25, 25, 26, 26, 26, 27, 27, 28, 28, 28, 29, 29, 30, 30, 30, 31, 31,
    32, 32, 33, 33, 34, 34, 35, 35, 36, 36, 37, 37, 38, 39, 39, 40, 40, 41, 41, 42, 43, 43, 44, 45, 45, 46, 47, 47, 48, 49, 49, 50,
    51, 52, 52, 53, 54, 55, 55, 56, 57, 58, 59, 60, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 80,
    81, 82, 83, 84, 86, 87, 88, 89, 91, 92, 93, 95, 96, 98, 99,100,102,103,105,106,108,109,111,113, 114,116,118,119,121,123,125,127
  };
  return table[level&127];
}

/* translate 7 bit dbdown code to float level */
static float dbdown(const int16_t level) { return 0.007874f*dbdown7bit(level); }

#endif
