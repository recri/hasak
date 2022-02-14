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
 * keyers
 */
#include "keyer_paddle.h"

static KeyerPaddle keyer_paddle(KYRF_PAD);

static void keyer_s_key(int note) {
  if (hasak.notes[KYRN_S_KEY] != hasak.notes[KYRN_S_KEY_ST]) note_toggle(KYRN_S_KEY_ST);
}

static void keyer_tune(int note) {
  if (hasak.notes[KYRN_TUNE] != hasak.notes[KYRN_TUNE_ST]) note_toggle(KYRN_TUNE_ST);
}  

static void keyer_but(int note) {
  if (hasak.notes[KYRN_BUT] != hasak.notes[KYRN_BUT_ST]) note_toggle(KYRN_BUT_ST);
}

static void keyer_pad(int note) {
  const int key = keyer_paddle.clock(hasak.notes[KYRN_L_PAD], hasak.notes[KYRN_R_PAD], 0);
  if (key != note_get(KYRN_PAD_ST))
    note_toggle(KYRN_PAD_ST);
  Serial.printf("keyer_pad(%d) -> key %d\n", note, key);
		
}

static void keyer_setup(void) {
  note_listen(KYRN_S_KEY, keyer_s_key);
  note_listen(KYRN_TUNE, keyer_tune);
  note_listen(KYRN_BUT, keyer_but);
  note_listen(KYRN_L_PAD, keyer_pad);
  note_listen(KYRN_R_PAD, keyer_pad);
}

static void keyer_loop(void) {
  static elapsedSamples ticks;
  if (ticks) {
    const int key = keyer_paddle.clock(hasak.notes[KYRN_L_PAD], hasak.notes[KYRN_R_PAD], ticks);
    if (key != note_get(KYRN_PAD_ST))
      note_toggle(KYRN_PAD_ST);
    ticks = 0;
  }
}

