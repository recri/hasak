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
  int16_t i, n_active = 0, ibest = -1, send = 0, ended = 0, change_over = 0;

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

  if (active_stream < 0) {
    /* there is no currently active stream */
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
    if (n_active == 0 || priority[active_stream] >= priority[ibest]) {
      /* no contention */
      /* send the active stream */
      send = 1;
    } else {
      /* pre-empt active stream */
      change_over = 1;
      /* abort keyed stream for current active stream */
      queue_reset();
      /* set the new active stream */
      active_stream = ibest;
      /* start the new active stream */
      send = 1;
    }
  }

  /* send says we found something to send in the input stream */
  if (send) {
    /* fetch ptt parameters */
    int16_t ptt_head = time_to_samples(get_vox_ptt_head(get_active_vox()));
    int16_t ptt_tail = time_to_samples(get_vox_ptt_tail(get_active_vox()));

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
    if (block[active_stream] && get_st_enable())
      transmit(block[active_stream], 0);

    /* deal with undelayed voices */
    if (ptt_head == 0) {
      /* immediate send, no ptt delay */
      int16_t *pkeyin = block[active_stream] ? block[active_stream]->data : (int16_t *)zeros;
      int16_t *pend = pkeyin+AUDIO_BLOCK_SAMPLES;
      audio_block_t *pttout = allocate();
      if (block[active_stream] && get_tx_enable() && is_not_local()) transmit(block[active_stream],1);
      if (pttout) {
	int16_t *pptt = pttout->data;
	while (pkeyin < pend) {
	  if (*pkeyin++ == 0) {
	    if (active_tail > 0) {
	      active_tail -= 1;
	      *pptt++ = bool2fix(1);
	    } else {
	      ended = 1;
	      *pptt++ = bool2fix(0);
	    }
	  } else {
	    active_tail = ptt_head+ptt_tail;
	    ended = 0;
	    *pptt++ = bool2fix(1);
	  }
	}
	if (get_tx_enable() && is_not_local()) transmit(pttout, 2);
	release(pttout);
      }
    } else {
      /* ptt_head delay on key line */
      /* queue key in and ptt */
      int16_t *pkeyin = block[active_stream] ? block[active_stream]->data : (int16_t *)zeros;
      int16_t *pend = pkeyin+AUDIO_BLOCK_SAMPLES;
      int delay_queued = active_tail > 0;
      while (pkeyin < pend) {
	if (*pkeyin++ == 0) {
	  /* zero keyed, count down active_tail */
	  if (active_tail > 0) {
	    active_tail -= 1;
	    queue_runs(-1, +1);
	  } else {
	    ended = 1;
	    queue_runs(-1, -1);
	  }
	} else {
	  /* one keyed, reset active_tail */
	  active_tail = ptt_tail;
	  /* push delay if necessary */
	  if ( ! delay_queued) {
	    queue_runs(+ptt_head, -ptt_head);
	    delay_queued = 1;
	  }
	  /* queue key and ptt */
	  ended = 0;
	  queue_runs(+1, +1);
	}
      }
      /* fill output buffers with queued output */
      if ( ! queue_is_empty()) {
	/* process the queue */
	audio_block_t *keyout = allocate(), *pttout = allocate();
	if (keyout && pttout) {
	  int16_t *pk, *pp, *pend;
	  int sumk = 0, sump = 0;
	  pk = keyout->data;
	  pp = pttout->data;
	  pend = pk+AUDIO_BLOCK_SAMPLES;
	  while (pk < pend && ! queue_is_empty()) {
	    sumk += *pk++ = bool2fix(get_key());
	    sump += *pp++ = bool2fix(get_ptt());
	  }
	  /* queue has drained, reset active_stream and fill buffer with zeros */
	  if (pk < pend) {
	    ended = 1;
	    while (pk < pend) {
	      *pk++ = bool2fix(0);
	      *pp++ = bool2fix(0);
	    }
	  }
	  if (get_tx_enable() && is_not_local()) {
	    if (sumk != 0) transmit(keyout, 1); // send key out stream
	    if (sump != 0) transmit(pttout, 2); // send ptt out stream
	  }
	} else {
	  /* could drain the queue of a block worth anyway */
	}
	if (keyout) release(keyout);
	if (pttout) release(pttout);
      }
    }
  }
  if (ended) active_stream = -1;
  for (i = 0; i < KYR_N_VOX; i += 1) 
    if (block[i]) release(block[i]);
}
