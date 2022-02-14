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

static elapsedSamples ptt_timingCounter;
static uint32_t ptt_timing_head; /*  */
static uint32_t ptt_timing_tail; /*  */

static void ptt_timing_st(int note) {
  if (note == KYRN_KEY_ST) {	/* sidetone transition */
    if (note_get(KYRN_KEY_ST)) { /* sidetone is on */
      if ( ! note_get(KYRN_PTT_TX)) {
	note_toggle(KYRN_PTT_TX);
	ptt_timing_head = get_nrpn(KYRP_HEAD_TIME);
	ptt_timing_tail = max(get_nrpn(KYRP_TAIL_TIME), get_nrpn(KYRP_HANG_TIME)*get_xnrpn(KYRP_XPER_DIT));
      }
      note_after(ptt_timing_head, KYRN_KEY_TX, 1);
    } else {			/* sidetone is off */
      note_after(ptt_timing_head, KYRN_KEY_TX, 0);
    }
  } else if (note == KYRN_KEY_TX) { /* tx keying transition */
    ptt_timingCounter = 0;	/* time the element */
  }
}

static void ptt_timing_setup(void) {
  note_listen(KYRN_KEY_ST, ptt_timing_listener);
  note_listen(KYRN_KEY_TX, ptt_timing_listener);
  note_listen(KYRN_PTT_TX, ptt_timing_listener);
}

static void ptt_timing_loop(void) {
  if (note_get(KYRN_PTT_TX) &&		      /* ptt tx is on */
      note_get(KYRN_KEY_TX) == 0 &&	      /* key tx is off */
      ptt_timingCounter > ptt_timing_tail)    /* tail has expired */
    note_toggle(KYRN_PTT_TX);		      /* ptt off */
}
