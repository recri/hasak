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
 * paddle keyers
 */

#include "src/keyer_paddle.h"

static KeyerPaddle cwkey_paddle;
static KeyerPaddle cwkey_paddle2;
static KeyerPaddle cwkey_paddle3;

static void cwkey_paddle_listen(int note, int _) {
  if (note_get(NOTE_ST_PAD) != cwkey_paddle.clock(note_get(NOTE_HW_L_PAD), note_get(NOTE_HW_R_PAD), 0))
    note_toggle(NOTE_ST_PAD);
}

static void cwkey_paddle2_listen(int note, int _) {
  if (note_get(NOTE_ST_PAD2) != cwkey_paddle2.clock(note_get(NOTE_HW_L_PAD2), note_get(NOTE_HW_R_PAD2), 0))
    note_toggle(NOTE_ST_PAD2);
}

static void cwkey_paddle3_listen(int note, int _) {
  if (note_get(NOTE_ST_PAD3) != cwkey_paddle3.clock(note_get(NOTE_HW_L_PAD3), note_get(NOTE_HW_R_PAD3), 0))
    note_toggle(NOTE_ST_PAD3);
}

static void cwkey_paddle_sample(int nrpn, int _) {
  if (note_get(NOTE_ST_PAD) != cwkey_paddle.clock(note_get(NOTE_HW_L_PAD), note_get(NOTE_HW_R_PAD), 1))
    note_toggle(NOTE_ST_PAD);
  if (note_get(NOTE_ST_PAD2) != cwkey_paddle2.clock(note_get(NOTE_HW_L_PAD2), note_get(NOTE_HW_R_PAD2), 1))
    note_toggle(NOTE_ST_PAD2);
  if (note_get(NOTE_ST_PAD3) != cwkey_paddle3.clock(note_get(NOTE_HW_L_PAD3), note_get(NOTE_HW_R_PAD3), 1))
    note_toggle(NOTE_ST_PAD3);
}

static void cwkey_paddle_setup(void) {
  note_listen(NOTE_HW_L_PAD, cwkey_paddle_listen);
  note_listen(NOTE_HW_R_PAD, cwkey_paddle_listen);
  note_listen(NOTE_HW_L_PAD2, cwkey_paddle2_listen);
  note_listen(NOTE_HW_R_PAD2, cwkey_paddle2_listen);
  note_listen(NOTE_HW_L_PAD3, cwkey_paddle3_listen);
  note_listen(NOTE_HW_R_PAD3, cwkey_paddle3_listen);
  nrpn_listen(NRPN_SAMPLE, cwkey_paddle_sample);
}

// static void cwkey_paddle_loop(void) {}
