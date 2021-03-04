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
// audio sample values
#include "src/Audio/sample_value.h"

#include <Wire.h>

// Audio Library program, written by hand

// inputs
AudioInputUSB           usb_in;	// usb audio in
AudioInputI2S           i2s_in;	// i2s audio in
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
// switch codec to use line-in instead of microphone
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


// outputs
AudioOutputSample	key_out;
AudioOutputSample	ptt_out;
AudioOutputSample	probe1;
AudioOutputSample	probe2;

AudioConnection		patchCord10a(ptt_delay, 0, key_out, 0);
AudioConnection		patchCord10b(ptt_delay, 1, ptt_out, 0);

AudioConnection		patchCord10c(paddle, 0, probe1, 0);
AudioConnection		patchCord10d(arbiter, 0, probe2, 0);

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
AudioConnection		patchCord11e(r_hdw_out, 0, hdw_out, 1);

#elif defined(TEENSY40) || defined(TEENSY41)
AudioOutputMQS		hdw_out;
AudioConnection		patchCord11e(l_hdw_out, 0, hdw_out, 0);
AudioConnection		patchCord11e(r_hdw_out, 0, hdw_out, 1);
#endif

// codec control
AudioControlSGTL5000     sgtl5000;

// counter to demonstrate that attachInterrupt to LRCLK
// actually produced a sample rate interrupt.
// unsigned long buttonpolls = 0;
//
// poll inputs at sample rate
// latch outputs at sample rate
// could also poll/latch arbitrary memory locations
// like these three.
uint8_t _tx_enable, _st_enable;
int16_t _probe1, _probe2;

static void pollatch() {
  l_pad.send(bool2fix(digitalRead(KYR_L_PAD_PIN)^1));
  r_pad.send(bool2fix(digitalRead(KYR_R_PAD_PIN)^1));
  s_key.send(bool2fix(digitalRead(KYR_S_KEY_PIN)^1));
  ptt_sw.send(bool2fix(digitalRead(KYR_PTT_SW_PIN)^1));
  st_enable.send(bool2fix(_st_enable));
  tx_enable.send(bool2fix(_tx_enable));
  digitalWrite(KYR_KEY_OUT_PIN,fix2bool(key_out.recv()));
  digitalWrite(KYR_PTT_OUT_PIN,fix2bool(ptt_out.recv()));
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

static void nrpn_setup(void) {
  nrpn_set(KYRP_SPEED,18);
  nrpn_set(KYRP_WEIGHT,50);
  nrpn_set(KYRP_RATIO,50);
  nrpn_set(KYRP_COMP,0);
  nrpn_set(KYRP_FARNS,0);
  nrpn_set(KYRP_TONE,800);
  nrpn_set(KYRP_RISE_TIME, 50);	// 50 ms/10
  nrpn_set(KYRP_FALL_TIME, 50);	// 50 ms/10
  nrpn_set(KYRP_RISE_RAMP, KYRP_RAMP_HANN);
  nrpn_set(KYRP_FALL_RAMP, KYRP_RAMP_HANN);
  nrpn_set(KYRP_TAIL_TIME, 50);	// 50 ms/10
  nrpn_set(KYRP_HEAD_TIME, 50);	// 50 ms/10
  nrpn_set(KYRP_PAD_MODE, KYRP_MODE_A);
  nrpn_set(KYRP_PAD_SWAP, 0);
  nrpn_set(KYRP_PAD_ADAPT, KYRP_ADAPT_NORMAL);
}

void setup(void) {
  Serial.begin(115200);
  pinMode(KYR_L_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_R_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_S_KEY_PIN, INPUT_PULLUP);
  pinMode(KYR_PTT_SW_PIN, INPUT_PULLUP);
  pinMode(KYR_KEY_OUT_PIN, OUTPUT); digitalWrite(KYR_KEY_OUT_PIN, 1);
  pinMode(KYR_PTT_OUT_PIN, OUTPUT); digitalWrite(KYR_PTT_OUT_PIN, 1);
  arbiter_setup();
  nrpn_setup();
  AudioMemory(40);
  attachInterrupt(KYR_LRCLK, pollatch, RISING);
  sgtl5000.enable();
  sgtl5000.volume(0.3);		// needs to be automated to remove pop
  midi_setup();
  winkey_setup();
}

#include "diagnostics.h"

void loop(void) {
  diagnostics_loop();
  winkey_loop();
  midi_loop();
}

/***************************************************************
** MIDI input handling.
***************************************************************/
uint8_t kyr_channel = KYR_CHANNEL;

int16_t kyr_nrpn[KYRP_LAST];

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
  set_vox_dit(vox, ticksPerDit);
  set_vox_dah(vox, ticksPerDah);
  set_vox_ies(vox, ticksPerIes);
  set_vox_ils(vox, ticksPerIls);
  set_vox_iws(vox, ticksPerIws);
}

static void nrpn_set(uint16_t nrpn, uint16_t value) {
  switch (nrpn) {
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

  case KYRP_RISE_TIME: // Serial.printf("RISE_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value; break;
  case KYRP_FALL_TIME: // Serial.printf("FALL_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_RISE_RAMP: // Serial.printf("RISE_RAMP %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_FALL_RAMP: // Serial.printf("FALL_RAMP %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_TONE: // Serial.printf("TONE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_HEAD_TIME: // Serial.printf("HEAD_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_TAIL_TIME: // Serial.printf("TAIL_TIME %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_PAD_MODE: // Serial.printf("PAD_MODE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_PAD_SWAP: // Serial.printf("PAD_SWAP %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_PAD_ADAPT: // Serial.printf("PAD_ADAPT %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_IQ_ENABLE: // Serial.printf("IQ_ENABLE %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_IQ_ADJUST: // Serial.printf("IQ_ADJUST %d\n", value); 
    kyr_nrpn[nrpn] = value;  break;

  case KYRP_HEAD_PHONE_VOLUME: Serial.printf("HEAD_PHONE_VOLUME %d\n", value); 
    sgtl5000.volume(value/32763.0);
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_INPUT_SELECT: Serial.printf("INPUT_SELECT %d\n", value);  
    sgtl5000.inputSelect(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_MIC_GAIN: Serial.printf("MIC_GAIN %d\n", value);  
    sgtl5000.micGain(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_MUTE_HEAD_PHONES: Serial.printf("MUTE_HEAD_PHONES %d\n", value); 
    if (value) sgtl5000.muteHeadphone(); else sgtl5000.unmuteHeadphone();    
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_MUTE_LINE_OUT: Serial.printf("MUTE_LINE_OUT %d\n", value); 
    if (value) sgtl5000.muteLineout(); else sgtl5000.unmuteLineout();
    kyr_nrpn[nrpn] = value; break;
  case KYRP_LINE_IN_LEVEL: Serial.printf("LINE_IN_LEVEL %d\n", value); 
    sgtl5000.lineInLevel(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_LINE_OUT_LEVEL: Serial.printf("LINE_OUT_LEVEL %d\n", value); 
    sgtl5000.lineOutLevel(value);
    kyr_nrpn[nrpn] = value; break;
    
  default: Serial.printf("uncaught nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void myNoteOn(byte channel, byte note, byte velocity) {
  if (channel == kyr_channel) {
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
  if (channel == kyr_channel) {
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

void midi_setup(void) {
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
}

void midi_loop(void) {
  while (usbMIDI.read());
}

/***************************************************************
** Winkey input handling.
***************************************************************/
void winkey_setup(void) {
}

void winkey_loop(void) {
}

