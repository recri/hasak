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
** sidetone ptt timing, just a simple tail
*/

static elapsedSamples stptt_tail_counter; /* count up the tail time */

static void stptt_sidetone_listener(int note) {
  if (note_get(KYRN_KEY_ST) &&	/* sidetone on */
      ! note_get(KYRN_PTT_ST))  /* ptt not on */
      note_toggle(KYRN_PTT_ST);	/* start sidetone */
  stptt_tail_counter = 0;	/* reset the element timer */
}

static void stptt_every_sample(void) {
  const unsigned tail = xnrpn_get(KYRP_XPER_IWS); /* 7 dit word space tail */
  if (note_get(KYRN_PTT_ST) &&		      /* ptt st is on */
      note_get(KYRN_KEY_ST) == 0 &&	      /* key st is off */
      stptt_tail_counter > tail) 	      /* tail has expired */
    note_toggle(KYRN_PTT_ST);		      /* ptt off */
}

static void stptt_setup(void) {
  note_listen(KYRN_KEY_ST, stptt_sidetone_listener);
  every_sample(stptt_every_sample);
}

static void stptt_loop(void) {
}
