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
#include "cwmorse.h"		// morse code table
#include "linkage.h"		// forward references, linkage, and conversions
#include "audio.h"		// audio graph
#undef READ_ONLY		// trash from the audio library includes
#include "timing.h"		// timing counters
#include "every_sample.h"	// every_sample implementation
#include "after_idle.h"		// after_idle implementation
#include "midi.h"		// midi interface
#include "default.h"		// define MIDI note, ctrl, and nrpn flags FIX.ME rename midi_defs.h
#include "codec.h"		// handle the audio hardware
#include "nrpn.h"		// default values and listeners for nrpns
#include "cwdecode.h"		// decode cw element stream -> ascii
#include "cwdetime.h"		// detime cw key line -> element stream
#include "cwroute.h"		// route notes into and out of the keyer
#include "cwptt.h"		// translate sidetone key -> tx ptt and key
#include "cwstptt.h"		// translate sidetone key -> sidetone ptt
#include "cwarbiter.h"		// arbitrate multiple side tone key lines
#include "cwkey_straight.h"	// hardware switch -> straight key sidetone
#include "cwkey_paddle.h"	// hardare paddle -> iambic keyed sidetone
#include "cwkey_text.h"		// ascii characters -> mechanically keyed sidetone
#include "input.h"		// input adc states -> input adc nrpns FIX.ME rename inadc.h
// #include "inmap.h"		// input adc nrpns -> keyer parameter nrpns
#include "inpin.h"		// input pin states -> input pin notes FIX.ME rename
// #include "outpin.h"		// output notes -> output pins FIX.ME
// #include "instring.h"	// input strings from MIDI
// #include "outstring.h"	// output strings to MIDI
// #include "cwinkey.h"		// FIX.ME - make it work
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

  input_setup();
  inpin_setup();
  cwkey_straight_setup();
  cwkey_paddle_setup();
  cwkey_text_setup();
  cwarbiter_setup();
  cwstptt_setup();
  cwptt_setup();
  cwroute_setup();
  cwdetime_setup();
  cwdecode_setup();
  // cwinkey_setup();
  diagnostics_setup();
}

/*
** loop calls the loop function for each module
** but many of those functions are empty.
*/
void loop(void) {
  timing_loop();		// accumulate counts
  midi_loop();			// drain midi input
  codec_loop();
  default_loop();
  nrpn_loop();
  inpin_loop();
  input_loop();
  cwkey_straight_loop();
  cwkey_paddle_loop();
  cwkey_text_loop();
  cwarbiter_loop();		// arbitration of keyer events
  cwstptt_loop();			// probably
  cwptt_loop();			// key generated ptt
  cwroute_loop();
  cwdetime_loop();
  cwdecode_loop();
  // cwwinkey_loop();		// winkey
  every_sample_loop();		// run every sample
  after_idle_loop();		// run once at end of loop()
  diagnostics_loop();		// console
}


