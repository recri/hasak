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
 * We have allocated KYR_N_POUT MIDI notes for digital pin output transitions
 * These are named KYR_NOTE_POUT0 to KYR_NOTE_POUT7
 * The overall enable for output pins is NRPN_POUT_ENABLE
 * Output pins are individually enabled by assignment of an valid pin number to
 * NRPN_POUT0_PIN to NRPN_POUT7_PIN, valid pin numbers
 * These pins currently all use KYR_POUT_LOGIC
 * match the KYR_N_NOTE nrpns that specify the pin for output
 * so either the note or the nrpn suffice to compute the other
 * note_get() is active high, 
 * hermes lite KEY/PTT jack is active low
 * POUT_LOGIC should be 0 for this to work out
 */

/*
 * This listens to the midi note bound to this pin
 * and sets the pin value to correspond to the midi note value.
 */
static void pout_note_listener(int note, int _) { 
  const int pin = nrpn_get(note-NOTE_POUT0+NRPN_POUT0_PIN);
  // Serial.printf("pout_note_listener(note=%d, _) -> pin=%d\n", note, pin);
  if (nrpn_get(NRPN_POUT_ENABLE) && pin_valid(pin)) {
    digitalWriteFast(pin, 1^nrpn_get(NRPN_POUT_LOGIC)^note_get(note));
    // Serial.printf("pout_note_listener(note=%d, _) digitalWrite(pin=%d, val=%d)\n", note, pin, 1^nrpn_get(NRPN_POUT_LOGIC)^note_get(note));
  }
}

/*
 * This listens to the nrpn which assigns this POUT to its pin
 * and sets the pin value to correspond to the midi note value.
 */
static void pout_pin_listener(int nrpn, int _) {
  const int pin = nrpn_get(nrpn);
  const int note = (nrpn-NRPN_POUT0_PIN+NOTE_POUT0);
  // Serial.printf("pout_pin_listener(nrpn=%d, _) -> pin=%d and note=%d\n", nrpn, pin, note);
  if (pin_valid(pin)) {
    pinMode(pin, OUTPUT);
    digitalWriteFast(pin, 1^nrpn_get(NRPN_POUT_LOGIC)^note_get(note));
    // Serial.printf("pout_pin_listener(nrpn=%d, _) digitalWrite(pin=%d, val=%d)\n", nrpn, pin, 1^nrpn_get(NRPN_POUT_LOGIC)^note_get(note));
  }
}

static void pout_setup(void) {
  // while ( ! Serial);
  for (int i = 0; i < KYR_N_POUT; i += 1) {
    nrpn_listen(NRPN_POUT0_PIN+i, pout_pin_listener);
    note_listen(NOTE_POUT0+i, pout_note_listener);
    // Serial.printf("nrpn_listen(%d, pout_pin_listener)\n", NRPN_POUT0_PIN+i);
    // Serial.printf("note_listen(%d, pout_note_listener)\n", NOTE_POUT0+i);
  }
}

// static void pout_loop(void) {}
