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
** initial tune sidetone routing
** final sidetone, key out, and ptt out routing.
*/

static void cwroute_midi_tune_listener(int _, int __, int in_tune) {
  if (nrpn_get(NRPN_RKEY_ENABLE))
    note_set(NOTE_ST_TUNE, in_tune != 0);
}

static void cwroute_sidetone_listener(int _, int __, int key_st) {
  /* NOTE_KEY_ST can be routed to:
  ** NOTE_AU_ST_KEY to make a tone
  ** NOTE_MIDI_OUT_ST to make a midi note
  */
  if (nrpn_get(NRPN_ST_ENABLE)) {
    note_set(NOTE_AU_ST_KEY, key_st);
  // if (nrpn_get(NRPN_MIDI_KEY_ENABLE))
    note_set(NOTE_MIDI_OUT_ST, key_st ? VAL_EXT_NOTE_ON : VAL_EXT_NOTE_OFF);
  }
}

static void cwroute_key_out_listener(int _, int __, int key_out) {
  /* NOTE_KEY_OUT can be routed to
  ** NOTE_MIDI_OUT_KEY to produce a midi note
  ** NOTE_HW_KEY_OUT to change a pin state
  ** NOTE_HW_KEY_OUT2 to change another pin state
  ** NOTE_AU_IQ_KEY to key an IQ oscillator
  ** the last works best if you also route the IQ oscillator to usb_out and back to the host SDR.
  */
  if (nrpn_get(NRPN_TX_ENABLE)) {
    if (nrpn_get(NRPN_IQ_ENABLE))
      note_set(NOTE_AU_IQ_KEY, key_out);
    // if (nrpn_get(NRPN_MIDI_KEY_ENABLE))
    note_set(NOTE_MIDI_OUT_KEY, key_out ? VAL_EXT_NOTE_ON : VAL_EXT_NOTE_OFF);
    // if (nrpn_get(NRPN_HW_KEY_ENABLE))
    note_set(NOTE_HW_KEY_OUT, key_out);
    // if (nrpn_get(NRPN_HW_KEY2_ENABLE))
    note_set(NOTE_HW_KEY_OUT2, key_out);
  }
}

static void cwroute_ptt_out_listener(int _, int __, int ptt_out) {
  /* ptt out can be routed to
  ** NOTE_MIDI_OUT_PTT to generate an external midi note
  ** NOTE_HW_PTT_OUT and/or NOTE_HW_PTT_OUT2 to change a pin state
  */
  if (nrpn_get(NRPN_TX_ENABLE)) {
    // if (nrpn_get(NRPN_MIDI_PTT_ENABLE)) ?
    note_set(NOTE_MIDI_OUT_PTT, ptt_out ? VAL_EXT_NOTE_ON : VAL_EXT_NOTE_OFF);
    // if (nrpn_get(NRPN_HW_KEY_PTT_ENABLE)) ?
    note_set(NOTE_HW_PTT_OUT, ptt_out);
    // if (nrpn_get(NRPN_HW_KEY_PTT2_ENABLE)) ?
    note_set(NOTE_HW_PTT_OUT2, ptt_out);
  }
}

static void cwroute_setup(void) {
  note_listen(NOTE_MIDI_IN_TUNE, cwroute_midi_tune_listener);
  note_listen(NOTE_KEY_ST, cwroute_sidetone_listener);
  note_listen(NOTE_KEY_OUT, cwroute_key_out_listener);
  note_listen(NOTE_PTT_OUT, cwroute_ptt_out_listener);
}

//static void cwroute_loop(void) {}
