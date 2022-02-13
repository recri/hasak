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

static void keyer_setup(void) {
}

static KeyerPaddle keyer_paddle(KYRF_PAD);
static elapsedMicros keyer_timer;

static void keyer_loop(void) {
  if (hasak.notes[KYRN_S_KEY] != hasak.notes[KYRN_S_KEY_ST]) note_toggle(KYRN_S_KEY_ST);
  if (hasak.notes[KYRN_TUNE] != hasak.notes[KYRN_TUNE_ST]) note_toggle(KYRN_TUNE_ST);
  const int key = keyer_paddle.clock(hasak.notes[KYRN_L_PAD], hasak.notes[KYRN_R_PAD], keyer_timer);
  keyer_timer = 0;
  if (key != hasak.notes[KYRN_PAD_ST]) note_toggle(KYRN_PAD_ST);
  if (hasak.notes[KYRN_BUT] != hasak.notes[KYRN_BUT_ST]) note_toggle(KYRN_BUT_ST);
}

