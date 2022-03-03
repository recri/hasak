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
#include "src/ring_buffer.h"

static elapsedSamples cwptt_tail_counter; /* count down the time */
static RingBuffer<uint32_t,256> cwptt_delay_line;
static unsigned long cwptt_tail_time;
static byte cwptt_tail_time_is_stale = 1; /* flag for initial computation */

static void cwptt_nrpn_listener(int nrpn, int _) {
  cwptt_tail_time_is_stale = 1;	/* flag for recompute */
}

static void cwptt_sidetone_listener(int note, int _) {
  if (note_get(KYRN_KEY_ST))	/* sidetone is on */
    if ( ! note_get(KYRN_PTT_OUT))
      note_toggle(KYRN_PTT_OUT);
  // queue element for possibly later replay
  if (cwptt_delay_line.can_put())
    cwptt_delay_line.put(timing_samples()+nrpn_get(NRPN_HEAD_TIME));
  else
    Serial.printf("cwptt_listener, delay line overflow, reduce speed or ptt head time\n");
}

static void cwptt_key_out_listener(int note, int _) {
  cwptt_tail_counter = 0;	     /* reset the element timer */
}

static void cwptt_sample(int nrpn, int _) {
  if (cwptt_tail_time_is_stale) {
    cwptt_tail_time_is_stale = 0;
    cwptt_tail_time = max(nrpn_get(NRPN_TAIL_TIME), nrpn_get(NRPN_HANG_TIME)*xnrpn_get(NRPN_XPER_DIT));
  }
  if (note_get(KYRN_PTT_OUT) &&		      /* ptt tx is on */
      note_get(KYRN_KEY_OUT) == 0 &&	      /* key tx is off */
      cwptt_tail_counter > cwptt_tail_time) { /* tail has expired */
    note_toggle(KYRN_PTT_OUT);		      /* ptt off */
  }
  if (cwptt_delay_line.can_get() &&	      /* something is queued */ 
      cwptt_delay_line.peek()-timing_samples() <= 0) { /* its time has come */
    cwptt_delay_line.get();			       /* pull it off the queue */
    note_toggle(KYRN_KEY_OUT);			       /* toggle the output note */
  }
}

static void cwptt_setup(void) {
  cwptt_tail_time_is_stale = 1;			    // flag for recompute
  nrpn_listen(NRPN_TAIL_TIME, cwptt_nrpn_listener); // and reflag when necessary
  nrpn_listen(NRPN_HANG_TIME, cwptt_nrpn_listener); // and reflag when necessary
  nrpn_listen(NRPN_XPER_DIT+1, cwptt_nrpn_listener); // and reflag when necessary
  note_listen(KYRN_KEY_ST, cwptt_sidetone_listener);
  note_listen(KYRN_KEY_OUT, cwptt_key_out_listener);
  nrpn_listen(NRPN_SAMPLE, cwptt_sample);
}

// static void cwptt_loop(void) {}
