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
** ptt timing shim
*/

static elapsedSamples keypttCounter;
static uint32_t keyptt_head; /*  */
static uint32_t keyptt_tail; /*  */

static void keyptt_listener(int note) {
  if (note == KYRN_KEY_ST) {	/* sidetone transition */
    if (note_get(KYRN_KEY_ST)) { /* sidetone is on */
      if ( ! note_get(KYRN_PTT_TX)) {
	note_toggle(KYRN_PTT_TX);
	keyptt_head = get_nrpn(KYRP_HEAD_TIME);
	keyptt_tail = max(get_nrpn(KYRP_TAIL_TIME), get_nrpn(KYRP_HANG_TIME)*get_xnrpn(KYRP_XPER_DIT));
      }
      note_after(keyptt_head, KYRN_KEY_TX, 1);
    } else {			/* sidetone is off */
      note_after(keyptt_head, KYRN_KEY_TX, 0);
    }
  } else if (note == KYRN_KEY_TX) { /* tx keying transition */
    keypttCounter = 0;	/* time the element */
  }
}

static void keyptt_setup(void) {
  note_listen(KYRN_KEY_ST, keyptt_listener);
  note_listen(KYRN_KEY_TX, keyptt_listener);
  note_listen(KYRN_PTT_TX, keyptt_listener);
}

static void keyptt_loop(void) {
  if (note_get(KYRN_PTT_TX) &&		      /* ptt tx is on */
      note_get(KYRN_KEY_TX) == 0 &&	      /* key tx is off */
      keypttCounter > keyptt_tail)	      /* tail has expired */
    note_toggle(KYRN_PTT_TX);		      /* ptt off */
}
