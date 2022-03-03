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
//#ifdef __cplusplus
//extern "C" {
//#endif

#include "EEPROM.h"
#include "json.h"

#define KYR_NRPN_SIZE		(KYR_N_NRPN*sizeof(int16_t))
#define KYR_MSG_SIZE		(EEPROM_BYTES-KYR_NRPN_SIZE-6*sizeof(int16_t))
#define KYR_SAMPLE_RATE		(((uint16_t)AUDIO_SAMPLE_RATE)/100)
#define KYR_EEPROM_LENGTH	(EEPROM.length())
#define KYR_ID_CPU		(TEENSY_CPU_ID)

typedef struct {
  /* header for eeprom */
  int16_t header[2];
  /* saved messages */
  int8_t msgs[KYR_MSG_SIZE];
  /* index for reading and writing message bytes */
  uint16_t index;
}  hasak_t;

typedef struct {
  int16_t header1[2];
  int16_t nrpns[KYR_N_NRPN];
  int16_t header2[2];
  int8_t msgs[KYR_MSG_SIZE];
  int16_t header3[2];
} hasak_eeprom_image_t;

static hasak_t hasak = {
  { (int16_t)KYR_MAGIC, KYR_NRPN_SIZE },
  { 0 },
  0
};

/*
** Update the keyer timing, specified by speed, weight, ratio, comp, and farnsworth,
** and produce the samples per dit, dah, ies, ils, and iws.
*/
static void nrpn_update_keyer_timing(int numb, int _) {
  const int wordDits = 50;
  const int sampleRate = AUDIO_SAMPLE_RATE;
  const float wpm = nrpn_get(NRPN_SPEED)+nrpn_get(NRPN_SPEED_FRAC)*7.8125e-3f;
  const int weight = nrpn_get(NRPN_WEIGHT);
  const int ratio = nrpn_get(NRPN_RATIO);
  const int compensation = signed_value(nrpn_get(NRPN_COMP));
  const int farnsworth = nrpn_get(NRPN_FARNS);;
  // const float ms_per_dit = (1000 * 60) / (wpm * wordDits);
  const float r = (ratio-50)*0.01;
  const float w = (weight-50)*0.01;
  const int c = compensation;
  /* samples_per_dit = (samples_per_second * second_per_minute) / (words_per_minute * dits_per_word);  */
  const uint32_t ticksPerBaseDit = ((sampleRate * 60.0f) / (wpm * wordDits));
  const int32_t ticksPerDit = (1+r+w) * ticksPerBaseDit+c+0.5;
  const int32_t ticksPerDah = (3-r+w) * ticksPerBaseDit+c+0.5;
  const int32_t ticksPerIes = (1  -w) * ticksPerBaseDit-c+0.5;
        int32_t ticksPerIls = (3  -w) * ticksPerBaseDit-c+0.5;
        int32_t ticksPerIws = (7  -w) * ticksPerBaseDit-c+0.5;
    
  // Serial.printf("recompute_morse settings\n");

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
  // store the values
  /* Serial.printf("morse_keyer base dit %ld, dit %ld, dah %ld, ies %ld, ils %ld, iws %ld\n", 
     ticksPerBaseDit, ticksPerDit, ticksPerDah, ticksPerIes, ticksPerIls, ticksPerIws); */
  // AudioNoInterrupts();
  xnrpn_set(NRPN_XPER_DIT, ticksPerDit);
  xnrpn_set(NRPN_XPER_DAH, ticksPerDah);
  xnrpn_set(NRPN_XPER_IES, ticksPerIes);
  xnrpn_set(NRPN_XPER_ILS, ticksPerIls);
  xnrpn_set(NRPN_XPER_IWS, ticksPerIws);
  // AudioInterrupts();
  /* Serial.printf("morse_keyer dit %ld, dah %ld, ies %ld, ils %ld, iws %ld\n", 
     get_vox_xnrpn(vox, NRPN_XPER_DIT), get_vox_xnrpn(vox, NRPN_XPER_DAH),
     get_vox_xnrpn(vox, NRPN_XPER_IES), get_vox_xnrpn(vox, NRPN_XPER_ILS),
     get_vox_xnrpn(vox, NRPN_XPER_IWS)); */
}

/*
** trigger a morse timing update when any of the required values changes.
*/
static void nrpn_recompute_morse(int nrpn, int _) {
  // Serial.printf("recompute_morse setting handler\n");
  after_idle(nrpn_update_keyer_timing);
}

/*
** expand mixer shorthand bits to individual mixer channel nrpns
** nrpn == NRPN_MIX_ENABLE then set left and right according to bits
** nrpn == NRPN_MIX_ENABLE_L then set left according to bits
** nrpn == NRPN_MIX_ENABLE_R then set right according to bits
** in each case the mask has 12 bits and ranges from 0 to 4095
** in each case we have 4 bits for usb, i2s, and hdw out mixers
** the order of the mixers and enables is:
** usb: L0 L1 L2 L3 R0 R1 R2 R3
** i2s: L0 L1 L2 L3 R0 R1 R2 R3
** hdw: L0 L1 L2 L3 R0 R1 R2 R3
** so, bit i, counting from 0 at the msb end of the value, 
** addresses left mixer channel (i%4)+(i/4)*8
** and addresses right mixer channel (i%4)+(i/4)*8+4
*/
static void nrpn_recompute_mixer_enables(int nrpn, int _) {
  const int value = nrpn_get(nrpn);
  for (int i = 0; i < 12; i += 1) {
    const int l = (i%4)+(i/4)*8;
    const int r = l+4;
    const int v = (value & (1<<(11-i))) != 0;
    if (nrpn == NRPN_MIX_ENABLE) {
      nrpn_set(NRPN_MIXER2+l, v);
      nrpn_set(NRPN_MIXER2+r, v);
    } else if (nrpn == NRPN_MIX_ENABLE_L) {
      nrpn_set(NRPN_MIXER2+l, v);
    } else if (nrpn == NRPN_MIX_ENABLE_R) {
      nrpn_set(NRPN_MIXER2+r, v);
    } else {
      Serial.printf("bad nrpn %d in nrpn_recompute_mixer_enables\n");
    }
  }
}

/* 
** EEPROM functions.  Disabled.
*/
static void nrpn_write_eeprom(int nrpn, int _) {
#if 0
  hasak_eeprom_image_t data;
  int error = 0;
  for (int i = 0; i < 2; i += 1)
    data.header1[i] = data.header2[i] = data.header3[i] = hasak.header[i];
  for (int i = 0; i < NRPN_LAST; i += 1)
    data.nrpns[i] = nrpn_get(i);
  for (unsigned i = 0; i < sizeof(data.msgs); i += 1)
    data.msgs[i] = hasak.msgs[i];
  EEPROM.put(0, data);
  /* verify */
  for (int i = 0; i < NRPN_LAST; i += 1) {
    int16_t value;
    EEPROM.get(sizeof(data.header1)+i*sizeof(int16_t), value);
    if (value != data.nrpn[i]) {
      Serial.printf("nrpn_write_eeprom hasak.nrpn[%d] == %d != %d\n", nrpn, value, hasak.nrpn[nrpn]);
      ++error;
    }
  }
  for (unsigned i = 0; i < sizeof(hasak.msgs); i += 1) {
    int8_t value;
    EEPROM.get(sizeof(data.header1)+sizeof(data.nrpn)+sizeof(data.header2)+i, value);
    if (value != data.msgs[i]) {
      Serial.printf("nrpn_write_eeprom hasak.msgs[%d] == %d != %d\n", i, value, hasak.msgs[i]);
    }
  }
  nrpn_send(nrpn, error ? 0 : 1);
#endif
}

static void nrpn_read_eeprom(int nrpn, int _) {
#if 0
  hasak_eeprom_image_t data;
  EEPROM.get(0, data);
  for (int i = 0; i < 2; i += 1;)
    if (data.header1[i] != data.header2[i] || 
	data.header2[i] != data.header3[i] || 
	data.header3[i] != hasak.header[i]) {
      /* signal failure */
      nrpn_send(nrpn, 0);
      return;
    }
  /* apply nrpns */
  for (int n = 0; n < NRPN_LAST; n += 1)
    nrpn_set(n, data.nrpns[n]);
  /* copy msgs */
  for (int i = 0; i < sizeof(data.msgs); i += 1)
    hasak.msgs[i] = data.msgs[i];
  /* signal success */
  nrpn_send(nrpn, 1);
#endif
}

/*
** echo all nrpn values to usb, slowly
*/
static int nrpn_echo_index;

static void nrpn_echo_all_step(int _, int  __) {
  if (nrpn_echo_index < NRPN_LAST_SAVED) {
    if (nrpn_get(nrpn_echo_index) != VAL_NOT_SET)
      nrpn_send(nrpn_echo_index, nrpn_get(nrpn_echo_index));
    nrpn_echo_index += 1;
    after_idle(nrpn_echo_all_step);
  } else {
    nrpn_send(NRPN_ECHO_ALL, 0);
    nrpn_echo_index = -1;
  }
}

static void nrpn_echo_all(int nrpn, int _) {
  nrpn_echo_index = NRPN_SAVED;
  after_idle(nrpn_echo_all_step);
}

/*
** feed the text keyers
*/
static void nrpn_send_text(int nrpn, int _) {
  const int value = nrpn_get(nrpn);
  if (nrpn == NRPN_SEND_TEXT) {
    note_set(NOTE_TXT_TEXT, value&127);
    // keyer_text.send_text(value&127);
  } else if (nrpn == NRPN_SEND_TEXT2) {
    note_set(NOTE_TXT_TEXT2, value&127);
    // keyer_text2.send_text(value&127);
  } else {
    Serial.printf("unexpected nrpn %d in nrpn_send_text\n", nrpn);
  }
  nrpn_send(nrpn, value&127);
}

/*
** maintain the message buffers
*/
static void nrpn_msg_handler(int nrpn, int _) {
  const int value = nrpn_get(nrpn);
  if (nrpn == NRPN_MSG_INDEX) { 
    hasak.index = value%sizeof(hasak.msgs);
    nrpn_send(nrpn, hasak.index);
  } else if (nrpn == NRPN_MSG_WRITE) {
    nrpn_send(nrpn, value&127);
    hasak.msgs[hasak.index++] = value&127;
    hasak.index %= sizeof(hasak.msgs);
  } else if (nrpn == NRPN_MSG_READ) { 
    nrpn_send(nrpn, hasak.msgs[hasak.index++]);
    hasak.index %= sizeof(hasak.msgs);
  } else {
    Serial.printf("unexpected nrpn %d in nrpn_msg_handler\n", nrpn);
  }
}
    
static void nrpn_query(int nrpn, int _) {
  const int value = nrpn_get(nrpn);
  if (nrpn_is_valid(value) && nrpn_get(value) != VAL_NOT_SET)
    nrpn_send(nrpn, nrpn_get(value));
}
       
static void nrpn_unset_listener(int nrpn, int _) {
  const int value = nrpn_get(nrpn);
  if (nrpn_is_valid(value))
    nrpn_set(nrpn, VAL_NOT_SET);
}

static void nrpn_id_json(int nrpn, int _) {
  string_id_json(json_string, sizeof(json_string)-1);
}

/*
** this where we initialize the keyer nrpns
*/

static void nrpn_set_default(void) {

  nrpn_set(NRPN_CHANNEL, KYR_CHANNEL);
  nrpn_set(NRPN_INPUT_ENABLE, 1);
  nrpn_set(NRPN_OUTPUT_ENABLE, 1);
  nrpn_set(NRPN_ECHO_ENABLE, 1);
  nrpn_set(NRPN_LISTENER_ENABLE, 1);
  
  nrpn_set(NRPN_PIN_ENABLE, 1);
  nrpn_set(NRPN_POUT_ENABLE, 1);
  nrpn_set(NRPN_PADC_ENABLE, 1);
  nrpn_set(NRPN_ST_ENABLE, KYR_ENABLE_ST);
  nrpn_set(NRPN_TX_ENABLE, KYR_ENABLE_TX);
  nrpn_set(NRPN_IQ_ENABLE, 0);
  nrpn_set(NRPN_PTT_REQUIRE, 0);
  nrpn_set(NRPN_RKEY_ENABLE, 1);
  nrpn_set(NRPN_CW_AUTOPTT, 1);
  nrpn_set(NRPN_ECHO_ENABLE, 1);
  nrpn_set(NRPN_RX_MUTE, 0);
  nrpn_set(NRPN_MIC_HWPTT, 0);
  nrpn_set(NRPN_CW_HWPTT, 1);
  nrpn_set(NRPN_HDW_IN_ENABLE, 1);
  nrpn_set(NRPN_HDW_OUT_ENABLE, 1);

  nrpn_set(NRPN_VOLUME, 0);
  nrpn_set(NRPN_LEVEL, 0);	/* dB/10, centiBel */
  nrpn_set(NRPN_IQ_LEVEL, 0);
  nrpn_set(NRPN_I2S_LEVEL, 0);
  nrpn_set(NRPN_HDW_LEVEL, 0);
  nrpn_set(NRPN_ST_BALANCE, 0);
  nrpn_set(NRPN_IQ_BALANCE, 0);

  /* keyer defaults */
  nrpn_set(NRPN_TONE, 600);	/* Hz */
  nrpn_set(NRPN_SPEED, 18);	/* wpm */
  nrpn_set(NRPN_SPEED_FRAC, 0);	/* wpm 1/128ths */
  nrpn_set(NRPN_WEIGHT, 50);	/* percent */
  nrpn_set(NRPN_RATIO, 50);	/* percent */
  nrpn_set(NRPN_COMP,ms_to_samples(0));	/* 0 samples */
  nrpn_set(NRPN_FARNS, 0);	/* wpm */
  
  nrpn_set(NRPN_HEAD_TIME, ms_to_samples(0));	// 0 ms
  nrpn_set(NRPN_TAIL_TIME, ms_to_samples(0));	// 0 ms
  nrpn_set(NRPN_HANG_TIME, 0);	/* 0 dits */
  nrpn_set(NRPN_RISE_TIME, ms_to_samples(5));	// 5.0 ms
  nrpn_set(NRPN_FALL_TIME, ms_to_samples(5));	// 5.0 ms
  nrpn_set(NRPN_RISE_RAMP, VAL_RAMP_HANN);
  nrpn_set(NRPN_FALL_RAMP, VAL_RAMP_HANN);
  nrpn_set(NRPN_PAD_MODE, VAL_MODE_A);
  nrpn_set(NRPN_PAD_SWAP, 0);
  nrpn_set(NRPN_PAD_ADAPT, VAL_ADAPT_NORMAL);
  nrpn_set(NRPN_AUTO_ILS, 1);
  nrpn_set(NRPN_AUTO_IWS, 0);
  nrpn_set(NRPN_PAD_KEYER, VAL_KEYER_VK6PH);
  // nrpn_set(NRPN_PAD_KEYER, VAL_KEYER_AD5DZ);
  // nrpn_set(NRPN_PAD_KEYER, VAL_KEYER_ND7PA);
  // nrpn_set(NRPN_PAD_KEYER, VAL_KEYER_K1EL);

  nrpn_set(NRPN_ACTIVE_ST, NOTE_ST_NONE);
  nrpn_set(NRPN_MIXER_SLEW_RAMP, VAL_RAMP_HANN);
  nrpn_set(NRPN_MIXER_SLEW_TIME, 128);
  nrpn_set(NRPN_FREQ_SLEW_RAMP, VAL_RAMP_HANN);
  nrpn_set(NRPN_FREQ_SLEW_TIME,	128);
  nrpn_set(NRPN_PIN_DEBOUNCE, 1000);
  nrpn_set(NRPN_POUT_LOGIC, 1);
  nrpn_set(NRPN_PADC_RATE, 10);
  xnrpn_set(NRPN_XIQ_FREQ, 600);
  nrpn_set(NRPN_IQ_USB, 1);

  nrpn_set(NRPN_PIN0_PIN, KYR_L_PAD_PIN);  // left paddle pin
  nrpn_set(NRPN_PIN1_PIN, KYR_R_PAD_PIN);  // right paddle pin
  nrpn_set(NRPN_PIN2_PIN, KYR_S_KEY_PIN);  // straight key pin
  nrpn_set(NRPN_PIN3_PIN, KYR_EXT_PTT_PIN);  // external ptt switch pin
  nrpn_set(NRPN_PIN4_PIN, 127);
  nrpn_set(NRPN_PIN5_PIN, 127);
  nrpn_set(NRPN_PIN6_PIN, 127);
  nrpn_set(NRPN_PIN7_PIN, 127);

  nrpn_set(NRPN_POUT0_PIN, KYR_KEY_OUT_PIN); // key out, maybe ptt out
  nrpn_set(NRPN_POUT1_PIN, KYR_PTT_OUT_PIN); // ptt out, maybe key out
  nrpn_set(NRPN_POUT2_PIN, 127);
  nrpn_set(NRPN_POUT3_PIN, 127);
  nrpn_set(NRPN_POUT4_PIN, 127);
  nrpn_set(NRPN_POUT5_PIN, 127);
  nrpn_set(NRPN_POUT6_PIN, 127);
  nrpn_set(NRPN_POUT7_PIN, 127);

  nrpn_set(NRPN_PADC0_PIN, KYR_VOLUME_POT);
  nrpn_set(NRPN_PADC0_NRPN, VAL_PADC_VOLUME);
  nrpn_set(NRPN_PADC1_PIN, KYR_ST_VOL_POT);
  nrpn_set(NRPN_PADC1_NRPN, VAL_PADC_LEVEL);
  nrpn_set(NRPN_PADC2_PIN, KYR_ST_FREQ_POT);
  nrpn_set(NRPN_PADC2_NRPN, VAL_PADC_TONE);
  nrpn_set(NRPN_PADC3_PIN, KYR_SPEED_POT);
  nrpn_set(NRPN_PADC3_NRPN, VAL_PADC_SPEED);
  nrpn_set(NRPN_PADC4_PIN, 127);
  nrpn_set(NRPN_PADC4_NRPN, -1);
  nrpn_set(NRPN_PADC5_PIN, 1);
  nrpn_set(NRPN_PADC5_NRPN, -1);
  nrpn_set(NRPN_PADC6_PIN, 127);
  nrpn_set(NRPN_PADC6_NRPN, -1);
  nrpn_set(NRPN_PADC7_PIN, 127);
  nrpn_set(NRPN_PADC7_NRPN, -1);

  /* morse code table */
  /* change this to NOTSET, pull NOTSET values from morse[i-NRPN_MORSE] */
  for (int i = 0; i < 64; i += 1) nrpn_set(NRPN_MORSE+i, cwmorse[i]);

  /* output mixers */
  for (int i = NRPN_MIXER; i < NRPN_MIXER+24; i += 1) nrpn_set(i, 0); /* 0 dB */

  /* output mixers enable */
  for (int i = NRPN_MIXER2; i < NRPN_MIXER2+24; i += 1) nrpn_set(i, 0); /* muted */

  nrpn_set(NRPN_MIX_ENABLE,   0b001011001100); // enables left and right mixers according to 12 bits.IQ to usb, usb+sidetone to i2s and hdw
  nrpn_set(NRPN_MIX_ENABLE_L, 0b001011001100); // enables left mixers according to 12 bits.
  nrpn_set(NRPN_MIX_ENABLE_R, 0b001011001100); // enables right mixers according to 12 bits.

  /* codec */
  nrpn_set(NRPN_VOLUME, 0);
  nrpn_set(NRPN_INPUT_SELECT, 0);
  nrpn_set(NRPN_INPUT_LEVEL, 0);

}

static void nrpn_set_default_shim(int nrpn, int _) {
  nrpn_set_default();
}

static void nrpn_setup(void) {
  /* bootstrap controller */
  nrpn_listen(NRPN_ID_JSON, nrpn_id_json);

  /* morse timing update, listen for changes */
  nrpn_listen(NRPN_SPEED, nrpn_recompute_morse);
  nrpn_listen(NRPN_SPEED_FRAC, nrpn_recompute_morse);
  nrpn_listen(NRPN_WEIGHT, nrpn_recompute_morse);
  nrpn_listen(NRPN_RATIO, nrpn_recompute_morse);
  nrpn_listen(NRPN_COMP, nrpn_recompute_morse);
  nrpn_listen(NRPN_FARNS, nrpn_recompute_morse);

  /* mixer enable update, listen for changes */
  nrpn_listen(NRPN_MIX_ENABLE, nrpn_recompute_mixer_enables);
  nrpn_listen(NRPN_MIX_ENABLE_L, nrpn_recompute_mixer_enables);
  nrpn_listen(NRPN_MIX_ENABLE_L, nrpn_recompute_mixer_enables);

  /* commands */
  nrpn_listen(NRPN_WRITE_EEPROM, nrpn_write_eeprom);
  nrpn_listen(NRPN_READ_EEPROM, nrpn_read_eeprom);
  nrpn_listen(NRPN_SET_DEFAULT, nrpn_set_default_shim);
  nrpn_listen(NRPN_ECHO_ALL, nrpn_echo_all);
  nrpn_listen(NRPN_SEND_TEXT, nrpn_send_text);
  nrpn_listen(NRPN_SEND_TEXT2, nrpn_send_text);
  nrpn_listen(NRPN_MSG_INDEX, nrpn_msg_handler);
  nrpn_listen(NRPN_MSG_WRITE, nrpn_msg_handler);
  nrpn_listen(NRPN_MSG_READ, nrpn_msg_handler);

  /* information set in the nrpn array so it will be echoed */
  nrpn_set(NRPN_NRPN_SIZE, KYR_NRPN_SIZE);
  nrpn_set(NRPN_MSG_SIZE, KYR_MSG_SIZE);
  nrpn_set(NRPN_SAMPLE_RATE, KYR_SAMPLE_RATE);
  nrpn_set(NRPN_EEPROM_LENGTH, KYR_EEPROM_LENGTH);;
  nrpn_set(NRPN_ID_CPU, KYR_ID_CPU);
  nrpn_set(NRPN_ID_CODEC, codec_identify());
}

//static void nrpn_loop(void) {}

//#ifdef __cplusplus
//}
//#endif
