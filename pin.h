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

  /* KYR_N_PIN digital input pins which can be wired up */
  /* assumes that the pin and note nrpns occur in a contiguous block */
  /* ah, inverted logic, pins are active low, notes are active high */
  /* debounce by ignoring changes for a period of time */
  static elapsedSamples debounce[KYR_N_PIN];
  for (int i = 0; i < KYR_N_PIN; i += 1) {
    const int pin = nrpn_get(KYRP_PIN0_PIN+i);
    const int note = KYRN_PIN0+i;
    if (pin_valid(pin)) {
      if ((int)debounce[i] >= 0 && digitalReadFast(pin) == note_get(note)) {
	note_toggle(note);
	debounce[i] = -nrpn_get(KYRP_PIN_DEBOUNCE);
      }
    }
  }
}

static void pin_pin_listener(int nrpn) {
  const int pin = nrpn_get(nrpn);
  if (pin_valid(pin))
    pinMode(pin, INPUT_PULLUP);
}

static void pin_setup(void) {
  for (int i = 0; i < KYR_N_PIN; i += 1)
    nrpn_listen(KYRP_PIN0_PIN+i, pin_pin_listener);
}

static void pin_loop(void) {
  pin_sample();
}
