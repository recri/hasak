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
#include "src/Audio/effect_and_not.h"
#include "src/Audio/effect_ptt_delay.h"
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
AudioInputSample	tx_enable("tx_en"); // transmit enable
AudioInputSample	st_enable("st_en"); // side tone enable

// keyer logic
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
  arbiter.define_vox(0, KYR_VOX_S_KEY, KYR_VOX_S_KEY);
  arbiter.define_vox(1, KYR_VOX_PAD, KYR_VOX_PAD);
  arbiter.define_vox(2, KYR_VOX_WINK, KYR_VOX_WINK);
  arbiter.define_vox(3, KYR_VOX_KYR, KYR_VOX_KYR);
}
int get_active_vox(void) { return arbiter.get_active_vox(); }

// shaped key waveform
AudioSynthKeyedTone	key_ramp;
AudioConnection		patchCord3a(arbiter, 0, key_ramp, 0);

// tx enable
AudioEffectAndNot	and_not_kyr(KYR_VOX_KYR);
AudioConnection		patchCord3f(tx_enable, 0, and_not_kyr, 0); // tx enable if active vox != KYR_VOX_KYR

// microphone mute, disabled when ptt switch is pressed
//AudioEffectMute		mic_mute;
//AudioEffectMultiply	l_mic_mute;
//AudioEffectMultiply	r_mic_mute;
//AudioConnection		patchCord4a(ptt_sw, 0, mic_mute, 0);
//AudioConnection		patchCord4b(mic_mute, 0, l_mic_mute, 0);
//AudioConnection		patchCord4c(mic_mute, 0, r_mic_mute, 0);
//AudioConnection		patchCord4d(i2s_in, 0, l_mic_mute, 1);
//AudioConnection		patchCord4e(i2s_in, 1, r_mic_mute, 1);

// IQ mute, disabled when tx_enable and active_vox is not kyr
//AudioEffectMute		IQ_mute;
//AudioEffectMultiply	I_mute;
//AudioEffectMultiply	Q_mute;
//AudioConnection		patchCord5a(and_not_kyr, 0, IQ_mute, 0);
//AudioConnection		patchCord5b(IQ_mute, 0, I_mute, 0);
//AudioConnection		patchCord5c(IQ_mute, 0, Q_mute, 0);
//AudioConnection		patchCord5d(key_ramp, 0, I_mute, 1);
//AudioConnection		patchCord5e(key_ramp, 1, Q_mute, 1);

// receive mute, enabled when arbiter vox line is active
//AudioEffectMute		rx_mute;
//AudioEffectMultiply	l_rx_mute;
//AudioEffectMultiply	r_rx_mute;
//AudioConnection		patchCord6a(arbiter, 1, rx_mute, 0);
//AudioConnection		patchCord6b(rx_mute, 0, l_rx_mute, 0);
//AudioConnection		patchCord6c(rx_mute, 0, r_rx_mute, 0);
//AudioConnection		patchCord6d(usb_in, 0, l_rx_mute, 1);
//AudioConnection		patchCord6e(usb_in, 1, r_rx_mute, 1);

// sidetone mute, disabled when sidetone enabled is true
//AudioEffectMute		st_mute;
//AudioEffectMultiply	l_st_mute;
//AudioEffectMultiply	r_st_mute;
//AudioConnection		patchCord7a(st_enable, 0, st_mute, 0);
//AudioConnection		patchCord7b(st_mute, 0, l_st_mute, 0);
//AudioConnection		patchCord7c(st_mute, 0, r_st_mute, 0);
//AudioConnection		patchCord7d(key_ramp, 0, l_st_mute, 1);
//AudioConnection		patchCord7e(key_ramp, 0, r_st_mute, 1);

// ptt delay, give ptt a head start over key to external transmitter
AudioEffectPTTDelay	ptt_delay;
AudioConnection		patchCord8a(arbiter, 0, ptt_delay, 0); // arbiter key -> ptt_delay 0
AudioConnection		patchCord8b(arbiter, 1, ptt_delay, 1); // arbiter vox -> ptt_delay 1
AudioConnection		patchCord8c(and_not_kyr, 0, ptt_delay, 2); // tx really enabled to ptt_delay 2

// 
// output mixers
AudioMixer4              l_i2s_out;
AudioMixer4              r_i2s_out;
AudioMixer4              l_usb_out;
AudioMixer4              r_usb_out;
AudioMixer4              l_hdw_out;
AudioMixer4              r_hdw_out;
static void mixer_setup(void) {
  float gain = 1.0;
  for (int i = 0; i < 4; i += 1) {
    l_i2s_out.gain(i, gain);
    r_i2s_out.gain(i, gain);
    l_usb_out.gain(i, gain);
    r_usb_out.gain(i, gain);
    l_hdw_out.gain(i, gain);
    r_hdw_out.gain(i, gain);
  }
}
// first channel, rx audio and microphone input, op on headset mode
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
//AudioConnection		patchCord902(l_st_mute, 0, l_i2s_out, 2);
//AudioConnection		patchCord912(r_st_mute, 0, r_i2s_out, 2);
//AudioConnection		patchCord922(l_st_mute, 0, l_usb_out, 2);
//AudioConnection		patchCord932(r_st_mute, 0, r_usb_out, 2);
//AudioConnection		patchCord942(l_st_mute, 0, l_hdw_out, 2);
//AudioConnection		patchCord952(r_st_mute, 0, r_hdw_out, 2);
AudioConnection		patchCord902(key_ramp, 0, l_i2s_out, 2);
AudioConnection		patchCord912(key_ramp, 0, r_i2s_out, 2);
AudioConnection		patchCord922(key_ramp, 0, l_usb_out, 2);
AudioConnection		patchCord932(key_ramp, 0, r_usb_out, 2);
AudioConnection		patchCord942(key_ramp, 0, l_hdw_out, 2);
AudioConnection		patchCord952(key_ramp, 0, r_hdw_out, 2);

// fourth channel, keyed IQ, 
// send to usb_out to provide soundcard IQ TX stream to host sdr
// send to i2s_out to provide soundcard IQ TX stream to softrock
//AudioConnection		patchCord903(I_mute, 0, l_i2s_out, 3);
//AudioConnection		patchCord913(Q_mute, 0, r_i2s_out, 3);
//AudioConnection		patchCord923(I_mute, 0, l_usb_out, 3);
//AudioConnection		patchCord933(Q_mute, 0, r_usb_out, 3);
//AudioConnection		patchCord943(I_mute, 0, l_hdw_out, 3);
//AudioConnection		patchCord953(Q_mute, 0, r_hdw_out, 3);

// temporary diagnostic connection, adc input to r_usb_out
AudioConnection		patchCord999(adc_in, 0, r_usb_out, 3);

// outputs
AudioOutputSample	key_out;
AudioOutputSample	ptt_out;

AudioConnection		patchCord10a(ptt_delay, 0, key_out, 0);
AudioConnection		patchCord10b(ptt_delay, 1, ptt_out, 0);

AudioOutputSample	adc_out;
AudioConnection		patchCord10c(adc_in, 0, adc_out, 0);

AudioOutputSample	probe1;
AudioOutputSample	probe2;
AudioConnection		patchCord10d(paddle, 0, probe1, 0);
AudioConnection		patchCord10e(arbiter, 0, probe2, 0);

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

// counter to demonstrate that attachInterrupt to LRCLK
// actually produced a sample rate interrupt.
// unsigned long buttonpolls = 0;
//
// poll input pins at sample rate
// latch output pins at sample rate
// also poll/latch arbitrary memory locations
int16_t _adc_out;
int16_t _probe1, _probe2;

static void pollatch() {
  l_pad.send(bool2fix(digitalRead(KYR_L_PAD_PIN)^1));
  r_pad.send(bool2fix(digitalRead(KYR_R_PAD_PIN)^1));
  s_key.send(bool2fix(digitalRead(KYR_S_KEY_PIN)^1));
  ptt_sw.send(bool2fix(digitalRead(KYR_PTT_SW_PIN)^1));
  st_enable.send(bool2fix(get_st_enable()));
  tx_enable.send(bool2fix(get_tx_enable()));
  digitalWrite(KYR_KEY_OUT_PIN,fix2bool(key_out.recv()));
  digitalWrite(KYR_PTT_OUT_PIN,fix2bool(ptt_out.recv()));
  _adc_out = adc_out.recv();
  _probe1 = probe1.recv();
  _probe2 = probe2.recv();
  // use arbiter.get_active_vox() to find active vox
  // _active_vox = active_vox.recv(); // no conversion, int value
  //buttonpolls += 1;
}

/*
** forward references.
*/
static void midi_setup(void);
static void midi_loop(void);
static void winkey_setup(void);
static void winkey_loop(void);
static void nrpn_setup(void);

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
  sgtl5000.enable();
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

#include "diagnostics.h"

void loop(void) {
  midi_loop();
  winkey_loop();
  diagnostics_loop();
}

/***************************************************************
** NRPN (non-registered parameter) handling.
***************************************************************/
/*
** This is where the NRPN's get stored.
*/
int16_t kyr_nrpn[KYRP_LAST];

/* set a nrpn without */
static inline void set_nrpn(int nrpn, int value) {
  if (nrpn >= 0 && nrpn <= KYRP_LAST)
    kyr_nrpn[nrpn] = value;
}

/*
** The idea of set_vox_*(vox, val) and get_vox_*(vox) is that
** keyer voices may have customized parameters, but
** fall back to the common parameter set when no
** customized parameter has been set.
** So set_vox_*() sets the bank of the kyr_nrpn that
** is vox specific, and get_vox_*(vox) looks in the vox
** specific bank and returns its value if it has been
** set, otherwise returns the value from the default bank.
**
** The value that marks a set parameter is kyr_nrpn[INDEX] >= 0
** because nrpn values are 14bit unsigned.
*/

/* set a vox specialized nrpn */
static inline void set_vox_nrpn(int vox, int nrpn, int value) {
  if (vox >= 0 && vox <= KYR_N_VOX && nrpn >= KYRP_KEYER && nrpn < KYRP_VOX_1)
    set_nrpn(nrpn+vox*(KYRP_VOX_1-KYRP_VOX_0), value);
}

//static inline void set_vox_dit(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_DIT, value); }
//static inline void set_vox_dah(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_DAH, value); }
//static inline void set_vox_ies(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_IES, value); }
//static inline void set_vox_ils(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_ILS, value); }
//static inline void set_vox_iws(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_IWS, value); }

/*
** Update the keyer timing, specified by speed, weight, ratio, comp, and farnsworth,
** and produce the samples per dit, dah, ies, ils, and iws.
*/
static void nrpn_update_keyer(uint8_t vox) {
  const float wordDits = 50;
  const float sampleRate = AUDIO_SAMPLE_RATE;
  const float wpm = get_vox_speed(vox);
  const float weight = get_vox_weight(vox);
  const float ratio = get_vox_ratio(vox);
  const float compensation = get_vox_comp(vox);
  const float farnsworth = get_vox_farns(vox);;
  const float ms_per_dit = (1000 * 60) / (wpm * wordDits);
  const float r = (ratio-50)/100.0;
  const float w = (weight-50)/100.0;
  const float c = compensation / 10.0 / ms_per_dit; /* ms/10 / ms_per_dit */
  /* Serial.printf("nrpn_update_keyer sr %f, word %d, wpm %f, weight %d, ratio %d, comp %d, farns %d\n", 
     sampleRate, wordDits, wpm, weight, ratio, compensation, farnsworth); */
  /* Serial.printf("morse_keyer r %f, w %f, c %f\n", r, w, c); */
  /* samples_per_dit = (samples_per_second * second_per_minute) / (words_per_minute * dits_per_word);  */
  const unsigned ticksPerBaseDit = ((sampleRate * 60) / (wpm * wordDits));
  const int16_t ticksPerDit = (1+r+w+c) * ticksPerBaseDit;
  const int16_t ticksPerDah = (3-r+w+c) * ticksPerBaseDit;
  const int16_t ticksPerIes = (1  -w-c) * ticksPerBaseDit;
  int16_t ticksPerIls = (3  -w-c) * ticksPerBaseDit;
  int16_t ticksPerIws = (7  -w-c) * ticksPerBaseDit;
    
  //
  // Farnsworth timing: stretch inter-character and inter-word pauses
  // to reduce wpm to the specified farnsworth speed.
  // formula from https://morsecode.world/international/timing.html
  //
  if (farnsworth > 5 && farnsworth < wpm) {
    float f = 50.0/19.0 * wpm / farnsworth - 31.0/19.0;
    ticksPerIls *= f;		// stretched inter-character pause
    ticksPerIws *= f;		// stretched inter-word pause
  }
  /* Serial.printf("morse_keyer base dit %u, dit %u, dah %u, ies %u, ils %u, iws %u\n", 
     ticksPerBaseDit, ticksPerDit, ticksPerDah, ticksPerIes, ticksPerIls, ticksPerIws); */
  AudioNoInterrupts();
  set_vox_nrpn(vox, KYRP_PER_DIT, ticksPerDit);
  set_vox_nrpn(vox, KYRP_PER_DAH, ticksPerDah);
  set_vox_nrpn(vox, KYRP_PER_IES, ticksPerIes);
  set_vox_nrpn(vox, KYRP_PER_ILS, ticksPerIls);
  set_vox_nrpn(vox, KYRP_PER_IWS, ticksPerIws);
  AudioInterrupts();
}

  
/*
** this where we initialize the keyer, for the time being
** these are set to match what the controller starts with
** synchronizing will be an issue.
*/
static void nrpn_setup(void) {

  /* codec setup */
  nrpn_set(KYRP_MUTE_HEAD_PHONES, 1);
  nrpn_set(KYRP_INPUT_SELECT, 0);
  nrpn_set(KYRP_MIC_PREAMP_GAIN, 0); // suggestion in audio docs
  kyr_nrpn[KYRP_MIC_BIAS] = 7;	     // taken from audio library
  nrpn_set(KYRP_MIC_IMPEDANCE, 1);   // taken from audio library
  nrpn_set(KYRP_MUTE_LINE_OUT, 1);
  nrpn_set(KYRP_LINE_IN_LEVEL, 5);
  nrpn_set(KYRP_LINE_OUT_LEVEL, 29);
  nrpn_set(KYRP_MUTE_HEAD_PHONES, 0);
  for (int i = 0; i <= 80; i += 1)
    nrpn_set(KYRP_HEAD_PHONE_VOLUME, i);

  /* soft controls */
  // KYRP_AUDIO_MODE		(KYRP_SOFT+0) /* sound card operation mode */
  // KYRP_ST_PAN		(KYRP_SOFT+1) /* pan sidetone left or right */
  nrpn_set(KYRP_SEND_MIDI, 1);
  nrpn_set(KYRP_RECV_MIDI, 0);
  nrpn_set(KYRP_IQ_ENABLE, 0);
  nrpn_set(KYRP_IQ_ADJUST, 8096);
  nrpn_set(KYRP_TX_ENABLE, 0);
  nrpn_set(KYRP_ST_ENABLE, 1);

  /* morse code table */
  // extern const unsigned char morse[64];
  for (int i = 0; i < 64; i += 1) kyr_nrpn[KYRP_MORSE+i] = morse[i];

  /* mixer matrix */
  
  /* keyer defaults */
  nrpn_set(KYRP_SPEED,18);
  nrpn_set(KYRP_WEIGHT,50);
  nrpn_set(KYRP_RATIO,50);
  nrpn_set(KYRP_COMP,0);
  nrpn_set(KYRP_FARNS,0);
  nrpn_set(KYRP_TONE,600);
  nrpn_set(KYRP_RISE_TIME, 50);	// 5.0 ms
  nrpn_set(KYRP_FALL_TIME, 50);	// 5.0 ms
  nrpn_set(KYRP_RISE_RAMP, KYRP_RAMP_HANN);
  nrpn_set(KYRP_FALL_RAMP, KYRP_RAMP_HANN);
  nrpn_set(KYRP_TAIL_TIME, 0);	// 0.0 ms
  nrpn_set(KYRP_HEAD_TIME, 0);	// 0.0 ms
  nrpn_set(KYRP_PAD_MODE, KYRP_MODE_A);
  nrpn_set(KYRP_PAD_SWAP, 0);
  nrpn_set(KYRP_PAD_ADAPT, KYRP_ADAPT_NORMAL);
  
  /* voice specializations */
  for (int i = KYRP_VOX_1; i < KYRP_LAST; i += 1) kyr_nrpn[i] = KYRP_NOT_SET;
}

/*
** set a NRPN value.
** all are copied into the kyr_nrpn[] array.
** some are transformed into sample unit timings.
** some trigger calls to the sgtl5000 control interface.
** none are sent into the audio library processing loop,
** it pulls parameters as necessary.
*/
static void nrpn_set(uint16_t nrpn, uint16_t value) {
  switch (nrpn) {
  case KYRP_HEAD_PHONE_VOLUME: // Serial.printf("HEAD_PHONE_VOLUME %d\n", value); 
    // inspecting the sgtl5000 source finds only 7-8bits of precision for this.
    sgtl5000.volume(value/255.0); kyr_nrpn[nrpn] = value;  break; // fix.me - automate
  case KYRP_INPUT_SELECT:  Serial.printf("INPUT_SELECT %d\n", value);  
    sgtl5000.inputSelect(value); kyr_nrpn[nrpn] = value; break;
  case KYRP_MUTE_HEAD_PHONES: // Serial.printf("MUTE_HEAD_PHONES %d\n", value); 
    if (value) sgtl5000.muteHeadphone(); else sgtl5000.unmuteHeadphone();    
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_MUTE_LINE_OUT: // Serial.printf("MUTE_LINE_OUT %d\n", value); 
    if (value) sgtl5000.muteLineout(); else sgtl5000.unmuteLineout();
    kyr_nrpn[nrpn] = value; break;
  case KYRP_LINE_OUT_LEVEL: // Serial.printf("LINE_IN_LEVEL %d\n", value); 
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_L] = kyr_nrpn[KYRP_LINE_OUT_LEVEL_R] = value;
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_OUT_LEVEL_L: // Serial.printf("LINE_OUT_LEVEL_L %d\n", value); 
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_L] = value;
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_OUT_LEVEL_R: // Serial.printf("LINE_OUT_LEVEL_R %d\n", value); 
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_R] = value;
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL: // Serial.printf("LINE_OUT_LEVEL %d\n", value); 
    kyr_nrpn[KYRP_LINE_IN_LEVEL_L] = kyr_nrpn[KYRP_LINE_IN_LEVEL_R] = value;
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL_L: // Serial.printf("LINE_IN_LEVEL_L %d\n", value); 
    kyr_nrpn[KYRP_LINE_IN_LEVEL_L] = value;
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL_R: // Serial.printf("LINE_IN_LEVEL_R %d\n", value); 
    kyr_nrpn[KYRP_LINE_IN_LEVEL_R] = value;
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_MIC_PREAMP_GAIN: Serial.printf("MIC_PREAMP_GAIN %d\n", value);  
    sgtl5000.micPreampGain(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_MIC_BIAS: Serial.printf("MIC_BIAS %d\n", value);  
    sgtl5000.micBias(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_MIC_IMPEDANCE: Serial.printf("MIC_IMPEDANCE %d\n", value);  
    sgtl5000.micImpedance(value);
    kyr_nrpn[nrpn] = value; break;

    //case KYRP_AUDIO_MODE:
    //case KYRP_ST_PAN:
  case KYRP_SEND_MIDI:
    kyr_nrpn[nrpn] = value; break;
  case KYRP_RECV_MIDI:
    kyr_nrpn[nrpn] = value; break;
  case KYRP_IQ_ENABLE:  Serial.printf("IQ_ENABLE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_IQ_ADJUST: // Serial.printf("IQ_ADJUST %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_TX_ENABLE: // Serial.printf("ST_ENABLE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_ST_ENABLE: // Serial.printf("ST_ENABLE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_IQ_BALANCE: // Serial.printf("IQ_BALANCE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
    
    // case KYRP_MORSE+(0..63):
    
    // case KYRP_MIXER+(0..23):
    
  case KYRP_TONE: // Serial.printf("TONE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_HEAD_TIME: // Serial.printf("HEAD_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_TAIL_TIME: // Serial.printf("TAIL_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_RISE_TIME: // Serial.printf("RISE_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value; break;
  case KYRP_FALL_TIME: // Serial.printf("FALL_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_RISE_RAMP: Serial.printf("RISE_RAMP %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_FALL_RAMP: Serial.printf("FALL_RAMP %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;

  case KYRP_SPEED: // Serial.printf("SPEED %d\n", value); 
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(KYR_VOX_NONE); break;
  case KYRP_WEIGHT: // Serial.printf("WEIGHT %d\n", value); 
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(KYR_VOX_NONE); break;
  case KYRP_RATIO: // Serial.printf("RATIO %d\n", value);
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(KYR_VOX_NONE); break;
  case KYRP_COMP: // Serial.printf("COMP %d\n", value);
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(KYR_VOX_NONE); break;
  case KYRP_FARNS: // Serial.printf("FARNS %d\n", value);
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(KYR_VOX_NONE); break;

  case KYRP_PAD_MODE: Serial.printf("PAD_MODE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_PAD_SWAP: // Serial.printf("PAD_SWAP %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_PAD_ADAPT: Serial.printf("PAD_ADAPT %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;

  case KYRP_PER_DIT:
  case KYRP_PER_DAH:
  case KYRP_PER_IES:
  case KYRP_PER_ILS:
  case KYRP_PER_IWS: kyr_nrpn[nrpn] = value; break;

  default: Serial.printf("uncaught nrpn #%d with value %d\n", nrpn, value); break;

  }
}

/***************************************************************
** MIDI input handling.
***************************************************************/
/*
** this could be a NRPN, too, but the penalty for screwing it up
** could be pretty fierce.  it could also be split into different
** channels for receive and send.
*/
static uint8_t kyr_channel = KYR_CHANNEL;


/*
** receive a note on event and do something with it.
** only if KYRP_RECV_MIDI is enabled and it's on our channel.
*/
static void myNoteOn(byte channel, byte note, byte velocity) {
  if (get_recv_midi() && (channel == kyr_channel)) {
    switch (note) {
    case KYR_L_PAD_NOTE:      /* note used to report raw left paddle switch */
      digitalWrite(KYR_L_PAD_PIN, velocity == 0); break;
    case KYR_R_PAD_NOTE:      /* note used to report raw right paddle switch */
      digitalWrite(KYR_R_PAD_PIN, velocity == 0); break;
    case KYR_S_KEY_NOTE:      /* note used to report raw straight key switch */
      digitalWrite(KYR_S_KEY_PIN, velocity == 0); break;
    case KYR_PTT_SW_NOTE:      /* note used to report raw ptt switch */
      digitalWrite(KYR_PTT_SW_PIN, velocity == 0); break;
    case KYR_KEY_OUT_NOTE:      /* note used to send external key signal */
      digitalWrite(KYR_KEY_OUT_PIN, velocity == 0); break;
    case KYR_PTT_OUT_NOTE:      /* note used to send external ptt signal */
      digitalWrite(KYR_PTT_OUT_PIN, velocity == 0); break;
    default:
      break;
    }
  }
}

static void myNoteOff(byte channel, byte note, byte velocity) {
  if (get_recv_midi() && (channel == kyr_channel)) {
    switch (note) {
    case KYR_L_PAD_NOTE:      /* note used to report raw left paddle switch */
      digitalWrite(KYR_L_PAD_PIN, 1); break;
    case KYR_R_PAD_NOTE:      /* note used to report raw right paddle switch */
      digitalWrite(KYR_R_PAD_PIN, 1); break;
    case KYR_S_KEY_NOTE:      /* note used to report raw straight key switch */
      digitalWrite(KYR_S_KEY_PIN, 1); break;
    case KYR_PTT_SW_NOTE:      /* note used to report raw ptt switch */
      digitalWrite(KYR_PTT_SW_PIN, 1); break;
    case KYR_KEY_OUT_NOTE:      /* note used to send external key signal */
      digitalWrite(KYR_KEY_OUT_PIN, 1); break;
    case KYR_PTT_OUT_NOTE:      /* note used to send external ptt signal */
      digitalWrite(KYR_PTT_OUT_PIN, 1); break;
    default:
      break;
    }
  }
}

static void myControlChange(byte channel, byte control, byte value) {
  /*
  ** handle NRPN format
  ** the canned ctrlr format sends MSB, LSB
  ** for both the NRPN number and the value.
  */
  static uint16_t cc_nrpn, cc_value;

  if (channel == kyr_channel)
    switch (control) {
    case KYR_CC_MSB:	/* Data Entry (MSB) */
      cc_value = (value&127)<<7;
      break;
    case KYR_CC_LSB:	/* Data Entry (LSB) */
      cc_value |= value&127;
      nrpn_set(cc_nrpn, cc_value); 
      break;
    case KYR_CC_NRPN_MSB:	/* Non-registered Parameter (MSB) */
      cc_nrpn = (value&127)<<7;
      break;
    case KYR_CC_NRPN_LSB:	/* Non-registered Parameter (LSB) */
      cc_nrpn |= value&127;
      break;
    default:
      break;
    }
}

static void midi_setup(void) {
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
}

static uint8_t midi_send_toggle(uint8_t pin, uint8_t note) {
  // if the pin was high, then it went low, and is now active, so send velocity 1
  // if the pin was low, then it went high, and is now off, so send velocity 0
  usbMIDI.sendNoteOn(note, pin, kyr_channel);
  usbMIDI.send_now();		// send it now
  return pin ^ 1;		// invert the pin
}
static void midi_loop(void) {
  while (usbMIDI.read());
  if (get_send_midi()) {
    static uint8_t l_pad, r_pad, s_key, ptt_sw, key_out, ptt_out;
    if (digitalRead(KYR_L_PAD_PIN) != l_pad) l_pad = midi_send_toggle(l_pad, KYR_L_PAD_NOTE);
    if (digitalRead(KYR_R_PAD_PIN) != r_pad) r_pad = midi_send_toggle(r_pad, KYR_R_PAD_NOTE);
    if (digitalRead(KYR_S_KEY_PIN) != s_key) s_key = midi_send_toggle(r_pad, KYR_S_KEY_NOTE);
    if (digitalRead(KYR_PTT_SW_PIN) != ptt_sw) ptt_sw = midi_send_toggle(r_pad, KYR_PTT_SW_NOTE);
    if (digitalRead(KYR_KEY_OUT_PIN) != key_out) key_out = midi_send_toggle(r_pad, KYR_KEY_OUT_NOTE);
    if (digitalRead(KYR_PTT_OUT_PIN) != ptt_out) ptt_out = midi_send_toggle(r_pad, KYR_PTT_OUT_NOTE);
  }
}

/***************************************************************
** Winkey interface
***************************************************************/
static void winkey_setup(void) {
}

static void winkey_loop(void) {
}

