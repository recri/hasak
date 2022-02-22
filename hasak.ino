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

#include <Arduino.h>
static int pin_valid(int pin) { return (unsigned)pin < (unsigned)CORE_NUM_TOTAL_PINS; }
#include "config.h"		// configuration
#include "linkage.h"		// 
#include "audio.h"
#undef READ_ONLY		// from somewhere in the libraries the audio library wants
#include "timing.h"		// timing 
#include "every_sample.h"
#include "after_idle.h"
#include "midi.h"
#include "default.h"
#include "codec.h"
#include "nrpn.h"
#include "decode.h"
#include "cwroute.h"
#include "cwptt.h"
#include "stptt.h"
#include "arbiter.h"
#include "keyer_straight.h"
#include "keyer_paddle.h"
#include "keyer_text.h"
#include "input.h"
#include "inpin.h"
//#include "winkey.h"
#include "diagnostics.h"

//
// poll input pins at sample rate
// latch output pins at sample rate
// also poll/latch arbitrary memory locations
// also combine latch output pins and memory locations
// should move the bool2fix/fix2bool into the audio
// loop which runs at lower priority than this
// interrupt handler.
//
static void interrupt() {
  // maintain timing information
  uint32_t cycles = ARM_DWT_CYCCNT;
  // sidetone and transmit key to audio loop
  au_st_key.send(note_get(KYRN_AU_ST_KEY));
  au_key_out.send(note_get(KYRN_AU_IQ_KEY));
  // maintain timing information
  timing_isrCpuCyclesRaw += ARM_DWT_CYCCNT - cycles;
  timing_sampleCounter += 1;
}

void setup(void) {
  Serial.begin(115200);

  /* FIX.ME - these go in outpin.h */
  pinMode(KYR_KEY_OUT_PIN, OUTPUT); digitalWrite(KYR_KEY_OUT_PIN, 1);
  pinMode(KYR_PTT_OUT_PIN, OUTPUT); digitalWrite(KYR_PTT_OUT_PIN, 1);

  /* FIX.ME - these go in audio.h - audio_setup() */
  AudioMemory(40);

  nrpn_set_default(KYRP_SET_DEFAULT);
  nrpn_set(KYRP_VOLUME, -200);

  midi_setup();
  default_setup();
  codec_setup();
  codec_enable();
  nrpn_setup();

  /*
  ** set an interval timer to latch inputs into the audio update loop.
  */
  static IntervalTimer timer;
  timer.priority(96);
  timer.begin(interrupt, 1e6/AUDIO_SAMPLE_RATE_EXACT);

  timing_setup();		// start your timers
  every_sample_setup();
  after_idle_setup();

  input_setup();		// prepare to read adc's
  inpin_setup();		// prepare to read pin's
  keyer_straight_setup();
  keyer_paddle_setup();
  keyer_text_setup();
  arbiter_setup();		// prepare to arbitrate
  stptt_setup();
  cwptt_setup();		// cw ptt setup
  cwroute_setup();
  decode_setup();
  // winkey_setup();
  diagnostics_setup();
}

/*
** loop calls the loop function for each module
** but many of those functions are empty.
*/
void loop(void) {
  timing_loop();		// accumulate counts
  midi_loop();			// drain midi input
  codec_loop();			// probably nothing
  default_loop();		// probably nothing
  nrpn_loop();			// probably nothing
  inpin_loop();			// poll digital input pins
  input_loop();			// poll analog input pins
  keyer_straight_loop();	// probably nothing
  keyer_paddle_loop();		// probably nothing
  keyer_text_loop();		// probably nothing
  arbiter_loop();		// arbitration of keyer events
  stptt_loop();			// probably
  cwptt_loop();			// key generated ptt
  cwroute_loop();
  decode_loop();
  // winkey_loop();		// winkey
  every_sample_loop();		// run every sample
  after_idle_loop();		// run once at end of loop()
  diagnostics_loop();		// console
}


