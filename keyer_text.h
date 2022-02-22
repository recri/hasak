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
** hasak text keyer component.
**
** I've adopted the compact code representation and the winkey convention
** of a prefix operator to form prosigns by concatenation, but I'm feeling
** a little queasy about corner cases.
**
** The morse table has moved into the NRPN array, so it can be augmented as
** required.  It covers the 64 characters from ! to `, though many are undefined
** by default.
*/

#include "src/keyer_text.h"

static KeyerText keyer_text(KYRN_TXT_TEXT, KYRN_ST_TEXT);

static void keyer_text_listen(int note) { keyer_text.receive(); }

static void keyer_text_clock(void) { keyer_text.clock(1); }

#if defined (KYR_ENABLE_TEXT2)
static KeyerText keyer_text2(KYRN_TXT_TEXT2, KYRN_ST_TEXT2);

static void keyer_text2_listen(int note) { keyer_text2.receive(); }

static void keyer_text2_clock(void) { keyer_text2.clock(1); }
#endif

static void keyer_text_setup(void) {

  note_listen(keyer_text.text_note, keyer_text_listen);
  every_sample(keyer_text_clock);

#if defined(KYR_ENABLE_TEXT2)
  note_listen(keyer_text2.text_note, keyer_text2_listen);
  every_sample(keyer_text2_clock);
#endif

}

static void keyer_text_loop(void) {
}
