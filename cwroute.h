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

static void cwroute_midi_tune_listener(int note) {
  if (nrpn_get(KYRP_RKEY_ENABLE))
    note_set(KYRN_ST_TUNE, note_get(KYRN_MIDI_IN_TUNE) != 0);
}

static void cwroute_sidetone_listener(int note) {
  /* KYRN_KEY_ST can be routed to:
  ** KYRN_AU_ST_KEY to make a tone
  ** KYRN_MIDI_OUT_ST to make a midi note
  */
  if (nrpn_get(KYRP_ST_ENABLE)) {
    note_set(KYRN_AU_ST_KEY, note_get(KYRN_KEY_ST));
  // if (nrpn_get(NRPN_MIDI_KEY_ENABLE))
    note_set(KYRN_MIDI_OUT_ST, note_get(KYRN_KEY_ST) ? KYRV_EXT_NOTE_ON : KYRV_EXT_NOTE_OFF);
  }
}

static void cwroute_key_out_listener(int note) {
  /* KYRN_KEY_OUT can be routed to
  ** KYRN_MIDI_OUT_KEY to produce a midi note
  ** KYRN_HW_KEY_OUT to change a pin state
  ** KYRN_HW_KEY_OUT2 to change another pin state
  ** KYRN_AU_IQ_KEY to key an IQ oscillator
  ** the last works best if you also route the IQ oscillator to usb_out and back to the host SDR.
  */
  if (nrpn_get(KYRP_TX_ENABLE)) {
    if (nrpn_get(KYRP_IQ_ENABLE))
      note_set(KYRN_AU_IQ_KEY, note_get(KYRN_KEY_OUT));
    // if (nrpn_get(KYRP_MIDI_KEY_ENABLE))
    note_set(KYRN_MIDI_OUT_KEY, note_get(KYRN_KEY_OUT) ? KYRV_EXT_NOTE_ON : KYRV_EXT_NOTE_OFF);
    // if (nrpn_get(KYRP_HW_KEY_ENABLE))
    note_set(KYRN_HW_KEY_OUT, note_get(KYRN_KEY_OUT));
    // if (nrpn_get(KYRP_HW_KEY2_ENABLE))
    note_set(KYRN_HW_KEY_OUT2, note_get(KYRN_KEY_OUT));
  }
}

static void cwroute_ptt_out_listener(int note) {
  /* ptt out can be routed to
  ** KYRN_MIDI_OUT_PTT to generate an external midi note
  ** KYRN_HW_PTT_OUT and/or KYRN_HW_PTT_OUT2 to change a pin state
  */
  if (nrpn_get(KYRP_TX_ENABLE)) {
    // if (nrpn_get(KYRP_MIDI_PTT_ENABLE)) ?
    note_set(KYRN_MIDI_OUT_PTT, note_get(KYRN_PTT_OUT) ? KYRV_EXT_NOTE_ON : KYRV_EXT_NOTE_OFF);
    // if (nrpn_get(KYRP_HW_KEY_PTT_ENABLE)) ?
    note_set(KYRN_HW_PTT_OUT, note_get(KYRN_PTT_OUT));
    // if (nrpn_get(KYRP_HW_KEY_PTT2_ENABLE)) ?
    note_set(KYRN_HW_PTT_OUT2, note_get(KYRN_PTT_OUT));
  }
}

static void cwroute_setup(void) {
  note_listen(KYRN_MIDI_IN_TUNE, cwroute_midi_tune_listener);
  note_listen(KYRN_KEY_ST, cwroute_sidetone_listener);
  note_listen(KYRN_KEY_OUT, cwroute_key_out_listener);
  note_listen(KYRN_PTT_OUT, cwroute_ptt_out_listener);
}

static void cwroute_loop(void) {
}
