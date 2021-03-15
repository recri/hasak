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

#ifndef effect_arbiter_h_
#define effect_arbiter_h_
#include "../../config.h"
#include "Arduino.h"
#include "../../linkage.h"
#include "AudioStream.h"

/* this should be reused in input_text */
/* circular buffer of int16_t for run lengths */

class RingBuffer {
public:
  RingBuffer() { }
  void reset(void) { wptr = rptr = 0; }
  bool can_get(void) { return rptr!=wptr; }
  bool can_put(void) { return (wptr+1)%SIZE != rptr; }
  int16_t peek(void) { return buff[rptr]; }
  int16_t get(void) { int16_t val = buff[rptr++]; rptr %= SIZE; return val; }
  void put(int16_t val) { buff[wptr++] = val; wptr %= SIZE; }
  int16_t items(void) { return ((uint16_t)(wptr-rptr))%SIZE; }
  static const uint16_t SIZE = 128u;
private:
  uint16_t wptr = 0, rptr = 0;
  int16_t buff[SIZE];
};

/*
** circular buffer for run lengths of key and ptt events
** positive runs indicate sequences of 1's
** negative runs indicate sequences of 0's
** a zero run indicates empty
*/
class RunLengthQueue : RingBuffer {
public:
  RunLengthQueue() : RingBuffer() { }
  /* test the waters */
  int can_put_run(void) { return items()<SIZE; }
    
  /* append the run onto the queue */
  int put_run(int16_t run) { 
    if (in == 0) {
      in = run;
    } else if ((in < 0) == (run < 0)) {
      in += run;
    } else if (can_put()) {
      put(in);
      in = run;
    } else {
      overrun += 1;
      return -1;
    }
    maxusage = max(maxusage, items());
    return 0;
  }
  /* return the next bit in the run, -1 if none */
  int get_bit(void) {
    if (out == 0) {
      if (can_get())
	out = get();
      else if (in != 0) {
	out = in;
	in = 0;
      } else {
	underrun += 1;
	return -1;
      }
    }
    if (out < 0) { 
      out += 1; 
      return 0;
    }
    out -= 1;
    return 1;
  }
  bool emptyp(void) { 
    return ! (out != 0 || can_get());
  }
  void reset(void) {
    RingBuffer::reset();
    in = out = 0;
  }
  int overrun = 0, underrun = 0, maxusage = 0;
private:
  int16_t in = 0, out = 0;
};

class AudioEffectArbiter : public AudioStream {
public:
  AudioEffectArbiter() : AudioStream(KYR_N_VOX, inputQueueArray), active_stream(-1) { }
  virtual void update(void);
  void define_vox(int index, int _vox, int _priority) {
    if (index >= 0 && index < KYR_N_VOX) {
      vox[index] = _vox; priority[index] = _priority;
    }
  }
  uint8_t get_active_vox(void) { return active_stream < 0 ? KYR_VOX_NONE : vox[active_stream]; }

  void queue_reset(void) {
    keyq.reset();
    pttq.reset();
  }
  bool queue_is_empty(void) { return pttq.emptyp(); }
  void queue_runs(int16_t key_run, int16_t ptt_run) {
    if (keyq.can_put_run()) keyq.put_run(key_run);
    if (pttq.can_put_run()) pttq.put_run(ptt_run);
  }
  int16_t get_key(void) { return keyq.get_bit(); }
  int16_t get_ptt(void) { return pttq.get_bit(); }
    
private:
  int16_t active_stream, active_tail, change_over;
  uint8_t vox[KYR_N_VOX], priority[KYR_N_VOX];
  RunLengthQueue keyq, pttq;
  audio_block_t *inputQueueArray[KYR_N_VOX];
};

#endif
