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

static void pin_sample(void) {
  // ah, inverted logic, pins are active low, notes are active high
  // debounce by ignoring changes for a period of time

  /* first four input pins are hard wired */
  static elapsedSamples pin_l_pad_debounce;
  if ((int)pin_l_pad_debounce >= 0 &&
      digitalReadFast(KYR_L_PAD_PIN) == note_get(KYRN_HW_L_PAD)) {
    note_toggle(KYRN_HW_L_PAD);
    pin_l_pad_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
  }
  static elapsedSamples pin_r_pad_debounce;
  if ((int)pin_r_pad_debounce >= 0 &&
      digitalReadFast(KYR_R_PAD_PIN) == note_get(KYRN_HW_R_PAD)) {
    note_toggle(KYRN_HW_R_PAD);
    pin_r_pad_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
  }
  static elapsedSamples pin_s_key_debounce;
  if ((int)pin_s_key_debounce >= 0 &&
      digitalReadFast(KYR_S_KEY_PIN) == note_get(KYRN_HW_S_KEY)) {
    note_toggle(KYRN_HW_S_KEY);
    pin_s_key_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
  }
  static elapsedSamples pin_ext_ptt_debounce;
  if ((int)pin_ext_ptt_debounce >= 0 && 
      digitalReadFast(KYR_EXT_PTT_PIN) == note_get(KYRN_HW_EXT_PTT)) {
    note_toggle(KYRN_HW_EXT_PTT);
    pin_ext_ptt_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
  }

  /* eight additional pins which can be wired up */
  if (nrpn_get(KYRP_PIN0_PIN) >= 0 && nrpn_get(KYRP_PIN0_NOTE) > 0) {
    static elapsedSamples pin_0_debounce;
    if ((int)pin_0_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN0_PIN)) == note_get(nrpn_get(KYRP_PIN0_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN0_NOTE));
      pin_0_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN1_PIN) >= 0 && nrpn_get(KYRP_PIN1_NOTE) > 0) {
    static elapsedSamples pin_1_debounce;
    if ((int)pin_1_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN1_PIN)) == note_get(nrpn_get(KYRP_PIN1_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN1_NOTE));
      pin_1_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN2_PIN) >= 0 && nrpn_get(KYRP_PIN2_NOTE) > 0) {
    static elapsedSamples pin_2_debounce;
    if ((int)pin_2_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN2_PIN)) == note_get(nrpn_get(KYRP_PIN2_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN2_NOTE));
      pin_2_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN3_PIN) >= 0 && nrpn_get(KYRP_PIN3_NOTE) > 0) {
    static elapsedSamples pin_3_debounce;
    if ((int)pin_3_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN3_PIN)) == note_get(nrpn_get(KYRP_PIN3_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN3_NOTE));
      pin_3_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN4_PIN) >= 0 && nrpn_get(KYRP_PIN4_NOTE) > 0) {
    static elapsedSamples pin_4_debounce;
    if ((int)pin_4_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN4_PIN)) == note_get(nrpn_get(KYRP_PIN4_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN4_NOTE));
      pin_4_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN5_PIN) >= 0 && nrpn_get(KYRP_PIN5_NOTE) > 0) {
    static elapsedSamples pin_5_debounce;
    if ((int)pin_5_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN5_PIN)) == note_get(nrpn_get(KYRP_PIN5_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN5_NOTE));
      pin_5_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN6_PIN) >= 0 && nrpn_get(KYRP_PIN6_NOTE) > 0) {
    static elapsedSamples pin_6_debounce;
    if ((int)pin_6_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN6_PIN)) == note_get(nrpn_get(KYRP_PIN6_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN6_NOTE));
      pin_6_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
  if (nrpn_get(KYRP_PIN7_PIN) >= 0 && nrpn_get(KYRP_PIN7_NOTE) > 0) {
    static elapsedSamples pin_7_debounce;
    if ((int)pin_7_debounce >= 0 && 
	digitalReadFast(nrpn_get(KYRP_PIN7_PIN)) == note_get(nrpn_get(KYRP_PIN7_NOTE))) {
      note_toggle(nrpn_get(KYRP_PIN7_NOTE));
      pin_7_debounce = -nrpn_get(KYRP_PIN_DEBOUNCE);
    }
  }
}

static void pin_set_pin(int nrpn) {
  if (nrpn_get(nrpn) >= 0)
    pinMode(nrpn_get(nrpn), INPUT_PULLUP);
}

static void pin_setup(void) {
  pinMode(KYR_L_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_R_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_S_KEY_PIN, INPUT_PULLUP);
  pinMode(KYR_EXT_PTT_PIN, INPUT_PULLUP);
  nrpn_listen(KYRP_PIN0_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN1_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN2_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN3_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN4_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN5_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN6_PIN, pin_set_pin);
  nrpn_listen(KYRP_PIN7_PIN, pin_set_pin);
}

static void pin_loop(void) {
  pin_sample();
}
