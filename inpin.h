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
 * digital pin input
 */

void inpin_setup(void) {
  pinMode(KYR_L_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_R_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_S_KEY_PIN, INPUT_PULLUP);
  pinMode(KYR_EXT_PTT_PIN, INPUT_PULLUP);
}


void inpin_loop(void) {
  static elapsedMicros inpin_l_pad_debounce;
  static elapsedMicros inpin_r_pad_debounce;
  static elapsedMicros inpin_s_key_debounce;
  static elapsedMicros inpin_ext_ptt_debounce;
  // ah, inverted logic, pins are active low, notes are active high
  // debounce by ignoring changes for a period of time
  if (inpin_l_pad_debounce > (unsigned)get_nrpn(KYRP_DEBOUNCE) &&
      digitalReadFast(KYR_L_PAD_PIN) == hasak.notes[KYRN_L_PAD]) {
    note_toggle(KYRN_L_PAD);
    inpin_l_pad_debounce = 0;
  }
  if (inpin_r_pad_debounce > (unsigned)get_nrpn(KYRP_DEBOUNCE) &&
      digitalReadFast(KYR_R_PAD_PIN) == hasak.notes[KYRN_R_PAD]) {
    note_toggle(KYRN_R_PAD);
    inpin_r_pad_debounce = 0;
  }
  if (inpin_s_key_debounce > (unsigned)get_nrpn(KYRP_DEBOUNCE) &&
      digitalReadFast(KYR_S_KEY_PIN) == hasak.notes[KYRN_S_KEY]) {
    note_toggle(KYRN_S_KEY);
    inpin_s_key_debounce = 0;
  }
  if (inpin_ext_ptt_debounce > (unsigned)get_nrpn(KYRP_DEBOUNCE) && 
      digitalReadFast(KYR_EXT_PTT_PIN) == hasak.notes[KYRN_EXT_PTT]) {
    note_toggle(KYRN_EXT_PTT);
    inpin_ext_ptt_debounce = 0;
  }
}
