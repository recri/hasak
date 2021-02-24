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

#include "config.h"
#include <Arduino.h>

/*
** forward references.
*/
void midi_setup(void);
void midi_loop(void);
void winkey_setup(void);
void winkey_loop(void);
int get_active_vox(void);

/***************************************************************
** Audio library modules
***************************************************************/
#include <Audio.h>

#include "src/Audio/input_sample.h"
#include "src/Audio/input_text.h"
#include "src/Audio/effect_paddle.h"
#include "src/Audio/effect_arbiter.h"
#include "src/Audio/effect_ramp.h"
#include "src/Audio/effect_mute.h"
#include "src/Audio/effect_and_not.h"
#include "src/Audio/effect_ptt_delay.h"
#include "src/Audio/output_sample.h"

#include "src/Audio/sample_value.h"

#include <Wire.h>
//#include <SPI.h>
//#include <SD.h>
//#include <SerialFlash.h>


// Audio Library program, written by hand

// inputs
AudioInputUSB           usb_in;	// usb audio in
AudioInputI2S           i2s_in;	// i2s audio in
AudioInputSample	l_pad;	// left paddle in
AudioInputSample	r_pad;	// right paddle in
AudioInputSample	s_key;	// straight key in
AudioInputText		wink;	// winkey text in
AudioInputText		kyr;	// kyr text in for op
AudioInputSample        ptt_sw;	// ptt switch
AudioInputSample	tx_enable; // transmit enable
AudioInputSample	st_enable; // side tone enable

// keyer logic
AudioEffectPaddle	paddle;	// 
AudioConnection         patchCord1a(l_pad, 0, paddle, 0);
AudioConnection         patchCord1b(r_pad, 0, paddle, 1);

// arbitration
AudioEffectArbiter	arbiter;
AudioConnection		patchCord2b(s_key, 0, arbiter, 0);
AudioConnection		patchCord2a(paddle, 0, arbiter, 1);
AudioConnection		patchCord2c(wink, 0, arbiter, 2);
AudioConnection		patchCord2d(kyr, 0, arbiter, 3);
static void arbiter_setup(void) {
  arbiter.define_vox(0, KYR_VOX_S_KEY, KYR_PRI_S_KEY);
  arbiter.define_vox(1, KYR_VOX_PAD, KYR_PRI_PAD);
  arbiter.define_vox(2, KYR_VOX_WINK, KYR_PRI_WINK);
  arbiter.define_vox(3, KYR_VOX_KYR, KYR_PRI_KYR);
}
int get_active_vox(void) { return arbiter.get_active_vox(); }

// shaped key waveform
AudioEffectRamp		key_ramp;
AudioSynthWaveformSine	sine_I;
AudioSynthWaveformSine  sine_Q;
AudioEffectMultiply	I_ramp;
AudioEffectMultiply	Q_ramp;
AudioConnection		patchCord3a(arbiter, 0, key_ramp, 0);
AudioConnection		patchCord3b(key_ramp, 0, I_ramp, 0);
AudioConnection		patchCord3c(key_ramp, 0, Q_ramp, 0);
AudioConnection		patchCord3d(sine_I, 0, I_ramp, 1);
AudioConnection		patchCord3e(sine_Q, 0, Q_ramp, 1);

// tx enable
AudioEffectAndNot	and_not_kyr(KYR_VOX_KYR);
AudioConnection		patchCord3f(tx_enable, 0, and_not_kyr, 0); // tx enable if active vox != KYR_VOX_KYR

// microphone mute, disabled when ptt switch is pressed
AudioEffectMute		mic_mute;
AudioEffectMultiply	l_mic_mute;
AudioEffectMultiply	r_mic_mute;
AudioConnection		patchCord4a(ptt_sw, 0, mic_mute, 0);
AudioConnection		patchCord4b(mic_mute, 0, l_mic_mute, 0);
AudioConnection		patchCord4c(mic_mute, 0, r_mic_mute, 0);
AudioConnection		patchCord4d(i2s_in, 0, l_mic_mute, 1);
AudioConnection		patchCord4e(i2s_in, 1, r_mic_mute, 1);

// IQ mute, enabled when tx_enable or active_vox is kyr
AudioEffectMute		IQ_mute;
AudioEffectMultiply	I_mute;
AudioEffectMultiply	Q_mute;
AudioConnection		patchCord5a(and_not_kyr, 0, IQ_mute, 0);
AudioConnection		patchCord5b(IQ_mute, 0, I_mute, 0);
AudioConnection		patchCord5c(IQ_mute, 0, Q_mute, 0);
AudioConnection		patchCord5d(I_ramp, 0, I_mute, 1);
AudioConnection		patchCord5e(Q_ramp, 0, Q_mute, 1);

// receive mute, enabled when arbiter vox line is active
AudioEffectMute		rx_mute;
AudioEffectMultiply	l_rx_mute;
AudioEffectMultiply	r_rx_mute;
AudioConnection		patchCord6a(arbiter, 1, rx_mute, 0);
AudioConnection		patchCord6b(rx_mute, 0, l_rx_mute, 0);
AudioConnection		patchCord6c(rx_mute, 0, r_rx_mute, 0);
AudioConnection		patchCord6d(usb_in, 0, l_rx_mute, 1);
AudioConnection		patchCord6e(usb_in, 1, r_rx_mute, 1);

// sidetone mute, enabled when sidetone enabled is true
AudioEffectMute		st_mute;
AudioEffectMultiply	l_st_mute;
AudioEffectMultiply	r_st_mute;
AudioConnection		patchCord7a(st_enable, 0, st_mute, 0);
AudioConnection		patchCord7b(st_mute, 0, l_st_mute, 0);
AudioConnection		patchCord7c(st_mute, 0, r_st_mute, 0);
AudioConnection		patchCord7d(I_ramp, 0, l_st_mute, 1);
AudioConnection		patchCord7e(I_ramp, 0, r_st_mute, 1);

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
AudioConnection		patchCord900(l_rx_mute, 0, l_i2s_out, 0);
AudioConnection		patchCord910(r_rx_mute, 0, r_i2s_out, 0);
AudioConnection		patchCord920(l_mic_mute, 0, l_usb_out, 0);
AudioConnection		patchCord930(r_mic_mute, 0, r_usb_out, 0);
AudioConnection		patchCord940(l_rx_mute, 0, l_hdw_out, 0);
AudioConnection		patchCord950(r_rx_mute, 0, r_hdw_out, 0);

// second channel, line-out audio and line-in audio, 2x2 sound card mode
AudioConnection		patchCord901(usb_in, 0, l_i2s_out, 1);
AudioConnection		patchCord911(usb_in, 1, r_i2s_out, 1);
AudioConnection		patchCord921(i2s_in, 0, l_usb_out, 1);
AudioConnection		patchCord931(i2s_in, 1, r_usb_out, 1);
AudioConnection		patchCord941(usb_in, 0, l_hdw_out, 1);
AudioConnection		patchCord951(usb_in, 1, r_hdw_out, 1);

// third channel, sidetone, balance l vs r to localize
AudioConnection		patchCord902(l_st_mute, 0, l_i2s_out, 2);
AudioConnection		patchCord912(r_st_mute, 0, r_i2s_out, 2);
AudioConnection		patchCord922(l_st_mute, 0, l_usb_out, 2);
AudioConnection		patchCord932(r_st_mute, 0, r_usb_out, 2);
AudioConnection		patchCord942(l_st_mute, 0, l_hdw_out, 2);
AudioConnection		patchCord952(r_st_mute, 0, r_hdw_out, 2);

// fourth channel, keyed IQ
AudioConnection		patchCord903(I_mute, 0, l_i2s_out, 3);
AudioConnection		patchCord913(Q_mute, 0, r_i2s_out, 3);
AudioConnection		patchCord923(I_mute, 0, l_usb_out, 3);
AudioConnection		patchCord933(Q_mute, 0, r_usb_out, 3);
AudioConnection		patchCord943(I_mute, 0, l_hdw_out, 3);
AudioConnection		patchCord953(Q_mute, 0, r_hdw_out, 3);


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
AudioControlSGTL5000     sgtl5000_1;

// counter to demonstrate that attachInterrupt to LRCLK
// actually produced a sample rate interrupt.
//unsigned long buttonpolls = 0;
//
// poll inputs at sample rate
// latch outputs at sample rate
// could also poll/latch arbitrary memory locations
// like these three.
uint8_t _tx_enable, _st_enable;
int16_t _probe1, _probe2;

void pollatch() {
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

void setup() {
  Serial.begin(115200);
  pinMode(KYR_L_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_R_PAD_PIN, INPUT_PULLUP);
  pinMode(KYR_S_KEY_PIN, INPUT_PULLUP);
  pinMode(KYR_PTT_SW_PIN, INPUT_PULLUP);
  pinMode(KYR_KEY_OUT_PIN, OUTPUT);
  pinMode(KYR_PTT_OUT_PIN, OUTPUT);
  digitalWrite(KYR_KEY_OUT_PIN, 0);
  digitalWrite(KYR_PTT_OUT_PIN, 0);
  attachInterrupt(KYR_LRCLK, pollatch, RISING);
  arbiter_setup();
  sine_I.frequency(800);
  sine_Q.frequency(800);
  sine_I.amplitude(1.0);
  sine_Q.amplitude(1.0);
  sine_I.phase(0);
  sine_Q.phase(-M_PI/2);
  AudioMemory(40);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.3);
  midi_setup();
  winkey_setup();
}

void ioreport(const char *tag, int over, int under, int usage, int max) {
  Serial.print(tag);
  Serial.print(" overruns: "); Serial.print(over);
  Serial.print(" underruns "); Serial.print(under);
  Serial.print(" usage "); Serial.print(usage);
  Serial.print(" max usage "); Serial.print(max);
  Serial.println();
}
void ireport(const char *tag, AudioInputSample &sample) {
  ioreport(tag, sample.overruns(), sample.underruns(), sample.processorUsage(), sample.processorUsage());
}

void ireset(AudioInputSample &sample) {
  sample.overrunsReset();
  sample.underrunsReset();
  sample.processorUsageMaxReset();
}

void oreport(const char *tag, AudioOutputSample &sample) {
  ioreport(tag, sample.overruns(), sample.underruns(), sample.processorUsage(), sample.processorUsage());
}

void oreset(AudioOutputSample &sample) {
  sample.overrunsReset();
  sample.underrunsReset();
  sample.processorUsageMaxReset();
}

void preport(const char *tag, int16_t *p, int n) {
  int count = 0;
  int words = 0;
  int delay = 1.0/AUDIO_SAMPLE_RATE * 1e6;
  Serial.printf("%s %dus ", tag, delay);
  words += 2;
  for (int i = 0; i < n; i += 1) {
    if (*p != 0) {
      if (count >= 0) count += 1;
      else {
	Serial.printf("%d", count);
	count = 1;
	words += 1;
      }
    } else {
      if (count <= 0) count -= 1;
      else {
	Serial.printf("+%d", count);
	count = -1;
	words += 1;
      }
    }
    if (words > 10) { Serial.printf("\n"); words = 0; }
    delayMicroseconds(delay);
  }
  if (count > 0) Serial.printf("+%d\n", count);
  else Serial.printf("%d\n", count);
}

void mreport(const char *tag, AudioStream &str) {
  Serial.print(tag);
  Serial.print(" usage "); Serial.print(str.processorUsage());
  Serial.print(" max "); Serial.print(str.processorUsageMax());
  Serial.println();
}

void mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}

// #include "src/dspmath/window.h"
static const double dtwo_pi = 2*3.14159265358979323846;		/* 2*pi */
static float cosine_series1(const int size, const int k, const double a0, const double a1) {
  return a0 - a1 * cos((dtwo_pi * k) / (size-1));
}    
static float cosine_series2(const int size, const int k, const double a0, const double a1, const double a2) {
  return cosine_series1(size, k, a0, a1) + a2 * cos((2 * dtwo_pi * k) / (size-1));
}
static float cosine_series3(const int size, const int k, const double a0, const double a1, const double a2, const double a3) {
  return cosine_series2(size, k, a0, a1, a2) - a3 * cos((3 * dtwo_pi * k) / (size-1));
}

static float hann(int k, int size) { return cosine_series1(size, k, 0.50, 0.50); }
static float blackman_harris(int k, int size) { return cosine_series3(size, k, 0.3587500, 0.4882900, 0.1412800, 0.0116800); }

elapsedMicros usecs;

void benchmark_time(const char *name, float (*window)(int, int), int size, float *value) {
  usecs = 0;
  for (int i = 0; i < size; i += 1) { value[i] = window(2*size-1, i); }
  unsigned long t = usecs;
  Serial.printf("benchmark_time(%s, ..., %d) = %lu microseconds, %f us/element\n", name, size, t, t/((float)size));
}

void benchmark_windows() {
  float values[1024];
  benchmark_time("hann", hann, 1024, values);
  benchmark_time("blackman-harris", blackman_harris, 1024, values);
}

void loop() {
  if (Serial.available()) {
    switch (Serial.read()) {
    case '?': 
      Serial.println("hasak monitor usage:");
      Serial.println(" 0 -> audio library resource usage");
      Serial.println(" 1 -> left paddle resource usage");
      Serial.println(" 2 -> right paddle resource usage");
      Serial.println(" 3 -> straight key resource usage");
      Serial.println(" 4 -> ptt switch resource usage");
      Serial.println(" 5 -> key out resource usage");
      Serial.println(" 6 -> ptt out resource usage");
      Serial.println(" 7 -> iambic keyer resource usage");
      Serial.println(" 8 -> sidetone sine resource usage");
      Serial.println(" 9 -> probe1 trace");
      Serial.println(" a -> probe2 trace");
      Serial.println(" r -> reset usage counts");
      Serial.println(" w -> benchmark window computations");
      break;
    case '0':
      Serial.print(" cpu usage "); Serial.print(AudioProcessorUsage()); 
      Serial.print(" max "); Serial.print(AudioProcessorUsageMax());
      // Serial.println();
      Serial.print(" mem usage "); Serial.print(AudioMemoryUsage());
      Serial.print(" max "); Serial.print(AudioMemoryUsageMax());
      Serial.println();
      break;
    case '1': ireport("left paddle", l_pad); break;
    case '2': ireport("right paddle", r_pad); break;
    case '3': ireport("straight key", s_key); break;
    case '4': ireport("ptt switch", ptt_sw); break;
    case '5': oreport("key out", key_out); break;
    case '6': oreport("ptt out", ptt_out); break;
    case '7': mreport("paddle", paddle); break;
    case '8': mreport("sidetone", sine_I); break;
    case '9': preport("probe1", &_probe1, 5*44100); break;
    case 'a': preport("probe2", &_probe2, 5*44100); break;
    case 'r':
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
      ireset(l_pad);
      ireset(r_pad);
      ireset(s_key);
      ireset(ptt_sw);
      oreset(key_out);
      oreset(ptt_out);
      mreset(paddle);
      mreset(sine_I);
      break;
    case 'w':
      benchmark_windows();
      break;
    default:
      //Serial.print("buttonpolls: "); 
      //Serial.println(buttonpolls); 
      break;
    }
  }
}

/***************************************************************
** MIDI input handling.
***************************************************************/
uint8_t kyr_channel = KYR_CHANNEL;

static uint8_t cc_nrpn, cc_nrpn_value;

int16_t kyr_nrpn[1+KYR_N_VOX][1+KYRP_LAST_GLOBAL];

static void nrpn_adjust(int adjust) {
}

static void nrpn_set_msb(int msb) {
  cc_nrpn_value = (msb<<7)|(cc_nrpn_value&127);
}

static void nrpn_set_lsb(int lsb) {
}

static void myNoteOn(byte channel, byte note, byte velocity) {
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
static void myNoteOff(byte channel, byte note, byte velocity) {
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
static void myControlChange(byte channel, byte control, byte value) {
  if (channel == kyr_channel)
    switch (control) {
    case KYR_CC_MSB:	/* Data Entry (MSB) */
      cc_nrpn = ((value&127)<<7)|(cc_nrpn&127); cc_nrpn_value = 0; break;
    case KYR_CC_LSB:	/* Data Entry (LSB) */
      cc_nrpn = (cc_nrpn&(127<<7))|(value&127); cc_nrpn_value = 0; break;
    case KYR_CC_UP:	/* Data Button increment */
      nrpn_adjust(+value); break;
    case KYR_CC_DOWN:	/* Data Button decrement */
      nrpn_adjust(-value); break;
    case KYR_CC_NRPN_LSB:	/* Non-registered Parameter (LSB) */
      nrpn_set_lsb(value); break;
    case KYR_CC_NRPN_MSB:	/* Non-registered Parameter (MSB) */
      nrpn_set_msb(value); break;
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
