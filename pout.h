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
 * digital pin output
 * the KYR_N_NOTE notes that generate digital pin transitions
 * match the KYR_N_NOTE nrpns that specify the pin for output
 * so either the note or the nrpn suffice to compute the other
 
 */
static void pout_note_listener(int note, int _, int __) { 
  const int pin = nrpn_get(note-NOTE_POUT0+NRPN_POUT0_PIN);
  if (nrpn_get(NRPN_PIN_ENABLE) && pin_valid(pin))
    digitalWriteFast(pin, 1^nrpn_get(NRPN_POUT_LOGIC)^note_get(note));
}

static void pout_pin_listener(int nrpn, int _, int pin) {
  if (pin_valid(pin)) pinMode(pin, OUTPUT);
}

static void pout_setup(void) {
  for (int i = 0; i < KYR_N_POUT; i += 1) {
    nrpn_listen(NRPN_POUT0_PIN+i, pout_pin_listener);
    note_listen(NOTE_POUT0+i, pout_note_listener);
  }
}

// static void pout_loop(void) {}
