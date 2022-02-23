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
static void nrpn_update_keyer_timing(void) {
  const int wordDits = 50;
  const int sampleRate = AUDIO_SAMPLE_RATE;
  const float wpm = nrpn_get(KYRP_SPEED)+nrpn_get(KYRP_SPEED_FRAC)*7.8125e-3f;
  const int weight = nrpn_get(KYRP_WEIGHT);
  const int ratio = nrpn_get(KYRP_RATIO);
  const int compensation = signed_value(nrpn_get(KYRP_COMP));
  const int farnsworth = nrpn_get(KYRP_FARNS);;
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
  xnrpn_set(KYRP_XPER_DIT, ticksPerDit);
  xnrpn_set(KYRP_XPER_DAH, ticksPerDah);
  xnrpn_set(KYRP_XPER_IES, ticksPerIes);
  xnrpn_set(KYRP_XPER_ILS, ticksPerIls);
  xnrpn_set(KYRP_XPER_IWS, ticksPerIws);
  // AudioInterrupts();
  /* Serial.printf("morse_keyer dit %ld, dah %ld, ies %ld, ils %ld, iws %ld\n", 
     get_vox_xnrpn(vox, KYRP_XPER_DIT), get_vox_xnrpn(vox, KYRP_XPER_DAH),
     get_vox_xnrpn(vox, KYRP_XPER_IES), get_vox_xnrpn(vox, KYRP_XPER_ILS),
     get_vox_xnrpn(vox, KYRP_XPER_IWS)); */
}

/*
** trigger a morse timing update when any of the required values changes.
*/
static void nrpn_recompute_morse(int nrpn) {
  after_idle(nrpn_update_keyer_timing);
}

/*
** expand mixer shorthand bits to individual mixer channel nrpns
** nrpn == KYRP_OUT_ENABLE then set left and right according to bits
** nrpn == KYRP_OUT_ENABLE_L then set left according to bits
** nrpn == KYRP_OUT_ENABLE_R then set right according to bits
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
static void nrpn_recompute_mixer_enables(int nrpn) {
  const int value = nrpn_get(nrpn);
  for (int i = 0; i < 12; i += 1) {
    const int l = (i%4)+(i/4)*8;
    const int r = l+4;
    const int v = (value & (1<<(11-i))) != 0;
    if (nrpn == KYRP_OUT_ENABLE) {
      nrpn_set(KYRP_MIXER2+l, v);
      nrpn_set(KYRP_MIXER2+r, v);
    } else if (nrpn == KYRP_OUT_ENABLE_L) {
      nrpn_set(KYRP_MIXER2+l, v);
    } else if (nrpn == KYRP_OUT_ENABLE_R) {
      nrpn_set(KYRP_MIXER2+r, v);
    } else {
      Serial.printf("bad nrpn %d in nrpn_recompute_mixer_enables\n");
    }
  }
}

/* 
** EEPROM functions.  Disabled.
*/
static void nrpn_write_eeprom(int nrpn) {
#if 0
  hasak_eeprom_image_t data;
  int error = 0;
  for (int i = 0; i < 2; i += 1)
    data.header1[i] = data.header2[i] = data.header3[i] = hasak.header[i];
  for (int i = 0; i < KYRP_LAST; i += 1)
    data.nrpns[i] = nrpn_get(i);
  for (unsigned i = 0; i < sizeof(data.msgs); i += 1)
    data.msgs[i] = hasak.msgs[i];
  EEPROM.put(0, data);
  /* verify */
  for (int i = 0; i < KYRP_LAST; i += 1) {
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
  nrpn_echo(nrpn, error ? 0 : 1);
#endif
}

static void nrpn_read_eeprom(int nrpn) {
#if 0
  hasak_eeprom_image_t data;
  EEPROM.get(0, data);
  for (int i = 0; i < 2; i += 1;)
    if (data.header1[i] != data.header2[i] || 
	data.header2[i] != data.header3[i] || 
	data.header3[i] != hasak.header[i]) {
      /* signal failure */
      nrpn_echo(nrpn, 0);
      return;
    }
  /* apply nrpns */
  for (int n = 0; n < KYRP_LAST; n += 1)
    nrpn_set(n, data.nrpns[n]);
  /* copy msgs */
  for (int i = 0; i < sizeof(data.msgs); i += 1)
    hasak.msgs[i] = data.msgs[i];
  /* signal success */
  nrpn_echo(nrpn, 1);
#endif
}

/*
** echo all nrpn values to usb, slowly
*/
static int nrpn_echo_index;

static void nrpn_echo_all_step(void) {
  if (nrpn_echo_index < KYRP_LAST) {
    if (nrpn_get(nrpn_echo_index) != KYRV_NOT_SET)
      nrpn_echo(nrpn_echo_index, nrpn_get(nrpn_echo_index));
    nrpn_echo_index += 1;
    after_idle(nrpn_echo_all_step);
  } else {
    nrpn_echo(KYRP_ECHO_ALL, 0);
    nrpn_echo_index = -1;
  }
}

static void nrpn_echo_all(int nrpn) {
  nrpn_echo_index = 0;
  nrpn_echo_all_step();
}

/*
** feed the text keyers
*/
static void nrpn_send_text(int nrpn) {
  const int value = nrpn_get(nrpn);
  if (nrpn == KYRP_SEND_TEXT) {
    note_set(KYRN_TXT_TEXT, value&127);
    // keyer_text.send_text(value&127);
  } else if (nrpn == KYRP_SEND_TEXT2) {
    note_set(KYRN_TXT_TEXT2, value&127);
    // keyer_text2.send_text(value&127);
  } else {
    Serial.printf("unexpected nrpn %d in nrpn_send_text\n", nrpn);
  }
  nrpn_echo(nrpn, value&127);
}

/*
** maintain the message buffers
*/
static void nrpn_msg_handler(int nrpn) {
  const int value = nrpn_get(nrpn);
  if (nrpn == KYRP_MSG_INDEX) { 
    hasak.index = value%sizeof(hasak.msgs);
    nrpn_echo(nrpn, hasak.index);
  } else if (nrpn == KYRP_MSG_WRITE) {
    nrpn_echo(nrpn, value&127);
    hasak.msgs[hasak.index++] = value&127;
    hasak.index %= sizeof(hasak.msgs);
  } else if (nrpn == KYRP_MSG_READ) { 
    nrpn_echo(nrpn, hasak.msgs[hasak.index++]);
    hasak.index %= sizeof(hasak.msgs);
  } else {
    Serial.printf("unexpected nrpn %d in nrpn_msg_handler\n", nrpn);
  }
}
    
/*
** values for storing as readonly information.
*/

static int nrpn_eeprom_length(void) {
  return EEPROM.length();
}

static int nrpn_nrpn_size(void) {
  return KYR_N_NRPN*sizeof(int16_t);
}

static int nrpn_msg_size(void) {
  return nrpn_eeprom_length()-3*3*sizeof(int16_t)-nrpn_nrpn_size();
}

static int nrpn_sample_rate(void) {
  return ((uint16_t)AUDIO_SAMPLE_RATE)/100;
}

static int nrpn_id_cpu(void) {
#if defined(TEENSY30)
  return 30;
#elif defined(TEENSY31)
  return 31;
#elif defined(TEENSY32)
  return 32;
#elif defined(TEENSY35)
  return 35;
#elif defined(TEENSY36)
  return 36;
#elif defined(TEENSY40)
  return 40;
#elif defined(TEENSY41)
  return 41;
#else
  return 0;
#endif
}

static void nrpn_query(int nrpn) {
  const int value = nrpn_get(nrpn);
  if (nrpn_is_valid(value) && nrpn_get(value) != KYRV_NOT_SET)
    nrpn_echo(nrpn, nrpn_get(value));
}
       
static void nrpn_unset(int nrpn) {
  const int value = nrpn_get(nrpn);
  if (nrpn_is_valid(value))
    nrpn_set(nrpn, KYRV_NOT_SET);
}

static void nrpn_id_json(int nrpn) {
}

static void nrpn_string_handler(int nrpn) {
}


/*
** this where we initialize the keyer nrpns
*/

static void nrpn_set_default(int nrpn) {
  nrpn_set(KYRP_ID_KEYER, KYR_IDENT);
  nrpn_set(KYRP_ID_VERSION, KYR_VERSION);

  nrpn_set(KYRP_VOLUME, 0);
  nrpn_set(KYRP_INPUT_SELECT, 0);
  nrpn_set(KYRP_INPUT_LEVEL, 0);

  /* soft controls */
  nrpn_set(KYRP_BUTTON_0,  6800&KYRV_MASK); /* off */
  nrpn_set(KYRP_BUTTON_1, -2700&KYRV_MASK); /* center */
  nrpn_set(KYRP_BUTTON_2, -1800&KYRV_MASK); /* up */
  nrpn_set(KYRP_BUTTON_3,  -500&KYRV_MASK); /* down */
  nrpn_set(KYRP_BUTTON_4, -2250&KYRV_MASK); /* hey google */
  nrpn_set(KYRP_PTT_ENABLE, 0);
  nrpn_set(KYRP_IQ_ENABLE, 0);
  nrpn_set(KYRP_IQ_ADJUST, 0);
#if defined(KYR_ENABLE_TX)
  nrpn_set(KYRP_TX_ENABLE, 1);
#else
  nrpn_set(KYRP_TX_ENABLE, 0);
#endif
  nrpn_set(KYRP_ST_ENABLE, 1);
  nrpn_set(KYRP_IQ_BALANCE, 0);
  nrpn_set(KYRP_ST_PAN, 0);
  nrpn_set(KYRP_DEBOUNCE, 1000);
  nrpn_set(KYRP_REMOTE_KEY, 1);
  
  nrpn_set(KYRP_CHAN_CC, KYR_CHANNEL);
  nrpn_set(KYRP_CHAN_NOTE, KYR_CHANNEL);
  nrpn_set(KYRP_CHAN_NRPN, KYR_CHANNEL);

  nrpn_set(KYRP_NOTE_KEY_OUT, KYRN_MIDI_OUT_KEY);
  nrpn_set(KYRP_NOTE_PTT_OUT, KYRN_MIDI_OUT_PTT);
  nrpn_set(KYRP_NOTE_TUNE, KYRN_MIDI_IN_TUNE);
  nrpn_set(KYRP_NOTE_L_PAD, KYRN_HW_L_PAD);
  nrpn_set(KYRP_NOTE_R_PAD, KYRN_HW_R_PAD);
  nrpn_set(KYRP_NOTE_S_KEY, KYRN_HW_S_KEY);
  nrpn_set(KYRP_NOTE_EXT_PTT, KYRN_HW_EXT_PTT);
  // nrpn_set(KYRP_NOTE_ENABLE, KYR_ENABLE); obsolete

  nrpn_set(KYRP_ADC_ENABLE, 1);
  nrpn_set(KYRP_ADC0_CONTROL, KYRV_ADC_NOTHING);
  nrpn_set(KYRP_ADC1_CONTROL, KYRV_ADC_VOLUME);
  nrpn_set(KYRP_ADC2_CONTROL, KYRV_ADC_LEVEL);
  nrpn_set(KYRP_ADC3_CONTROL, KYRV_ADC_TONE);
  nrpn_set(KYRP_ADC4_CONTROL, KYRV_ADC_SPEED);

  /* morse code table */
  /* change this to NOTSET, pull NOTSET values from morse[i-KYRP_MORSE] */
  for (int i = 0; i < 64; i += 1) 
    nrpn_set(KYRP_MORSE+i, cwmorse[i]);

  /* output mixers */
  nrpn_set(KYRP_OUT_ENABLE, 0b001011001100); /* IQ to usb, usb+sidetone to i2s and hdw */
  for (int i = KYRP_MIXER; i < KYRP_MIXER+24; i += 1) nrpn_set(i, 0); /* 0 dB */
  /* output mixers enable */
  for (int i = KYRP_MIXER2; i < KYRP_MIXER2+24; i += 1) nrpn_set(i, 0); /* muted */
  /* output mixer enable shorthand */
  nrpn_set(KYRP_OUT_ENABLE, 0b001011001100); // enables left and right mixers according to 12 bits.
  nrpn_set(KYRP_OUT_ENABLE_L, 0b001011001100); // enables left mixers according to 12 bits.
  nrpn_set(KYRP_OUT_ENABLE_R, 0b001011001100); // enables right mixers according to 12 bits.

  /* keyer defaults - common */
  nrpn_set(KYRP_RISE_TIME, ms_to_samples(5));	// 5.0 ms
  nrpn_set(KYRP_FALL_TIME, ms_to_samples(5));	// 5.0 ms
  nrpn_set(KYRP_RISE_RAMP, KYRV_RAMP_HANN);
  nrpn_set(KYRP_FALL_RAMP, KYRV_RAMP_HANN);
  nrpn_set(KYRP_HEAD_TIME, ms_to_samples(0));	// 0 ms
  nrpn_set(KYRP_TAIL_TIME, ms_to_samples(0));	// 0 ms
  nrpn_set(KYRP_HANG_TIME, 0);	/* 0 dits */
  nrpn_set(KYRP_PAD_MODE, KYRV_MODE_A);
  nrpn_set(KYRP_PAD_SWAP, 0);
  nrpn_set(KYRP_PAD_ADAPT, KYRV_ADAPT_NORMAL);
  nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_VK6PH);
  // nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_AD5DZ);
  // nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_ND7PA);
  // nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_K1EL);
  nrpn_set(KYRP_AUTO_ILS, 1);
  nrpn_set(KYRP_AUTO_IWS, 0);

  /* keyer defaults */
  nrpn_set(KYRP_TONE, 600);	/* Hz */
  nrpn_set(KYRP_LEVEL, 0);	/* db/4 */
  nrpn_set(KYRP_SPEED, 18);	/* wpm */
  nrpn_set(KYRP_SPEED_FRAC, 0);	/* wpm 1/128ths */
  nrpn_set(KYRP_WEIGHT, 50);	/* percent */
  nrpn_set(KYRP_RATIO, 50);	/* percent */
  nrpn_set(KYRP_COMP,ms_to_samples(0));	/* 0 samples */
  nrpn_set(KYRP_FARNS, 0);	/* wpm */
  
  /* More defaults */
  nrpn_set(KYRP_N_NRPN, KYR_N_NRPN);
  nrpn_set(KYRP_CHANNEL, KYR_CHANNEL);
  nrpn_set(KYRP_N_NOTE, KYR_N_NOTE);
  nrpn_set(KYRP_N_CTRL, KYR_N_CTRL);
  nrpn_set(KYRP_ACTIVE_ST, KYRN_ST_NONE);

}

static void nrpn_setup(void) {

  /* morse timing update, listen for changes */
  nrpn_listen(KYRP_SPEED, nrpn_recompute_morse);
  nrpn_listen(KYRP_SPEED_FRAC, nrpn_recompute_morse);
  nrpn_listen(KYRP_WEIGHT, nrpn_recompute_morse);
  nrpn_listen(KYRP_RATIO, nrpn_recompute_morse);
  nrpn_listen(KYRP_COMP, nrpn_recompute_morse);
  nrpn_listen(KYRP_FARNS, nrpn_recompute_morse);

  /* mixer enable update, listen for changes */
  nrpn_listen(KYRP_OUT_ENABLE, nrpn_recompute_mixer_enables);
  nrpn_listen(KYRP_OUT_ENABLE_L, nrpn_recompute_mixer_enables);
  nrpn_listen(KYRP_OUT_ENABLE_L, nrpn_recompute_mixer_enables);

  /* inserted commands and info */
  nrpn_listen(KYRP_QUERY, nrpn_query);
  nrpn_listen(KYRP_UNSET, nrpn_unset);
  nrpn_listen(KYRP_ID_JSON, nrpn_id_json);
  nrpn_listen(KYRP_STRING_BEGIN, nrpn_string_handler);
  nrpn_listen(KYRP_STRING_END, nrpn_string_handler);
  nrpn_listen(KYRP_STRING_BYTE, nrpn_string_handler);
  
  /* commands */
  nrpn_listen(KYRP_WRITE_EEPROM, nrpn_write_eeprom);
  nrpn_listen(KYRP_READ_EEPROM, nrpn_read_eeprom);
  nrpn_listen(KYRP_SET_DEFAULT, nrpn_set_default);
  nrpn_listen(KYRP_ECHO_ALL, nrpn_echo_all);
  nrpn_listen(KYRP_SEND_TEXT, nrpn_send_text);
  nrpn_listen(KYRP_SEND_TEXT2, nrpn_send_text);
  nrpn_listen(KYRP_MSG_INDEX, nrpn_msg_handler);
  nrpn_listen(KYRP_MSG_WRITE, nrpn_msg_handler);
  nrpn_listen(KYRP_MSG_READ, nrpn_msg_handler);

  /* information set in the nrpn array so it will be echoed */
  nrpn_set(KYRP_ID_KEYER, KYR_IDENT);
  nrpn_set(KYRP_ID_VERSION, KYR_VERSION);
  nrpn_set(KYRP_NRPN_SIZE, nrpn_nrpn_size());
  nrpn_set(KYRP_MSG_SIZE, nrpn_msg_size());
  nrpn_set(KYRP_SAMPLE_RATE, nrpn_sample_rate());
  nrpn_set(KYRP_EEPROM_LENGTH, nrpn_eeprom_length());;
  nrpn_set(KYRP_ID_CPU, nrpn_id_cpu());
  nrpn_set(KYRP_ID_CODEC, codec_identify());

  /* set the default settings */
  nrpn_set_default(KYRP_SET_DEFAULT);
 /* possibly, probe the EEPROM for saved settings and restore them */
}

static void nrpn_loop(void) {
}

//#ifdef __cplusplus
//}
//#endif
