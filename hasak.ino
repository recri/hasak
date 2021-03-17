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
#include "src/Audio/input_sample.h"
#include "src/Audio/input_text.h"
#include "src/Audio/effect_paddle.h"
#include "src/Audio/effect_arbiter.h"
#include "src/Audio/synth_keyed_tone.h"
#include "src/Audio/effect_mute.h"
#include "src/Audio/output_sample.h"
#include "src/Audio/my_control_sgtl5000.h"
// audio sample values
#include "src/Audio/sample_value.h"

#include <Wire.h>

// Audio Library program, written by hand

// inputs
AudioInputUSB           usb_in;	// usb audio in
AudioInputI2S           i2s_in;	// i2s audio in
AudioInputAnalog	adc_in;	// headset switch detect
AudioInputSample	l_pad("l_pad");	// left paddle in
AudioInputSample	r_pad("r_pad");	// right paddle in
AudioInputSample	s_key("s_key");	// straight key in
AudioInputText		wink(KYR_VOX_WINK);	// winkey text in
AudioInputText		kyr(KYR_VOX_KYR);	// kyr text in for op
AudioInputSample        ptt_sw("ptt_s");	// ptt switch
//AudioInputSample	tx_enable("tx_en"); // transmit enable
//AudioInputSample	st_enable("st_en"); // side tone enable

// paddle keyer logic
AudioEffectPaddle	paddle(KYR_VOX_PAD);	// 
AudioConnection         patchCord1a(l_pad, 0, paddle, 0);
AudioConnection         patchCord1b(r_pad, 0, paddle, 1);

// arbitration
AudioEffectArbiter	arbiter;
AudioConnection		patchCord2b(s_key, 0, arbiter, 0);
AudioConnection		patchCord2a(paddle, 0, arbiter, 1);
AudioConnection		patchCord2c(wink, 0, arbiter, 2);
AudioConnection		patchCord2d(kyr, 0, arbiter, 3);
static void arbiter_setup(void) {
  // define the voices the arbiter sees on input channels
  // changing to let their identity be their priority
  arbiter.define_vox(0, KYR_VOX_S_KEY, KYR_VOX_S_KEY, 0);
  arbiter.define_vox(1, KYR_VOX_PAD, KYR_VOX_PAD, 0);
  arbiter.define_vox(2, KYR_VOX_WINK, KYR_VOX_WINK, 0);
  arbiter.define_vox(3, KYR_VOX_KYR, KYR_VOX_KYR, 1);
}
int get_active_vox(void) { return arbiter.get_active_vox(); }

// shaped key waveform
AudioSynthKeyedTone	tone_ramp(1);
AudioSynthKeyedTone	key_ramp(2);
AudioConnection		patchCord3a(arbiter, 0, tone_ramp, 0); // sidetone key line
AudioConnection		patchCord3b(arbiter, 1, key_ramp, 0);  // transmit key line

// microphone mute, disabled when ptt switch is pressed
//AudioEffectMute		mic_mute;
//AudioEffectMultiply	l_mic_mute;
//AudioEffectMultiply	r_mic_mute;
//AudioConnection		patchCord4a(ptt_sw, 0, mic_mute, 0);
//AudioConnection		patchCord4b(mic_mute, 0, l_mic_mute, 0);
//AudioConnection		patchCord4c(mic_mute, 0, r_mic_mute, 0);
//AudioConnection		patchCord4d(i2s_in, 0, l_mic_mute, 1);
//AudioConnection		patchCord4e(i2s_in, 1, r_mic_mute, 1);

// receive mute, enabled when arbiter vox line is active
//AudioEffectMute		rx_mute;
//AudioEffectMultiply	l_rx_mute;
//AudioEffectMultiply	r_rx_mute;
//AudioConnection		patchCord6a(arbiter, 1, rx_mute, 0);
//AudioConnection		patchCord6b(rx_mute, 0, l_rx_mute, 0);
//AudioConnection		patchCord6c(rx_mute, 0, r_rx_mute, 0);
//AudioConnection		patchCord6d(usb_in, 0, l_rx_mute, 1);
//AudioConnection		patchCord6e(usb_in, 1, r_rx_mute, 1);

// 
// output mixers
// channel 0 rx_input from usb_in or mic_input from i2s_in
// 
AudioMixer4              l_i2s_out;
AudioMixer4              r_i2s_out;
AudioMixer4              l_usb_out;
AudioMixer4              r_usb_out;
AudioMixer4              l_hdw_out;
AudioMixer4              r_hdw_out;
static void mixer_setup(float gain=1.0) {
  // set everything off
  for (int i = 0; i < 4; i += 1) {
    l_i2s_out.gain(i, 0);
    r_i2s_out.gain(i, 0);
    l_usb_out.gain(i, 0);
    r_usb_out.gain(i, 0);
    l_hdw_out.gain(i, 0);
    r_hdw_out.gain(i, 0);
  }
  // mix rx_in and sidetone to headphones
  for (int i = 0; i <= 2; i += 2) {
    l_i2s_out.gain(i, gain);
    r_i2s_out.gain(i, gain);
    l_hdw_out.gain(i, gain);
    r_hdw_out.gain(i, gain);
  }
  // keyed IQ to usb_out
  l_usb_out.gain(3, gain);
  r_usb_out.gain(3, gain);
  // temporary test: sidetone on l_usb_out, iq on r_usb_out, to see ppt_head.
  // l_usb_out should lead r_usb_out by head_time, but doesn't
  // l_usb_out.gain(2, gain);
  // l_usb_out.gain(3, 0);
}

// first channel, rx audio and microphone/line-in input, op on headset mode
// switch codec to use microphone instead of line-in
//AudioConnection		patchCord900(l_rx_mute, 0, l_i2s_out, 0);
//AudioConnection		patchCord910(r_rx_mute, 0, r_i2s_out, 0);
//AudioConnection		patchCord920(l_mic_mute, 0, l_usb_out, 0);
//AudioConnection		patchCord930(r_mic_mute, 0, r_usb_out, 0);
//AudioConnection		patchCord940(l_rx_mute, 0, l_hdw_out, 0);
//AudioConnection		patchCord950(r_rx_mute, 0, r_hdw_out, 0);
AudioConnection		patchCord900(usb_in, 0, l_i2s_out, 0);
AudioConnection		patchCord910(usb_in, 1, r_i2s_out, 0);
AudioConnection		patchCord920(i2s_in, 0, l_usb_out, 0);
AudioConnection		patchCord930(i2s_in, 1, r_usb_out, 0);
AudioConnection		patchCord940(usb_in, 0, l_hdw_out, 0);
AudioConnection		patchCord950(usb_in, 1, r_hdw_out, 0);

// second channel, line-out audio and line-in audio, 2x2 sound card mode
// switch codec to use line-in instead of microphone and line-out instead
// of headphones, can still use keyer on MQS output.
// same as the first, should be dropped
AudioConnection		patchCord901(usb_in, 0, l_i2s_out, 1);
AudioConnection		patchCord911(usb_in, 1, r_i2s_out, 1);
AudioConnection		patchCord921(i2s_in, 0, l_usb_out, 1);
AudioConnection		patchCord931(i2s_in, 1, r_usb_out, 1);
AudioConnection		patchCord941(usb_in, 0, l_hdw_out, 1);
AudioConnection		patchCord951(usb_in, 1, r_hdw_out, 1);

// third channel, sidetone
// probably only sent to i2s for headphones/speakers
// balance l vs r to localize
// adjust level of sidetone
AudioConnection		patchCord902(tone_ramp, 0, l_i2s_out, 2);
AudioConnection		patchCord912(tone_ramp, 0, r_i2s_out, 2);
AudioConnection		patchCord922(tone_ramp, 0, l_usb_out, 2);
AudioConnection		patchCord932(tone_ramp, 0, r_usb_out, 2);
AudioConnection		patchCord942(tone_ramp, 0, l_hdw_out, 2);
AudioConnection		patchCord952(tone_ramp, 0, r_hdw_out, 2);

// fourth channel, keyed IQ, 
// send to usb_out to provide soundcard IQ TX stream to host sdr
// send to i2s_out to provide soundcard IQ TX stream to softrock
// also used for diagnostics when iq is disabled
AudioConnection		patchCord903(key_ramp, 0, l_i2s_out, 3);
AudioConnection		patchCord913(key_ramp, 1, r_i2s_out, 3);
AudioConnection		patchCord923(key_ramp, 0, l_usb_out, 3);
AudioConnection		patchCord933(key_ramp, 1, r_usb_out, 3);
AudioConnection		patchCord943(key_ramp, 0, l_hdw_out, 3);
AudioConnection		patchCord953(key_ramp, 1, r_hdw_out, 3);

// outputs
AudioOutputSample	key_out;
AudioOutputSample	ptt_out;

AudioConnection		patchCord10a(arbiter, 1, key_out, 0);
AudioConnection		patchCord10b(arbiter, 2, ptt_out, 0);

AudioOutputSample	adc_out;
AudioConnection		patchCord10c(adc_in, 0, adc_out, 0);

AudioOutputUSB          usb_out;
AudioConnection		patchCord11a(l_usb_out, 0, usb_out, 0);
AudioConnection		patchCord11b(r_usb_out, 0, usb_out, 1);

AudioOutputI2S          i2s_out;
AudioConnection		patchCord11c(l_i2s_out, 0, i2s_out, 0);
AudioConnection		patchCord11d(r_i2s_out, 0, i2s_out, 1);

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

// codec control
MyAudioControlSGTL5000     sgtl5000;

/*
** core of program.
*/

//
// poll input pins at sample rate
// latch output pins at sample rate
// also poll/latch arbitrary memory locations
// also combine latch output pins and memory locations
// should move the bool2fix/fix2bool into the audio
// loop which runs at lower priority than this
// interrupt handler.
//
int16_t _adc_out;
uint8_t _key_out, _ptt_out;

static void pollatch() {
  l_pad.send(bool2fix(digitalRead(KYR_L_PAD_PIN)^1));
  r_pad.send(bool2fix(digitalRead(KYR_R_PAD_PIN)^1));
  s_key.send(bool2fix(digitalRead(KYR_S_KEY_PIN)^1));
  ptt_sw.send(bool2fix(digitalRead(KYR_PTT_SW_PIN)^1));
  digitalWrite(KYR_KEY_OUT_PIN,_key_out=fix2bool(key_out.recv()));
  digitalWrite(KYR_PTT_OUT_PIN,_ptt_out=fix2bool(ptt_out.recv()));
  _adc_out = adc_out.recv();
}

/*
** forward references.
*/
static void nrpn_setup(void);
static void midi_setup(void);
static void midi_loop(void);
static void winkey_setup(void);
static void winkey_loop(void);
static void diagnostics_loop(void);

void setup(void) {
  Serial.begin(115200);
  pinMode(KYR_L_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_R_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_S_KEY_PIN, INPUT_PULLUP);
  pinMode(KYR_PTT_SW_PIN, INPUT_PULLUP);
  pinMode(KYR_KEY_OUT_PIN, OUTPUT); digitalWrite(KYR_KEY_OUT_PIN, 1);
  pinMode(KYR_PTT_OUT_PIN, OUTPUT); digitalWrite(KYR_PTT_OUT_PIN, 1);
#ifdef KYR_DUP_LRCLK
  pinMode(KYR_DUP_LRCLK, INPUT);
#endif
  arbiter_setup();
  mixer_setup();
  AudioMemory(40);
  nrpn_setup();
#ifdef KYR_DUP_LRCLK
  attachInterrupt(KYR_DUP_LRCLK, pollatch, RISING);
#else
  attachInterrupt(KYR_LRCLK, pollatch, RISING);
#endif
  midi_setup();
  winkey_setup();
}

void loop(void) {
  midi_loop();
  winkey_loop();
  diagnostics_loop();
}

#include "nrpn.h"
#include "midi.h"
#include "winkey.h"
#include "diagnostics.h"
