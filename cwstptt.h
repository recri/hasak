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

static elapsedSamples cwstptt_tail_counter; /* count up the tail time */

static void cwstptt_sidetone_listener(int _, int __, int key_st) {
  if (key_st &&	/* sidetone on */
      ! note_get(NOTE_PTT_ST))  /* sidetone ptt not on */
      note_toggle(NOTE_PTT_ST);	/* start sidetone ptt */
  cwstptt_tail_counter = -xnrpn_get(NRPN_XPER_IWS); /* 7 dit word space tail */	/* reset the element timer */
}

static void cwstptt_sample(int _, int __, int ___) {
  if (note_get(NOTE_PTT_ST) != 0 &&   /* ptt st is on */
      note_get(NOTE_KEY_ST) == 0 &&   /* key st is off */
      (int)cwstptt_tail_counter >= 0) /* tail has expired */
    note_toggle(NOTE_PTT_ST);	      /* ptt off */
}

static void cwstptt_setup(void) {
  note_listen(NOTE_KEY_ST, cwstptt_sidetone_listener);
  nrpn_listen(NRPN_SAMPLE, cwstptt_sample);
}

// static void cwstptt_loop(void) {}
