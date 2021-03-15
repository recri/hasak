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

int get_active_vox(void);

extern int16_t kyr_nrpn[];

/* fetch a nrpn */
static inline int get_nrpn(int nrpn) { 
  return nrpn >= 0 && nrpn <= KYRP_LAST ? kyr_nrpn[nrpn] : -1;
}
/* fetch a vox specialized nrpn, not yet implemented */
static inline int get_vox_nrpn(int vox, int nrpn) {
  int value = get_nrpn(KYRP_KEYER+vox*KYRP_VOX_OFFSET+(nrpn-KYRP_KEYER));
  return (value >= 0) ? value : get_nrpn(nrpn);
}
/***************************************************************
** Parameter fetching
** keyers, oscillators, ramps, and ptt fetch parameters as required
** which allows them to vary as you twiddle the ctrlr dials
***************************************************************/
/* paddle modes */
static inline int get_vox_pad_mode(int vox) { return get_vox_nrpn(vox, KYRP_PAD_MODE); }
static inline int get_vox_pad_swap(int vox) { return get_vox_nrpn(vox, KYRP_PAD_SWAP); }
static inline int get_vox_pad_adapt(int vox) { return get_vox_nrpn(vox, KYRP_PAD_ADAPT); }
static inline int get_vox_auto_ils(int vox) { return get_vox_nrpn(vox, KYRP_AUTO_ILS); }
static inline int get_vox_auto_iws(int vox) { return get_vox_nrpn(vox, KYRP_AUTO_IWS); }
static inline int get_vox_pad_keyer(int vox) { return get_vox_nrpn(vox, KYRP_PAD_KEYER); }

/* keyer timing */
static inline int get_vox_dit(int vox) { return get_vox_nrpn(vox, KYRP_PER_DIT); }
static inline int get_vox_dah(int vox) { return get_vox_nrpn(vox, KYRP_PER_DAH); }
static inline int get_vox_ies(int vox) { return get_vox_nrpn(vox, KYRP_PER_IES); }
static inline int get_vox_ils(int vox) { return get_vox_nrpn(vox, KYRP_PER_ILS); }
static inline int get_vox_iws(int vox) { return get_vox_nrpn(vox, KYRP_PER_IWS); }

/* raw key timing */
static inline int get_vox_speed(int vox) { return get_vox_nrpn(vox, KYRP_SPEED); }
static inline int get_vox_weight(int vox) { return get_vox_nrpn(vox, KYRP_WEIGHT); }
static inline int get_vox_ratio(int vox) { return get_vox_nrpn(vox, KYRP_RATIO); }
static inline int get_vox_comp(int vox) { return get_vox_nrpn(vox, KYRP_COMP); }
static inline int get_vox_farns(int vox) { return get_vox_nrpn(vox, KYRP_FARNS); }

/* keyed tone parameters */
static inline int get_vox_tone(int vox) { return get_vox_nrpn(vox, KYRP_TONE); }
static inline int get_vox_rise_time(int vox) { return get_vox_nrpn(vox, KYRP_RISE_TIME); }
static inline int get_vox_rise_ramp(int vox) { return get_vox_nrpn(vox, KYRP_RISE_RAMP); }
static inline int get_vox_fall_time(int vox) { return get_vox_nrpn(vox, KYRP_FALL_TIME); }
static inline int get_vox_fall_ramp(int vox) { return get_vox_nrpn(vox, KYRP_FALL_RAMP); }
static inline int time_to_samples(int time) { return ((int)AUDIO_SAMPLE_RATE) * time / 10000; }

/* keyer ptt parameters */
static inline int get_vox_ptt_head(int vox) { return get_vox_nrpn(vox, KYRP_HEAD_TIME); }
static inline int get_vox_ptt_tail(int vox) { return get_vox_nrpn(vox, KYRP_TAIL_TIME); }

/* global parameters for keyers */
static inline int get_iq_enable(void) { return get_nrpn(KYRP_IQ_ENABLE); }
static inline int get_iq_adjust(void) { return get_nrpn(KYRP_IQ_ADJUST); }
static inline int get_tx_enable(void) { return get_nrpn(KYRP_TX_ENABLE); }
static inline int get_st_enable(void) { return get_nrpn(KYRP_ST_ENABLE); }
static inline int get_send_midi(void) { return get_nrpn(KYRP_SEND_MIDI); }
static inline int get_recv_midi(void) { return get_nrpn(KYRP_RECV_MIDI); }
#endif
