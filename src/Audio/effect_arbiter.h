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

#include "run_length_queue.h"

class AudioEffectArbiter : public AudioStream {
public:
  AudioEffectArbiter() : AudioStream(KYR_N_VOX, inputQueueArray) {
    active_stream = -1;
  }

  virtual void update(void);

  void define_vox(int index, int _vox, int _priority, int _local=0) {
    if (index >= 0 && index < KYR_N_VOX) {
      vox[index] = _vox;
      priority[index] = _priority;
      local[index] = _local;
    }
  }

  uint8_t get_active_vox(void) { return active_stream < 0 ? KYR_VOX_NONE : vox[active_stream]; }

  void queue_reset(void) {
    keyq.reset();
    pttq.reset();
  }

  bool queue_is_empty(void) { return pttq.emptyp(); }

  bool is_not_local(void) { return active_stream >= 0 && ! local[active_stream]; }
  
  void queue_runs(int16_t key_run, int16_t ptt_run) {
    if (keyq.can_put_run()) keyq.put_run(key_run);
    if (pttq.can_put_run()) pttq.put_run(ptt_run);
  }

  int16_t get_key(void) { return keyq.get_bit(); }
  int16_t get_ptt(void) { return pttq.get_bit(); }
    
private:
  int16_t active_stream, active_tail, change_over;
  uint8_t vox[KYR_N_VOX], priority[KYR_N_VOX], local[KYR_N_VOX];
  RunLengthQueue keyq, pttq;
  audio_block_t *inputQueueArray[KYR_N_VOX];
};

#endif
