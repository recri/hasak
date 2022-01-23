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
#include "../../config.h"
#include <Arduino.h>
#include "../../linkage.h"
#include "effect_arbiter.h"
#include "sample_value.h"
#include "buffer_value.h"

/* 
** simplified arbiter that ors together any number of inputs
*/
void AudioEffectArbiter::update(void)
{
  audio_block_t *block[KYR_N_VOX];
  int16_t i, n_active = 0, ibest = -1, send = 0, change_over = 0;

  /* scan incoming streams for activity */
  /* find highest priority stream among those */
  for (i = 0; i < KYR_N_VOX; i += 1) {
    block[i] = receiveReadOnly(i);
    if (run_length(block[i]) != -AUDIO_BLOCK_SAMPLES) {
      n_active += 1;	// active if keyed
      /* remember the highest priority active stream */
      if (ibest < 0 || priority[ibest] < priority[i])
	ibest = i;
    }
  }

  /* the result of the scan is combined with active_stream */
  if (active_stream < 0) {
    /* there is no currently active stream */
    /* any lingering active_tail has been cleared */
    if (n_active == 0) {
      /* there is no new voice keying */
      /* send nothing */
      ;
    } else {
      /* there are one or more new voice(s) */
      active_stream = ibest;
      /* send the active stream */
      send = 1;
    }
  } else {
    /* there is a currently active stream */
    if (n_active == 0 || priority[active_stream] <= priority[ibest]) {
      /* no contention */
      /* send the active stream */
      send = 1;
    } else {
      /* pre-empt active stream */
      change_over = 1;
      /* abort keyed stream for current active stream */
      active_delay = -1;
      /* set the new active stream */
      active_stream = ibest;
      /* start the new active stream */
      send = 1;
    }
  }

  /* send says we found something to send in the input stream,
     or the active stream has not yet finished  */
  if (send) {

    /* change over means there must be one zero sample to trigger ramp off for previous vox */
    if (change_over && block[active_stream] && block[active_stream]->data[0] != bool2fix(0)) {
      /* rewrite block[active_stream] so it starts with a 0 */
      audio_block_t *change = allocate();
      if (change) {
	memcpy(change->data, block[active_stream]->data, sizeof(change->data));
	change->data[0] = bool2fix(0);
	release(block[active_stream]);
	block[active_stream] = change;
      }
    }

    /* deal with sidetone key stream first */
    if (block[active_stream] && (get_nrpn(KYRP_ST_ENABLE) || local[active_stream]))
      transmit(block[active_stream], 0);

    /* fetch current ptt parameters */
    int vox = get_active_vox();
    int32_t ptt_head = ms_to_samples(get_nrpn(KYRP_HEAD_TIME));
    int32_t ptt_tail = ms_to_samples(max(get_nrpn(KYRP_TAIL_TIME), get_nrpn(KYRP_HANG_TIME)*get_vox_xnrpn(vox, KYRP_XPER_DIT)));

    /* deal with actual change in active_delay, or a mismatch triggered by change_over */
    if (ptt_head != active_delay) {
      queue_reset();		// discard previous queue
      active_head = 0;		// no active ptt_head to fill
      active_tail = 0;		// no active ptt_tail to fill
      active_delay = ptt_head;	// new delay
      if (ptt_head) keyq.put_run(-ptt_head); // put the delay into the key queue
    }

    /* check for end of active stream */
    if ( ! block[active_stream] && active_head == 0 && active_tail == 0 && queue_is_all_zeros())
      active_stream = -1;

    /* queue key in and ptt_head */
    int16_t *pkeyin = (active_stream != -1 && block[active_stream]) ? block[active_stream]->data : (int16_t *)zeros;
    int16_t *pend = pkeyin+AUDIO_BLOCK_SAMPLES;

    /* read the incoming key stream */
    while (pkeyin < pend) {
      if (*pkeyin++ == 0) {
	/* zero keyed, count down active_head */
	if (active_head > 0) {
	  active_head -= 1;
	  queue_runs(-1, +1);
	} else {
	  queue_runs(-1, -1);
	}
      } else {
	/* one keyed, reset active_head */
	active_head = ptt_head;
	/* queue key and ptt */
	queue_runs(+1, +1);
	if (active_head > 0) active_head -= 1;
      }
    }

    /* okay, if active_head is zero, active_tail is zero, and both queues are ? */
    /* fill output buffers with queued output and apply ptt_tail */
    audio_block_t *keyout = allocate(), *pttout = allocate();
    if (keyout && pttout) {
      int16_t *pk, *pp, *pend, key, ptt;
      int sumk = 0, sump = 0;
      pk = keyout->data;
      pp = pttout->data;
      pend = pk+AUDIO_BLOCK_SAMPLES;
      while (pk < pend) {
	/* fetch from queue */
	key = get_key();
	ptt = get_ptt();
	if (key) {		// key active, reset tail
	  active_tail = ptt_tail;
	  ptt = 1;
	} else if (active_tail > 0) { // tail active, set ptt
	  active_tail -= 1;
	  ptt = 1;
	}
	/* store to output */
	sumk += *pk++ = bool2fix(key);
	sump += *pp++ = bool2fix(ptt);
      }
      if (active_stream >= 0 && get_nrpn(KYRP_TX_ENABLE) && ! local[active_stream]) {
	if (sumk != 0) transmit(keyout, 1); // send key out stream
	if (sump != 0) transmit(pttout, 2); // send ptt out stream
      }
    }
    if (keyout) release(keyout);
    if (pttout) release(pttout);
  }

  for (i = 0; i < KYR_N_VOX; i += 1) 
    if (block[i]) release(block[i]);
}
