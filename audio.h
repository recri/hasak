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
