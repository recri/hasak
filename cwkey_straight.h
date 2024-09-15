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
 * straight key keyers
 */
static void cwkey_straight_s_key_listen(int _, int __, int ___) {
  if (note_get(NOTE_HW_S_KEY) != note_get(NOTE_ST_S_KEY)) note_toggle(NOTE_ST_S_KEY);
}

static void cwkey_straight_s_key2_listen(int _, int __, int ___) {
  if (note_get(NOTE_HW_S_KEY2) != note_get(NOTE_ST_S_KEY2)) note_toggle(NOTE_ST_S_KEY2);
}

static void cwkey_straight_s_key3_listen(int _, int __, int ___) {
  if (note_get(NOTE_HW_S_KEY3) != note_get(NOTE_ST_S_KEY3)) note_toggle(NOTE_ST_S_KEY3);
}

static void cwkey_straight_tune_listen(int _, int __, int ___) {
  if (note_get(NOTE_MIDI_IN_TUNE) != note_get(NOTE_ST_TUNE)) note_toggle(NOTE_ST_TUNE);
}  

static void cwkey_straight_setup(void) {
  note_listen(NOTE_HW_S_KEY, cwkey_straight_s_key_listen);
  note_listen(NOTE_MIDI_IN_TUNE, cwkey_straight_tune_listen);
  note_listen(NOTE_HW_S_KEY2, cwkey_straight_s_key2_listen);
  note_listen(NOTE_HW_S_KEY3, cwkey_straight_s_key3_listen);
}
