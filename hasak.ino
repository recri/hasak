/* -*- mode: c++; tab-width: 8 -*- */
/*
 * hasak (ham and swiss army knife) keyer for Teensy 4.X, 3.X
 * Copyright (c) 2021, 2022 by Roger Critchlow, Charlestown, MA, USA
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

#include "config.h"		// configuration
#include "linkage.h"		// forward references, linkage, and conversions

static unsigned audio_update_counter(void); // forward for timing.h
static void audio_timing_reset(void);	    // forward for timing.h

#include "timing.h"		// timing counters
#include "listener.h"		// linked lists of functions, after_idle listener
#include "midi.h"		// midi interface
#include "define.h"		// define MIDI note, ctrl, and nrpn flags FIX.ME rename midi_defs.h

static int codec_identify(void); // forward for nprn.h

#include "nrpn.h"		// default values and listeners for nrpns
#include "string.h"		// string handler

#include "audio.h"		// audio graph
#undef READ_ONLY		// trash from the audio library includes
#include "codec.h"		// handle the audio hardware
#include "levels.h"		// level computations

#include "cwmorse.h"		// morse code table
#include "cwdecode.h"		// decode cw element stream -> ascii
#include "cwdetime.h"		// detime cw key line -> element stream
#include "cwroute.h"		// route notes into and out of the keyer
#include "cwptt.h"		// translate sidetone key -> tx ptt and key
#include "cwstptt.h"		// translate sidetone key -> sidetone ptt
#include "cwarbiter.h"		// arbitrate multiple side tone key lines
#include "cwkey_straight.h"	// hardware switch -> straight key sidetone
#include "cwkey_paddle.h"	// hardare paddle -> iambic keyed sidetone
#include "cwkey_text.h"		// ascii characters -> mechanically keyed sidetone

#include "padc.h"		// input adc states -> input adc nrpns
#include "padcmap.h"		// input adc nrpns -> keyer parameter nrpns

#include "pin.h"		// input pin states -> input pin notes rename
#include "pout.h"		// output pin notes -> output pins

#include "led.h"		// 

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
  au_st_key.send(note_get(NOTE_AU_ST_KEY));
  au_key_out.send(note_get(NOTE_AU_IQ_KEY));
  // maintain timing information
  timing_isrCpuCyclesRaw += ARM_DWT_CYCCNT - cycles;
  timing_sampleCounter += 1;
}

void setup(void) {
#if KYR_SERIAL_ENABLE
  Serial.begin(115200);
  while ( ! Serial);
  Serial.printf("hasak.ino setup()\n");
#endif
  listener_setup();		// set up listener free list
  midi_setup();			// initialize the midi interface
  define_setup();		// define our notes ctrls and nrpns
  nrpn_setup();			// set up nrpn handlers
  string_setup();		// set up string handlers
  audio_setup();		// set up audio handlers
  codec_setup();		// let the code install handlers
  codec_enable();		// enable the codec
  timing_setup();		// start your timers
  padc_setup();			// nrpn/note handlers for analog pins
  padcmap_setup();		// mapping adc values into parameters
  pin_setup();			// nrpn/note handlers for digital inputs
  pout_setup();			// nrpn/note handlers for digital outputs
  levels_setup();		// nrpn/note handlers for level setting
  cwkey_straight_setup();	// straight key keyer(s)
  cwkey_paddle_setup();		// paddle keyer(s)
  cwkey_text_setup();		// nrpn/note handlers for text keyers
  cwarbiter_setup();		// arbitrate who gets the key
  cwstptt_setup();		// simple sidetone ptt signal
  cwptt_setup();		// full cw ptt generation
  cwroute_setup();		// note handlers to route cw notes into and out of
  cwdetime_setup();		// incomplete sidetone detimer into elements
  cwdecode_setup();		// very incomplete sidetone element decoder
  led_setup();
  // cwinkey_setup();		// winkey emulation
  diagnostics_setup();		// diagnostics console

  //
  // now start
  // before this point setting parameters with nrpn_set()
  // might not work correctly, because they might not trigger
  // the correct handlers.  
  // The *_setup functions set a lot of listeners to implement parameters.
  //

  nrpn_set_default();		// set the default parameters

  /* FIX.ME - shouldn't this go into audio.h - audio_setup() */
  AudioMemory(40);

  /*  interval timer to latch inputs into the audio update loop. */
  static IntervalTimer timer;
  // timer.priority(96);
  timer.begin(interrupt, 1e6/AUDIO_SAMPLE_RATE_EXACT);
}

/*
** loop calls the loop function for each module
** but many of those functions are empty.
*/
void loop(void) {
  timing_loop();		// accumulate counts
  midi_loop();			// drain midi input

  // counter updates
  
  nrpn_incr(NRPN_LOOP);		   // update loop counter

  { static elapsedSamples counter; // update sample counter
    if ((int)counter >= 0) { counter = -1; nrpn_incr(NRPN_SAMPLE); } }

  { static elapsedUpdates counter; // update buffer update counter
    if ((int)counter >= 0) { counter = -1; nrpn_incr(NRPN_UPDATE); } }

  { static elapsedMillis counter; // update millisecond counter
    if ((int)counter >= 0) { counter = -1; nrpn_incr(NRPN_MILLI); } }

  { static elapsedMillis counter; // update 10 millisecond counter
    if ((int)counter >= 0) { counter = -10; nrpn_incr(NRPN_10MILLI); } }

  { static elapsedMillis counter; // update 100 millisecond counter
    if ((int)counter >= 0) { counter = -100; nrpn_incr(NRPN_100MILLI); } }

  listener_loop();		// after idle dispatch
  diagnostics_loop();		// console
}


