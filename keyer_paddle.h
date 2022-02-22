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

#if defined(KYR_ENABLE_PADDLE1)
static KeyerPaddle keyer_paddle;

static void keyer_paddle_listen(int note) {
  if (note_get(KYRN_ST_PAD) != keyer_paddle.clock(note_get(KYRN_HW_L_PAD), note_get(KYRN_HW_R_PAD), 0))
    note_toggle(KYRN_ST_PAD);
}

static void keyer_paddle_clock(void) {
  if (note_get(KYRN_ST_PAD) != keyer_paddle.clock(note_get(KYRN_HW_L_PAD), note_get(KYRN_HW_R_PAD), 1))
    note_toggle(KYRN_ST_PAD);
}

#endif

#if defined(KYR_ENABLE_PADDLE2)
static KeyerPaddle keyer_paddle2;

static void keyer_paddle2_listen(int note) {
  if (note_get(KYRN_ST_PAD2) != keyer_paddle2.clock(note_get(KYRN_HW_L_PAD2), note_get(KYRN_HW_R_PAD2), 0))
    note_toggle(KYRN_ST_PAD2);
}

static void keyer_paddle2_clock(void) {
  if (note_get(KYRN_ST_PAD2) != keyer_paddle2.clock(note_get(KYRN_HW_L_PAD2), note_get(KYRN_HW_R_PAD2), 1))
    note_toggle(KYRN_ST_PAD2);
}

#endif

#if defined(KYR_ENABLE_PADDLE3)
static KeyerPaddle keyer_paddle3;

static void keyer_paddle3_listen(int note) {
  if (note_get(KYRN_ST_PAD3) != keyer_paddle3.clock(note_get(KYRN_HW_L_PAD3), note_get(KYRN_HW_R_PAD3), 0))
    note_toggle(KYRN_ST_PAD3);
}

static void keyer_paddle3_clock(void) {
  if (note_get(KYRN_ST_PAD3) != keyer_paddle3.clock(note_get(KYRN_HW_L_PAD3), note_get(KYRN_HW_R_PAD3), 1))
    note_toggle(KYRN_ST_PAD3);
}

#endif


static void keyer_paddle_setup(void) {

#if defined(KYR_ENABLE_PADDLE1)
  note_listen(KYRN_HW_L_PAD, keyer_paddle_listen);
  note_listen(KYRN_HW_R_PAD, keyer_paddle_listen);
  every_sample(keyer_paddle_clock);
#endif

#if defined(KYR_ENABLE_PADDLE2)
  note_listen(KYRN_HW_L_PAD2, keyer_paddle2_listen);
  note_listen(KYRN_HW_R_PAD2, keyer_paddle2_listen);
  every_sample(keyer_paddle2_clock);
#endif

#if defined(KYR_ENABLE_PADDLE3)
  note_listen(KYRN_HW_L_PAD3, keyer_paddle3_listen);
  note_listen(KYRN_HW_R_PAD3, keyer_paddle3_listen);
  every_sample(keyer_paddle3_clock);
#endif

}

static void keyer_paddle_loop(void) {
}
