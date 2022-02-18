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

/* valid teensy pin number, from teensy core sources */
static int pin_valid(int pin) { return (unsigned)pin < (unsigned)CORE_NUM_TOTAL_PINS; }

static void midi_send_nrpn(const int16_t nrpn, const int16_t value);
static void note_set(const int16_t note, const int8_t value);

#include "config.h"		// configuration
#include "convert.h"
#include "timing.h"		// timing 
#include "midi.h"
#include "note.h"
#include "ctrl.h"
#include "nrpn.h"
#include "audio.h"

#include "codec.h"
// #include "nrpn.h"
// #include "midi.h"
// #include "note.h"
#include "cwptt.h"
#include "arbiter2.h"
#include "keyer/keyer.h"
#include "input.h"
#include "inpin.h"
#include "winkey.h"
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
  uint32_t cycles = ARM_DWT_CYCCNT;
  timing_sampleCount += 1;
  st_key.send(hasak.notes[KYRN_KEY_ST]);
  tx_key.send(hasak.notes[KYRN_KEY_TX]);
  digitalWriteFast(KYR_KEY_OUT_PIN,hasak._key_out); // note active high, is pin active high or low?
  digitalWriteFast(KYR_PTT_OUT_PIN,hasak._ptt_out);
  timing_isrCpuCyclesRaw += ARM_DWT_CYCCNT - cycles;
}

void setup(void) {
  Serial.begin(115200);

  pinMode(KYR_KEY_OUT_PIN, OUTPUT); digitalWrite(KYR_KEY_OUT_PIN, 1);
  pinMode(KYR_PTT_OUT_PIN, OUTPUT); digitalWrite(KYR_PTT_OUT_PIN, 1);

  AudioMemory(40);
  nrpn_setup();

  /* We want to sample the switch inputs at the sample rate.
   * We can do that by attaching a pin interrupt to the LRCLK
   * pin.  But that doesn't work on the Teensy4, so we jumper
   * the LRCLK pin to another pin and attach a pin interrupt
   * to the second pin.  But that doesn't work if people don't
   * want to hack on their Teensys, so we can also use an
   * interval timer.
   */
  static IntervalTimer timer;
  timer.priority(96);
  timer.begin(interrupt, 1e6/AUDIO_SAMPLE_RATE_EXACT);

  midi_setup();
  winkey_setup();
  input_setup();
  inpin_setup();
  note_setup();
  keyer_setup();
  arbiter2_setup();
  cwptt_setup();
  diagnostics_setup();
}

void loop(void) {
  timing_loop();		// accumulate counts
  midi_loop();			// drain midi input
  inpin_loop();			// poll digital input pins
  input_loop();			// poll analog input pins
  note_loop();			// note manager
  keyer_loop();			// keyer events
  arbiter2_loop();		// arbitration of keyer events
  cwptt_loop();			// key generated ptt
  winkey_loop();		// winkey
  nrpn_loop();			// nrpn management 
  diagnostics_loop();		// console
}


