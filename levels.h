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

/*
** mixer related functions.
*/

/*
** expand mixer shorthand bits to individual mixer channel nrpns
** nrpn == NRPN_MIX_ENABLE then set left and right according to bits
** nrpn == NRPN_MIX_ENABLE_L then set left according to bits
** nrpn == NRPN_MIX_ENABLE_R then set right according to bits
** in each case the mask has 12 bits and ranges from 0 to 4095
** in each case we have 4 bits for usb, i2s, and hdw out mixers
** the order of the mixers and enables is:
** usb: L0 L1 L2 L3 R0 R1 R2 R3
** i2s: L0 L1 L2 L3 R0 R1 R2 R3
** hdw: L0 L1 L2 L3 R0 R1 R2 R3
** so, bit i, counting from 0 at the msb end of the value, 
** addresses left mixer channel (i%4)+(i/4)*8
** and addresses right mixer channel (i%4)+(i/4)*8+4
*/
static void levels_recompute_mixer_enables(int nrpn, int _) {
  const int value = nrpn_get(nrpn);
  for (int i = 0; i < 12; i += 1) {
    const int l = (i%4)+(i/4)*8;
    const int r = l+4;
    const int v = (value & (1<<(11-i))) != 0;
    if (nrpn == NRPN_MIX_ENABLE || nrpn == NRPN_MIX_ENABLE_L) nrpn_set(NRPN_MIXER2+l, v);
    if (nrpn == NRPN_MIX_ENABLE || nrpn == NRPN_MIX_ENABLE_R) nrpn_set(NRPN_MIXER2+r, v);
  }
}

/* compute mixer targets from levels and balance controls */
static void levels_recompute_mixer_levels(int nrpn, int _) {
  for (int i = 0; i < 24; i += 1) {    
    const int channel = i/8;	/* channel = 0 for usb_out, 1 for i2s_out, 2 for hdw_out */
    const int input = i%4;	/* input = 0 for i2s_in/usb_in, 1 for sidetone, 2 for keyed IQ, 3 for hdw_in */
    const int left = (i%8) < 4;	/* left versus right channel */
    int level = nrpn_get(NRPN_USB_LEVEL+channel); /* base output level for this channel */
    if (input == 0) {				  /* rx audio from usb_in */
      if (channel == 1 && nrpn_get(NRPN_RX_PTT_LEVEL) != 0 && note_get(NOTE_PTT_ST) != 0) {
	level += nrpn_get(NRPN_RX_PTT_LEVEL);	  /* mute rx audio when sidetone active */
      }
    } else if (input == 1) {			  /* sidetone */
      level += nrpn_get(NRPN_LEVEL);		  /* adjust sidetone level */
      const int balance = signed_value(nrpn_get(NRPN_ST_BALANCE));
      if (balance != 0) {
	if (left != 0 && balance > 0) {		  /* pan by attenuating left channel */
	  level -= balance;
	} else if (left == 0 && balance < 0) {	  /* pan by attenuating right channel */
	  level += balance;
	}
      }
    }
    if (level != nrpn_get(NRPN_MIX_USB_L0+i))
      nrpn_set(NRPN_MIX_USB_L0+i, level);
  }
}

static void levels_note_ptt_st_listener(int note, int _) {
  if (nrpn_get(NRPN_RX_PTT_LEVEL) != 0)
    levels_recompute_mixer_levels(-1, -1);
}

static void levels_setup(void) {
  /* mixer enable update, listen for changes */
  nrpn_listen(NRPN_MIX_ENABLE, levels_recompute_mixer_enables);
  nrpn_listen(NRPN_MIX_ENABLE_L, levels_recompute_mixer_enables);
  nrpn_listen(NRPN_MIX_ENABLE_L, levels_recompute_mixer_enables);

  /* mixer level update, listen for changes */
  note_listen(NOTE_PTT_ST, levels_note_ptt_st_listener);
  nrpn_listen(NRPN_VOLUME, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_LEVEL, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_USB_LEVEL, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_I2S_LEVEL, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_HDW_LEVEL, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_ST_BALANCE, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_RX_PTT_LEVEL, levels_recompute_mixer_levels);
  nrpn_listen(NRPN_CODEC_VOLUME, levels_recompute_mixer_levels);

}
