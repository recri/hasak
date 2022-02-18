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
#ifdef __cplusplus
extern "C" {
#endif

/*
** access to nrpns and xnrpns
*/
/* report invalid access */
static int invalid_nrpn(const int nrpn, const char *who) {
  Serial.printf("invalid nrpn %d in %s\n", nrpn, who);
  return -1;
}

/* report invalid access */
static int invalid_xnrpn(const int nrpn, const char *who) {
  Serial.printf("invalid xnrpn %d in %s\n", nrpn, who);
  return -1;
}

/* valid nrpn test */
static inline int nrpn_is_valid(const int nrpn) {
  return midi.nrpn_is_valid(nrpn);
}

/* fetch a nrpn */
static inline int nrpn_get(const int nrpn) { 
  return nrpn_is_valid(nrpn) ? midi.nrpn_get(nrpn) : invalid_nrpn(nrpn, "nrpn_get");
}

/* fetch an xnrpn */  
static inline int xnrpn_get(const int nrpn) {
  return nrpn_is_valid(nrpn+1) ? (midi.nrpn_get(nrpn)<<14)|midi.nrpn_get(nrpn+1) : invalid_xnrpn(nrpn, "xnrpn_get");
}

/* set a nrpn */
static inline void nrpn_set(const int nrpn, const int value) {
  if (nrpn_is_valid(nrpn))
    midi.nrpn_set(nrpn, value);
  else
    invalid_nrpn(nrpn, "nrpn_set");
}

/* set an xnrpn */
static inline void xnrpn_set(const int nrpn, const int value) {
  if (nrpn_is_valid(nrpn+1)) {
    midi.nrpn_set(nrpn, (value>>14)&0x3fff);
    midi.nrpn_set(nrpn+1, value&0x3fff);
  } else
    invalid_xnrpn(nrpn, "xnrpn_set");
}

/* define a nrpn */
static void nrpn_define(const int nrpn, const int value, 
			const int input_enable, const int output_enable, 
			const int echo_enable, const int read_only) {
  if (nrpn_is_valid(nrpn)) {
    midi.nrpn_flags_set(midi_flags_encode(input_enable, output_enable, echo_enable, read_only));
    nprn_set(nrpn, value);
  } else
    nrpn_invalid(nrpn, "nrpn_define");
}

/*
** Update the keyer timing, specified by speed, weight, ratio, comp, and farnsworth,
** and produce the samples per dit, dah, ies, ils, and iws.
*/
static void nrpn_update_keyer_timing(void) {
  const int wordDits = 50;
  const int sampleRate = AUDIO_SAMPLE_RATE;
  const float wpm = get_nrpn(KYRP_SPEED)+get_nrpn(KYRP_SPEED_FRAC)*7.8125e-3f;
  const int weight = get_nrpn(KYRP_WEIGHT);
  const int ratio = get_nrpn(KYRP_RATIO);
  const int compensation = signed_value(get_nrpn(KYRP_COMP));
  const int farnsworth = get_nrpn(KYRP_FARNS);;
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

// echo the nrpn settings back to the control echo channel
static void nrpn_echo(const int16_t nrpn, const int16_t value) {
  /* if (nrpn == 193) Serial.printf("nrpn_echo sets nrpn[193] to %d\n", value); */
  /* if (nrpn == KYRP_ID_KEYER || nrpn == KYRP_ID_VERSION) Serial.printf("nrpn_echo sends nrpn[%d] as %d\n", nrpn, value); */
  midi_send_nrpn(nrpn, value);
}  

static void nrpn_update_mixer(const int16_t nrpn, const int16_t value) {
  if (nrpn == KYRP_OUT_ENABLE) {
    // Serial.printf("update_mixer(%d, 0x%03x)\n", nrpn, value);
    AudioNoInterrupts();
    hasak.nrpn[nrpn] = value;
    for (int i = KYRP_MIX_USB_L0; i <= KYRP_MIX_HDW_R3; i += 1)
      nrpn_update_mixer(i, get_nrpn(i));
    AudioInterrupts();
  } else if (nrpn >= KYRP_MIX_USB_L0 && nrpn <= KYRP_MIX_HDW_R3) {
    uint16_t index = (nrpn-KYRP_MIXER)/4; /* index of mixer in mix_out */
    uint16_t chan = (nrpn-KYRP_MIXER)%4;  /* channel of the mixer */
    uint16_t mask = 1<<(11-(((index>>1)<<2)+chan)); /* bit enabling channel */
    float gain = ((mask&get_nrpn(KYRP_OUT_ENABLE)) ? 1 : 0) * tenthdbtolinear(signed_value(value));
    //Serial.printf("update_mixer(%d, %d) index %d chan %d mask %03x gain %f\n", 
    //		  nrpn, value, index, chan, mask, gain);
   mix_out[index]->gain(chan, gain);
  } else {
    Serial.printf("unexpected nrpn %d in nrpn_update_mixer\n", nrpn);
  }
}

static void nrpn_report(const char *name, const int16_t oldval, const int16_t newval) {
  Serial.printf("%s %d to %d\n", name, oldval, newval);
}

/*
** this where we initialize the keyer, for the time being
** these are set to match what the controller starts with.
** synchronizing with the controller will be an issue.
*/
#include "morse_table.h"

static void nrpn_set_defaults(void) {
  nrpn_set(KYRP_ID_KEYER, KYRC_IDENT);
  nrpn_set(KYRP_ID_VERSION, KYRC_VERSION);

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
#if defined(KYRC_ENABLE_TX)
  nrpn_set(KYRP_TX_ENABLE, 1);
#else
  nrpn_set(KYRP_TX_ENABLE, 0);
#endif
  nrpn_set(KYRP_ST_ENABLE, 1);
  nrpn_set(KYRP_IQ_BALANCE, 0);
  nrpn_set(KYRP_ST_PAN, 0);
  nrpn_set(KYRP_DEBOUNCE, 1000);
  nrpn_set(KYRP_REMOTE_KEY, 1);
  
  nrpn_set(KYRP_CHAN_CC, KYRD_CHANNEL);
  nrpn_set(KYRP_CHAN_NOTE, KYRD_CHANNEL);
  nrpn_set(KYRP_CHAN_NRPN, KYRD_CHANNEL);

  nrpn_set(KYRP_NOTE_KEY_OUT, KYRN_KEY_OUT);
  nrpn_set(KYRP_NOTE_PTT_OUT, KYRN_PTT_OUT);
  nrpn_set(KYRP_NOTE_TUNE, KYRN_TUNE);
  nrpn_set(KYRP_NOTE_L_PAD, KYRN_L_PAD);
  nrpn_set(KYRP_NOTE_R_PAD, KYRN_R_PAD);
  nrpn_set(KYRP_NOTE_S_KEY, KYRN_S_KEY);
  nrpn_set(KYRP_NOTE_EXT_PTT, KYRN_EXT_PTT);
  nrpn_set(KYRP_NOTE_ENABLE, KYR_ENABLE);

  nrpn_set(KYRP_ADC_ENABLE, 1);
  nrpn_set(KYRP_ADC0_CONTROL, KYRV_ADC_NOTHING);
  nrpn_set(KYRP_ADC1_CONTROL, KYRV_ADC_VOLUME);
  nrpn_set(KYRP_ADC2_CONTROL, KYRV_ADC_LEVEL);
  nrpn_set(KYRP_ADC3_CONTROL, KYRV_ADC_TONE);
  nrpn_set(KYRP_ADC4_CONTROL, KYRV_ADC_SPEED);

  /* morse code table */
  /* change this to NOTSET, pull NOTSET values from morse[i-KYRP_MORSE] */
  for (int i = KYRP_MORSE; i < KYRP_MORSE+64; i += 1) 
    nrpn_set(i, morse[i-KYRP_MORSE]);

  /* output mixers */
  nrpn_set(KYRP_OUT_ENABLE, 0b001011001100); /* IQ to usb, usb+sidetone to i2s and hdw */
  for (int i = KYRP_MIXER; i < KYRP_MIXER+24; i += 1) nrpn_set(i, 0); /* 0 dB */
  /* output mixers enable */

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
  // nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_AD5DZ);
  nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_VK6PH);
  // nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_ND7PA);
  // nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_K1EL);
  nrpn_set(KYRP_AUTO_ILS, 1);
  nrpn_set(KYRP_AUTO_IWS, 0);

  /* keyer defaults - per voice */
  nrpn_set(KYRP_TONE, 600);	/* Hz */
  nrpn_set(KYRP_LEVEL, 0);	/* db/4 */
  nrpn_set(KYRP_SPEED, 18);	/* wpm */
  nrpn_set(KYRP_SPEED_FRAC, 0);	/* wpm 1/128ths */
  nrpn_set(KYRP_WEIGHT, 50);	/* percent */
  nrpn_set(KYRP_RATIO, 50);	/* percent */
  nrpn_set(KYRP_COMP,ms_to_samples(0));	/* 0 samples */
  nrpn_set(KYRP_FARNS, 0);	/* wpm */
}

#include "EEPROM.h"

static void nrpn_write_eeprom(void) {
  EEPROM.put(0, hasak.header); /* header */
  EEPROM.put(sizeof(hasak.header), hasak.nrpn); /* nrpns */
  EEPROM.put(sizeof(hasak.header)+sizeof(hasak.nrpn), hasak.header); /* header */
  EEPROM.put(sizeof(hasak.header)+sizeof(hasak.nrpn)+sizeof(hasak.header), hasak.msgs); /* stored messages */

  /* verify */
  for (int nrpn = 0; nrpn < KYRP_LAST; nrpn += 1) {
    int16_t value;
    EEPROM.get(sizeof(hasak.header)+nrpn*sizeof(int16_t), value);
    if (value != hasak.nrpn[nrpn])
      Serial.printf("nrpn_write_eeprom hasak.nrpn[%d] == %d != %d\n", nrpn, value, hasak.nrpn[nrpn]);
  }
  for (unsigned i = 0; i < sizeof(hasak.msgs); i += 1) {
    int8_t value;
    EEPROM.get(sizeof(hasak.header)+sizeof(hasak.nrpn)+sizeof(hasak.header)+i, value);
    if (value != hasak.msgs[i])
      Serial.printf("nrpn_write_eeprom hasak.msgs[%d] == %d != %d\n", i, value, hasak.msgs[i]);
  }
}

static int nrpn_echo_index;
static void nrpn_echo_all(void) {
  nrpn_echo_index = 0;
}

static int nrpn_read_eeprom(void) {
  uint16_t header[3];
  EEPROM.get(0, header);
  if (header[0] != hasak.header[0] || header[1] != hasak.header[1] || header[2] != hasak.header[2]) return 0;
  EEPROM.get(sizeof(header)+sizeof(hasak.nrpn), header);
  if (header[0] != hasak.header[0] || header[1] != hasak.header[1] || header[2] != hasak.header[2]) return 0;
  EEPROM.get(sizeof(header), hasak.nrpn);
  EEPROM.get(sizeof(header)+sizeof(hasak.nrpn)+sizeof(header), hasak.msgs);
  /* apply nrpns */
  for (int nrpn = 0; nrpn < KYRP_LAST; nrpn += 1)
    if (hasak.nrpn[nrpn] != KYRV_NOT_SET)
      nrpn_set(nrpn, hasak.nrpn[nrpn]);
  return 1;
}

static int nrpn_eeprom_length(void) {
  return EEPROM.length();
}

static int identifyCPU(void) {
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
  
/*
** set a NRPN value.
** all are copied into the hasak.nrpn[] array.
** some are transformed into sample unit timings.
** some trigger calls to the sgtl5000 control interface.
** none are sent into the audio library processing loop,
** it pulls parameters as necessary.
*/
static void nrpn_set(const int16_t nrpn, const int16_t value) {
  /* Serial.printf("nprn_set(%d, %d)\n", nrpn, value); */
  switch (nrpn) {
  case KYRP_VOLUME:
  case KYRP_INPUT_SELECT:
  case KYRP_INPUT_LEVEL:
    codec_nrpn_set(nrpn, value); hasak.nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_BUTTON_0:
  case KYRP_BUTTON_1:
  case KYRP_BUTTON_2:
  case KYRP_BUTTON_3:
  case KYRP_BUTTON_4:
  case KYRP_PTT_ENABLE:
  case KYRP_IQ_ENABLE: 
  case KYRP_TX_ENABLE:
  case KYRP_IQ_ADJUST:
  case KYRP_ST_ENABLE:
  case KYRP_IQ_BALANCE:
  case KYRP_ST_PAN:

  case KYRP_DEBOUNCE:
  case KYRP_HEAD_TIME:
  case KYRP_TAIL_TIME:
  case KYRP_HANG_TIME:
  case KYRP_RISE_TIME:
  case KYRP_FALL_TIME:
  case KYRP_RISE_RAMP:
  case KYRP_FALL_RAMP:
  case KYRP_PAD_MODE:
  case KYRP_PAD_SWAP:
  case KYRP_PAD_ADAPT:
  case KYRP_AUTO_ILS:
  case KYRP_AUTO_IWS:
  case KYRP_PAD_KEYER:

  case KYRP_CHAN_CC:
  case KYRP_CHAN_NOTE:
  case KYRP_CHAN_NRPN:
  case KYRP_NOTE_L_PAD:
  case KYRP_NOTE_R_PAD:
  case KYRP_NOTE_S_KEY:
  case KYRP_NOTE_EXT_PTT:
  case KYRP_NOTE_KEY_OUT:
  case KYRP_NOTE_PTT_OUT:
  case KYRP_NOTE_TUNE:
  case KYRP_ADC0_CONTROL:
  case KYRP_ADC1_CONTROL:
  case KYRP_ADC2_CONTROL:
  case KYRP_ADC3_CONTROL:
  case KYRP_ADC4_CONTROL:
  case KYRP_ADC_ENABLE:
  case KYRP_REMOTE_KEY:
  case KYRP_NOTE_ENABLE:
    hasak.nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
    
  case KYRP_OUT_ENABLE:
    nrpn_update_mixer(nrpn, value); hasak.nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;

    // case KYRP_MORSE+(0..63): see default case
    
    // case KYRP_MIXER+(0..23): see default case

  case KYRP_TONE:	    
  case KYRP_LEVEL: 
    hasak.nrpn[nrpn] = value; nrpn_echo(nrpn, value); break; \
  case KYRP_SPEED:
  case KYRP_WEIGHT:
  case KYRP_RATIO:
  case KYRP_FARNS:
  case KYRP_COMP:
  case KYRP_SPEED_FRAC:							\
    hasak.nrpn[nrpn] = value; nrpn_update_keyer_timing(); nrpn_echo(nrpn, value); break;
    
    /* commands, keep no state in hasak.nrpn */
  case KYRP_WRITE_EEPROM:
    nrpn_write_eeprom();
    nrpn_echo(nrpn, 1);
    break;
  case KYRP_READ_EEPROM: 
    nrpn_echo(nrpn, nrpn_read_eeprom());
    break;
  case KYRP_SET_DEFAULT:
    nrpn_set_defaults();
    nrpn_echo(nrpn, 1);
    break;
  case KYRP_ECHO_ALL:
    nrpn_echo_all();
    break;
  case KYRP_SEND_WINK: 
    note_set(KYRN_TXT_WINK, value&127);
    // keyer_text_wink.send_text(value&127); 
    nrpn_echo(nrpn, value&127);
    break;
  case KYRP_SEND_KYR: 
    note_set(KYRN_TXT_KYR, value&127);
    // keyer_text_kyr.send_text(value&127);
    nrpn_echo(nrpn, value&127);
    break;
  case KYRP_MSG_INDEX: 
    hasak.index = value%sizeof(hasak.msgs);
    nrpn_echo(nrpn, hasak.index);
    break;
  case KYRP_MSG_WRITE:
    nrpn_echo(nrpn, value&127);
    hasak.msgs[hasak.index++] = value&127;
    hasak.index %= sizeof(hasak.msgs);
    break;
  case KYRP_MSG_READ: 
    nrpn_echo(nrpn, hasak.msgs[hasak.index++]);
    hasak.index %= sizeof(hasak.msgs);
    break;

    /* information only, but set the nrpn array */
  case KYRP_ID_KEYER: hasak.nrpn[nrpn] = KYRC_IDENT; nrpn_echo(nrpn, KYRC_IDENT); break;
  case KYRP_ID_VERSION: hasak.nrpn[nrpn] = KYRC_VERSION; nrpn_echo(nrpn, KYRC_VERSION); break;

    /* information only, cause no side effects */
  case KYRP_NRPN_SIZE: nrpn_echo(nrpn, sizeof(hasak.nrpn)); break;
  case KYRP_MSG_SIZE: nrpn_echo(nrpn, sizeof(hasak.msgs)); break;
  case KYRP_SAMPLE_RATE: nrpn_echo(nrpn, ((uint16_t)AUDIO_SAMPLE_RATE)/100); break;
  case KYRP_EEPROM_LENGTH: nrpn_echo(nrpn, nrpn_eeprom_length()); break;
  case KYRP_ID_CPU: nrpn_echo(nrpn, identifyCPU()); break;
  case KYRP_ID_CODEC: nrpn_echo(nrpn, codec_identify()); break;

  default: 
    if (nrpn >= KYRP_MORSE && nrpn < KYRP_MORSE+64) {
      hasak.nrpn[nrpn] = value; nrpn_echo(nrpn, value);
      break;
    }
    if (nrpn >= KYRP_MIXER && nrpn < KYRP_MIXER+24) {
      nrpn_update_mixer(nrpn, value); hasak.nrpn[nrpn] = value; nrpn_echo(nrpn, value);
      break;
    }
    Serial.printf("uncaught nrpn #%d with value %d\n", nrpn, value);
    break;
  }
}

static void nrpn_setup(void) {
  codec_enable();
  nrpn_set(KYRP_VOLUME, -200);
  nrpn_set_defaults();
}

static void nrpn_loop(void) {
  static uint8_t delay = 0;
  if (nrpn_echo_index >= 0) {
    if ((delay++ & 15) != 0) return; 
    if (nrpn_echo_index < KYRP_LAST) {
      if (hasak.nrpn[nrpn_echo_index] != KYRV_NOT_SET)
	nrpn_echo(nrpn_echo_index, hasak.nrpn[nrpn_echo_index]);
      nrpn_echo_index += 1;
    } else {
      nrpn_echo(KYRP_ECHO_ALL, 0);
      nrpn_echo_index = -1;
    }
  }
}
#ifdef __cplusplus
}
#endif
