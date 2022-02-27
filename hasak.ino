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

//static int pin_valid(int pin) { return PIN_VALID(pin); }
//static int pin_analog(int pin) { return PIN_ANALOG(pin); }
//static int pin_i2s(int pin) { return PIN_I2S(pin); }
//static int pin_i2c(int pin) { return PIN_I2C(pin); }
//static int pin_nothing(void) { return 127; }

#include "config.h"		// configuration
#include "cwmorse.h"		// morse code table
#include "linkage.h"		// forward references, linkage, and conversions
#include "listener.h"		// linked lists of functions, after_idle listener
#include "audio.h"		// audio graph
#undef READ_ONLY		// trash from the audio library includes
#include "timing.h"		// timing counters
#include "midi.h"		// midi interface
#include "define.h"		// define MIDI note, ctrl, and nrpn flags FIX.ME rename midi_defs.h
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
#include "adc.h"		// input adc states -> input adc nrpns FIX.ME rename inadc.h
#include "adcmap.h"		// input adc nrpns -> keyer parameter nrpns
#include "pin.h"		// input pin states -> input pin notes rename
#include "pout.h"		// output pin notes -> output pins
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
  while ( ! Serial);
  // nrpn_set_default(KYRP_SET_DEFAULT);
  // nrpn_set(KYRP_VOLUME, -200);

  Serial.printf("before listener_setup()\n");
  listener_setup();
  Serial.printf("before midi_setup()\n");
  midi_setup();
  Serial.printf("before define_setup()\n");
  define_setup();
  Serial.printf("before codec_setup()\n");
  codec_setup();
  Serial.printf("before codec_enable()\n");
  codec_enable();
  Serial.printf("before nrpn_setup()\n");
  nrpn_setup();
  Serial.printf("before timing_setup()\n");
  timing_setup();		// start your timers
  Serial.printf("before adc_setup()\n");
  adc_setup();
  Serial.printf("before adcmap_setup()\n");
  adcmap_setup();
  Serial.printf("before pin_setup()\n");
  pin_setup();
  Serial.printf("before pout_setup()\n");
  pout_setup();
  Serial.printf("before cwkey_straight_setup()\n");
  cwkey_straight_setup();
  Serial.printf("before cwkey_paddle_setup()\n");
  cwkey_paddle_setup();
  Serial.printf("before cwkey_text_setup()\n");
  cwkey_text_setup();
  Serial.printf("before cwarbiter_setup()\n");
  cwarbiter_setup();
  Serial.printf("before cwstptt_setup()\n");
  cwstptt_setup();
  Serial.printf("before cwptt_setup()\n");
  cwptt_setup();
  Serial.printf("before cwroute_setup()\n");
  cwroute_setup();
  Serial.printf("before cwdetime_setup()\n");
  cwdetime_setup();
  Serial.printf("before cwdecode_setup()\n");
  cwdecode_setup();
  // cwinkey_setup();
  Serial.printf("before diagnostics_setup()\n");
  diagnostics_setup();

  //
  // now start
  //

  Serial.printf("before nrpn_set_default()\n");
  nrpn_set_default();
  /* FIX.ME - these go in audio.h - audio_setup() */
  Serial.printf("before AudioMemory(40)\n");
  AudioMemory(40);
  /*  interval timer to latch inputs into the audio update loop. */
  static IntervalTimer timer;
  timer.priority(96);
  Serial.printf("before timer.begin\n");
  timer.begin(interrupt, 1e6/AUDIO_SAMPLE_RATE_EXACT);
}

/*
** loop calls the loop function for each module
** but many of those functions are empty.
*/
void loop(void) {
  timing_loop();		// accumulate counts
  midi_loop();			// drain midi input
  // codec_loop();
  // define_loop();
  // nrpn_loop();
  // pout_loop();
  pin_loop();
  adc_loop();
  // adcmap_loop();
  // cwkey_straight_loop();
  // cwkey_paddle_loop();
  // cwkey_text_loop();
  // cwarbiter_loop();		// arbitration of keyer events
  // cwstptt_loop();			// probably
  // cwptt_loop();			// key generated ptt
  // cwroute_loop();
  // cwdetime_loop();
  // cwdecode_loop();
  // cwwinkey_loop();		// winkey
  static elapsedSamples counter;
  if ((int)counter >= 0) {
    counter = -1;
    // cwkey_straight_sample();
    cwkey_paddle_sample();
    cwkey_text_sample();
    cwarbiter_sample();
    cwstptt_sample();
    cwptt_sample();
    // cwroute_sample();
    // cwdetime_sample();
    // cwdecode_sample();
  }
  listener_loop();
  diagnostics_loop();		// console
}


