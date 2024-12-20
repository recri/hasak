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
 * These functions are responsible for entering pin input events
 * static void pin_setup(void) is called once to initialize the handlers
 * static void pin_sample(int nrpn, int _) is called every sample time to 
 * poll the input pins and update the associated pin notes.
 * static void pin_pin_listener(int nrpn, int _) is called when the physical
 * pin associated to the abstract pin is set, it initializes the pin mode.
 */

static void pin_sample(int nrpn, int _) {

  /* KYR_N_PIN digital input pins which can be wired up */
  /* assumes that the pin and note nrpns occur in a contiguous block */
  /* ah, inverted logic, pins are active low, notes are active high */
  /* debounce by ignoring changes for a period of time */
  if ( ! nrpn_get(NRPN_PIN_ENABLE)) return;
  static elapsedSamples debounce[KYR_N_PIN];
  for (int i = 0; i < KYR_N_PIN; i += 1) {
    const int pin = nrpn_get(NRPN_PIN0_PIN+i);
    if (pin_valid(pin)) {
      if ((int)debounce[i] >= 0) {
	const int note = NOTE_PIN0+i;
	debounce[i] = 0;
	/* this looks funny because pin is active low and note is active high */
	if (digitalReadFast(pin) == note_get(note)) {
	  note_toggle(note);
	  debounce[i] = -nrpn_get(NRPN_PIN_DEBOUNCE);
	}
      }
    }
  }
}

static void pin_pin_listener(int nrpn, int _) {
  const int pin = nrpn_get(nrpn);
  if (pin_valid(pin))
    pinMode(pin, INPUT_PULLUP);
}

static void pin_setup(void) {
  for (int i = 0; i < KYR_N_PIN; i += 1)
    nrpn_listen(NRPN_PIN0_PIN+i, pin_pin_listener);
  nrpn_listen(NRPN_SAMPLE, pin_sample);
}

// static void pin_loop(void) {}
