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
** crossing over from old to new
*/
//#define OLD_AUDIO_GRAPH 1
#define NEW_AUDIO_GRAPH 1

/*
** config.h contains preprocessor defined constants.
** linkage.h contains functions that fetch parameters.
*/
#include "config.h"		// configuration
#include <Arduino.h>
#include "linkage.h"

/***************************************************************
** Audio library
***************************************************************/

#include <Audio.h>
// custom audio modules
#include "src/Audio/input_byte.h"
#include "src/Audio/synth_keyed_tone.h"
#include "src/Audio/output_byte.h"

// audio sample values
#include "src/Audio/sample_value.h"

#include <Wire.h>

// Audio Library program, written by hand

// inputs
AudioInputUSB           usb_in;	// usb audio in
AudioInputI2S           i2s_in;	// i2s audio in
#if defined(KYRC_ENABLE_ADC_IN)
AudioInputAnalog	adc_in;	// headset switch detect
#endif

// revised inputs
AudioInputByte		st_key;		  // sidetone key line
AudioInputByte		tx_key;		  // transmitter key line

// shaped key waveform
AudioSynthKeyedTone	tone_ramp(1); // one channel sidetone ramp
AudioSynthKeyedTone	key_ramp(2);  // two channel IQ tone ramp
AudioConnection		patchCord3a(st_key, 0, tone_ramp, 0); // sidetone key line
AudioConnection		patchCord3b(tx_key, 1, key_ramp, 0);  // transmit key line

// 
// output mixers
// channel 0 rx_input from usb_in or mic_input from i2s_in
// 
AudioMixer4              l_usb_out;
AudioMixer4              r_usb_out;
AudioMixer4              l_i2s_out;
AudioMixer4              r_i2s_out;
AudioMixer4              l_hdw_out;
AudioMixer4              r_hdw_out;
// output mixer pointers in order of output mixer nrpns
AudioMixer4		*mix_out[6] = {
  &l_usb_out, &r_usb_out, &l_i2s_out, &r_i2s_out, &l_hdw_out, &r_hdw_out
};

// first channel, rx audio and microphone/line-in input, op on headset mode
// switch codec to use microphone instead of line-in
AudioConnection		patchCord900(i2s_in, 0, l_usb_out, 0);
AudioConnection		patchCord910(i2s_in, 1, r_usb_out, 0);
AudioConnection		patchCord920(usb_in, 0, l_i2s_out, 0);
AudioConnection		patchCord930(usb_in, 1, r_i2s_out, 0);
AudioConnection		patchCord940(usb_in, 0, l_hdw_out, 0);
AudioConnection		patchCord950(usb_in, 1, r_hdw_out, 0);

// second channel, keyed sidetone
// probably only sent to i2s for headphones/speakers
// balance l vs r to localize
// adjust level of sidetone
AudioConnection		patchCord901(tone_ramp, 0, l_usb_out, 1);
AudioConnection		patchCord911(tone_ramp, 0, r_usb_out, 1);
AudioConnection		patchCord921(tone_ramp, 0, l_i2s_out, 1);
AudioConnection		patchCord931(tone_ramp, 0, r_i2s_out, 1);
AudioConnection		patchCord941(tone_ramp, 0, l_hdw_out, 1);
AudioConnection		patchCord951(tone_ramp, 0, r_hdw_out, 1);

// third channel, keyed IQ oscillator, 
// send to usb_out to provide soundcard IQ TX stream to host sdr
// send to i2s_out to provide soundcard IQ TX stream to softrock
// also used for diagnostics when iq is disabled
AudioConnection		patchCord902(key_ramp, 0, l_usb_out, 2);
AudioConnection		patchCord912(key_ramp, 1, r_usb_out, 2);
AudioConnection		patchCord922(key_ramp, 0, l_i2s_out, 2);
AudioConnection		patchCord932(key_ramp, 1, r_i2s_out, 2);
AudioConnection		patchCord942(key_ramp, 0, l_hdw_out, 2);
AudioConnection		patchCord952(key_ramp, 1, r_hdw_out, 2);

// fourth channel, loopback
// send usb_in to usb_out, i2s_in to i2s_out and hdw_out
AudioConnection		patchCord903(usb_in, 0, l_usb_out, 3);
AudioConnection		patchCord913(usb_in, 1, r_usb_out, 3);
AudioConnection		patchCord923(i2s_in, 0, l_i2s_out, 3);
AudioConnection		patchCord933(i2s_in, 1, r_i2s_out, 3);
AudioConnection		patchCord943(i2s_in, 0, l_hdw_out, 3);
AudioConnection		patchCord953(i2s_in, 1, r_hdw_out, 3);

AudioOutputUSB          usb_out;
AudioConnection		patchCord11a(l_usb_out, 0, usb_out, 0);
AudioConnection		patchCord11b(r_usb_out, 0, usb_out, 1);

AudioOutputI2S          i2s_out;
AudioConnection		patchCord11c(l_i2s_out, 0, i2s_out, 0);
AudioConnection		patchCord11d(r_i2s_out, 0, i2s_out, 1);

#if defined(KYRC_ENABLE_HDW_OUT)
#if defined(TEENSY31) || defined(TEENSY32)
AudioOutputAnalog	hdw_out;
AudioConnection		patchCord11e(l_hdw_out, 0, hdw_out, 0);

#elif defined(TEENSY35) || defined(TEENSY36)
AudioOutputAnalogStereo	hdw_out;
AudioConnection		patchCord11e(l_hdw_out, 0, hdw_out, 0);
AudioConnection		patchCord11f(r_hdw_out, 0, hdw_out, 1);

#elif defined(TEENSY40) || defined(TEENSY41)
AudioOutputMQS		hdw_out;
AudioConnection		patchCord11e(l_hdw_out, 0, hdw_out, 0);
AudioConnection		patchCord11f(r_hdw_out, 0, hdw_out, 1);
#endif
#endif

// codec control
AudioControlSGTL5000     sgtl5000; // the codec in the teensy audio shield
AudioControlWM8960	   wm8960; // the codec in the CWKeyer shield

/*
** core of program.
*/

/*
** keep track of cycles during each sample buffer time
** used to normalize the cycle usage from the audio library
** also track the cycle usage in the pollatch interrupt routine.
*/
elapsedMicros perAudioBuffer;	// counts up microseconds
uint32_t microsPerAudioBuffer = AUDIO_BLOCK_SAMPLES * 1000000.0 / AUDIO_SAMPLE_RATE;
uint32_t cpuCyclesRaw, cpuCyclesPerAudioBuffer, cpuCyclesPerAudioBufferMax;
uint32_t isrCpuCyclesRaw, isrCyclesPerAudioBuffer, isrCyclesPerAudioBufferMax;

static void timing_loop(void) {
  if (perAudioBuffer >= microsPerAudioBuffer) {
    perAudioBuffer = 0;
    cpuCyclesPerAudioBuffer = (ARM_DWT_CYCCNT - cpuCyclesRaw) >> 6;
    cpuCyclesRaw = ARM_DWT_CYCCNT;
    // disable interrupts
    isrCyclesPerAudioBuffer = isrCpuCyclesRaw >> 6;
    isrCpuCyclesRaw = 0;
    // enable interupts
    cpuCyclesPerAudioBufferMax = max(cpuCyclesPerAudioBufferMax, cpuCyclesPerAudioBuffer);
    isrCyclesPerAudioBufferMax = max(isrCyclesPerAudioBuffer, isrCyclesPerAudioBufferMax);
    // this discards an early transient
    if (cpuCyclesPerAudioBufferMax > 100000) cpuCyclesPerAudioBufferMax = cpuCyclesPerAudioBuffer;
  }
}

static float timing_percent(uint32_t cpuCycles) {
  return 100*(float)cpuCycles/(float)cpuCyclesPerAudioBuffer;
}

static float timing_percent_max(uint32_t cpuCyclesMax) {
  return 100*(float)cpuCyclesMax/(float)cpuCyclesPerAudioBufferMax;
}

/*
** forward references.
*/
/* valid teensy pin number, from teensy core sources */
static int valid_pin(int pin) { return pin >= 0 && pin < CORE_NUM_TOTAL_PINS; }

// static void codec_enable(void);
// static int16_t codec_identify(void);
// static void codec_nrpn_set(const int16_t nrpn, const int16_t value);
static void midi_send_nrpn(const int16_t nrpn, const int16_t value);
static void note_set(const int16_t note, const int8_t value);

#include "codec.h"
#include "nrpn.h"
#include "midi.h"
#include "note.h"
#include "keyptt.h"
#include "arbiter2.h"
#include "keyer_text.h"
#include "keyer_paddle.h"
#include "keyer.h"
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
  hasak.sampleCount += 1;
  st_key.send(hasak.notes[KYRN_KEY_ST]);
  tx_key.send(hasak.notes[KYRN_KEY_TX]);
  digitalWriteFast(KYR_KEY_OUT_PIN,hasak._key_out); // note active high, is pin active high or low?
  digitalWriteFast(KYR_PTT_OUT_PIN,hasak._ptt_out);
  isrCpuCyclesRaw += ARM_DWT_CYCCNT - cycles;
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
  keyer_paddle_setup();
  keyer_text_setup();
  arbiter2_setup();
  keyptt_setup();
  diagnostics_setup();
}

void loop(void) {
  hasak.loopCount += 1;		// count loops for statistics
  timing_loop();		// accumulate cycle count to normalize audio library usage
  midi_loop();			// drain midi input
  inpin_loop();			// poll digital input pins
  input_loop();			// poll analog input pins
  note_loop();			// note manager
  keyer_loop();			// keyer events
  keyer_paddle_loop();		// paddle keyers
  keyer_text_loop();		// text keyers
  arbiter2_loop();		// arbitration of keyer events
  keyptt_loop();		// key generated ptt
  winkey_loop();		// winkey
  nrpn_loop();			// nrpn management 
  diagnostics_loop();		// console
}


