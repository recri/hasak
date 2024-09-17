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
/***************************************************************
** Audio library
***************************************************************/

#include <Audio.h>
// custom audio modules
#include "src/Audio/control_wm8960.h"
#include "src/Audio/input_byte.h"
#include "src/Audio/synth_keyed_tone.h"
#include "src/Audio/output_byte.h"
#include "src/Audio/rmixer.h"

// audio sample values
#include "src/Audio/sample_value.h"

#include <Wire.h>

// Audio Library program, written by hand

// inputs
AudioInputUSB           usb_in;	// usb audio in
AudioInputI2S           i2s_in;	// i2s audio in
// AudioInputAnalog	*hdw_inp; // audio input from analog 0 FIX.ME, allocate on enable
// AudioConnection		*hdw_in_patch[6]; /* audio input connects to output mixers */
AudioInputByte		au_st_key;  // sidetone key line
AudioInputByte		au_key_out; // transmitter key line

// shaped key waveform
AudioSynthKeyedTone	tone_ramp(1); // one channel sidetone ramp
AudioSynthKeyedTone	key_ramp(2);  // two channel IQ tone ramp
AudioConnection		patchCord3a(au_st_key, 0, tone_ramp, 0); // sidetone key line
AudioConnection		patchCord3b(au_key_out, 1, key_ramp, 0);  // transmit key line

// 
// output mixers
// channel 0 rx_input from usb_in or mic_input from i2s_in
// passing in nrpns for the gain and enable parameters for each channel
// and slew paramters common to all channels
// 
RAudioMixer4              l_usb_out(NRPN_MIX_USB_L0,NRPN_MIX_EN_USB_L0,NRPN_MIXER_SLEW_RAMP,NRPN_MIXER_SLEW_TIME);
RAudioMixer4              r_usb_out(NRPN_MIX_USB_R0,NRPN_MIX_EN_USB_R0,NRPN_MIXER_SLEW_RAMP,NRPN_MIXER_SLEW_TIME);
RAudioMixer4              l_i2s_out(NRPN_MIX_I2S_L0,NRPN_MIX_EN_I2S_L0,NRPN_MIXER_SLEW_RAMP,NRPN_MIXER_SLEW_TIME);
RAudioMixer4              r_i2s_out(NRPN_MIX_I2S_R0,NRPN_MIX_EN_I2S_R0,NRPN_MIXER_SLEW_RAMP,NRPN_MIXER_SLEW_TIME);
RAudioMixer4              l_hdw_out(NRPN_MIX_HDW_L0,NRPN_MIX_EN_HDW_L0,NRPN_MIXER_SLEW_RAMP,NRPN_MIXER_SLEW_TIME);
RAudioMixer4              r_hdw_out(NRPN_MIX_HDW_R0,NRPN_MIX_EN_HDW_R0,NRPN_MIXER_SLEW_RAMP,NRPN_MIXER_SLEW_TIME);

// first output channel, rx audio and microphone/line-in input, op on headset mode
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

// fourth channel, hdw_in when enabled FIX.ME

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


#if defined(TEENSY31) || defined(TEENSY32)
AudioOutputAnalog	hdw_out;
#elif defined(TEENSY35) || defined(TEENSY36)
AudioOutputAnalogStereo	hdw_out;
#elif defined(TEENSY40) || defined(TEENSY41)
AudioOutputMQS		hdw_out;
#endif
AudioConnection		*hdw_out_patch[2];

// codec control
AudioControlSGTL5000     sgtl5000; // the codec in the teensy audio shield
AudioControlWM8960	   wm8960; // the codec in the CWKeyer shield

//
// access to component names
//
static const char *audio_comp_name(int index) {
  switch (index) {
  case AUDIO_USB_IN: return "usb_in";
  case AUDIO_I2S_IN: return "i2s_in";
    // case AUDIO_HDW_IN: return "hdw_in";
  case AUDIO_ST_KEY: return "au_st_key";
  case AUDIO_KEY_OUT: return "au_key_out";
  case AUDIO_ST_OSC: return "tone_ramp";
  case AUDIO_IQ_OSC: return "key_ramp";
  case AUDIO_L_USB_OUT: return "l_usb_out";
  case AUDIO_R_USB_OUT: return "r_usb_out";
  case AUDIO_L_I2S_OUT: return "l_i2s_out";
  case AUDIO_R_I2S_OUT: return "r_i2s_out";
  case AUDIO_L_HDW_OUT: return "l_hdw_out";
  case AUDIO_R_HDW_OUT: return "r_hdw_out";
  case AUDIO_USB_OUT: return "usb_out";
  case AUDIO_I2S_OUT: return "i2s_out";
  case AUDIO_HDW_OUT: return "hdw_out";
  default:
    Serial.printf("audio_comp_name(%d) invalid index %d\n", index, index);
    return NULL;
  }
}

//
// access to components
//
static AudioStream *audio_comp(int index) {
  switch (index) {
  case AUDIO_USB_IN: return &usb_in;
  case AUDIO_I2S_IN: return &i2s_in;
    // case AUDIO_HDW_IN: return hdw_inp;
  case AUDIO_ST_KEY: return &au_st_key;
  case AUDIO_KEY_OUT: return &au_key_out;
  case AUDIO_ST_OSC: return &tone_ramp;
  case AUDIO_IQ_OSC: return &key_ramp;
  case AUDIO_L_USB_OUT: return &l_usb_out;
  case AUDIO_R_USB_OUT: return &r_usb_out;
  case AUDIO_L_I2S_OUT: return &l_i2s_out;
  case AUDIO_R_I2S_OUT: return &r_i2s_out;
  case AUDIO_L_HDW_OUT: return &l_hdw_out;
  case AUDIO_R_HDW_OUT: return &r_hdw_out;
  case AUDIO_USB_OUT: return &usb_out;
  case AUDIO_I2S_OUT: return &i2s_out;
  case AUDIO_HDW_OUT: return &hdw_out;
  default: 
    Serial.printf("audio_comp(%d) invalid index %d\n", index, index);
    return NULL;
  }
}

//
// fetch the update counter
//
static unsigned audio_update_counter(void) { return au_st_key.update_counter; }

//
// reset the audio cpu cycle counts
//
static void audio_timing_reset(void) {
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
  for (int i = 0; i < KYR_N_AUDIO; i += 1)
    if (audio_comp(i))
      audio_comp(i)->processorUsageMaxReset();
}

// error diagnostic says this is invalid because AudioInputAnalog has non-virtual members?
// optional wiring
// enable or disable hardware input
// this is a mono audio channel read from analog pin A0
//static void audio_hdw_in_enable(int nrpn, int _) {
//  if (nrpn_get(nrpn)) {		/* enable hardware in */
//    if (hdw_inp == NULL) {
//      AudioNoInterrupts();
//      hdw_inp = new AudioInputAnalog();
//      hdw_in_patch[0] = new AudioConnection(*hdw_inp, 0, l_usb_out, 3);
//      hdw_in_patch[1] = new AudioConnection(*hdw_inp, 0, r_usb_out, 3);
//      hdw_in_patch[2] = new AudioConnection(*hdw_inp, 0, l_i2s_out, 3);
//      hdw_in_patch[3] = new AudioConnection(*hdw_inp, 0, r_i2s_out, 3);
//      hdw_in_patch[4] = new AudioConnection(*hdw_inp, 0, l_hdw_out, 3);
//      hdw_in_patch[5] = new AudioConnection(*hdw_inp, 0, r_hdw_out, 3);
//      AudioInterrupts();
//    }
//  } else {			/* disable hardware in */
//    if (hdw_inp != NULL) {
//      AudioNoInterrupts();
//      delete hdw_inp; hdw_inp = NULL;
//      delete hdw_in_patch[0]; hdw_in_patch[0] = NULL;
//      delete hdw_in_patch[1]; hdw_in_patch[1] = NULL;
//      delete hdw_in_patch[2]; hdw_in_patch[2] = NULL;
//      delete hdw_in_patch[3]; hdw_in_patch[3] = NULL;
//      delete hdw_in_patch[4]; hdw_in_patch[4] = NULL;
//      delete hdw_in_patch[5]; hdw_in_patch[5] = NULL;
//      AudioInterrupts();
//    }
//  }
//}

// optional wiring
// enable or disable hardware output
// this is a mono or stereo output audio channel
static void audio_hdw_out_enable(int nrpn, int _) {
  if (nrpn_get(nrpn)) {
      AudioNoInterrupts();
#if defined(TEENSY31) || defined(TEENSY32)
      hdw_out_patch[0] = new AudioConnection(l_hdw_out, 0, hdw_out, 0);
#elif defined(TEENSY35) || defined(TEENSY36)
      hdw_out_patch[0] = new AudioConnection(l_hdw_out, 0, hdw_out, 0);
      hdw_out_patch[1] = new AudioConnection(r_hdw_out, 0, hdw_out, 1);
#elif defined(TEENSY40) || defined(TEENSY41)
      hdw_out_patch[0] = new AudioConnection(l_hdw_out, 0, hdw_out, 0);
      hdw_out_patch[1] = new AudioConnection(r_hdw_out, 0, hdw_out, 1);
#endif
      AudioInterrupts();
  } else {
      AudioNoInterrupts();
#if defined(TEENSY31) || defined(TEENSY32)
      delete hdw_out_patch[0]; hdw_out_patch[0] = NULL;
#elif defined(TEENSY35) || defined(TEENSY36)
      delete hdw_out_patch[0]; hdw_out_patch[0] = NULL;
      delete hdw_out_patch[1]; hdw_out_patch[1] = NULL;
#elif defined(TEENSY40) || defined(TEENSY41)
      delete hdw_out_patch[0]; hdw_out_patch[0] = NULL;
      delete hdw_out_patch[1]; hdw_out_patch[1] = NULL;
#endif
      AudioInterrupts();
  }
}

static void audio_setup(void) {
  //  nrpn_listen(NRPN_HDW_IN_ENABLE, audio_hdw_in_enable);
  nrpn_listen(NRPN_HDW_OUT_ENABLE, audio_hdw_out_enable);
}
