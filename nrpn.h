/***************************************************************
** NRPN (non-registered parameter) handling.
***************************************************************/
/* array of persistent nrpn values */
int16_t kyr_nrpn[KYRP_LAST];
/* header for eeprom stored nrpn values */
static uint16_t kyr_header[3] = { KYRC_MAGIC, sizeof(kyr_nrpn), KYRC_VERSION };
/* array of bytes used for persistent messages */
static int8_t kyr_msgs[EEPROM_BYTES-sizeof(kyr_nrpn)-sizeof(kyr_header)-sizeof(kyr_header)];
/* index into kyr_msgs for reading and writing contents */
static uint16_t kyr_index = 0;
/* forward reference */
static void nrpn_set(const int16_t nrpn, const int16_t value);

/* set a nrpn without */
static inline void set_nrpn(const int16_t nrpn, const int16_t value) {
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
static inline void set_vox_nrpn(const int16_t vox, const int16_t nrpn, const int16_t value) {
  if (vox >= 0 && vox <= KYR_N_VOX && nrpn >= KYRP_KEYER && nrpn < KYRP_VOX_TUNE)
    set_nrpn(nrpn+vox*KYRP_VOX_OFFSET, value);
}

/*
** Update the keyer timing, specified by speed, weight, ratio, comp, and farnsworth,
** and produce the samples per dit, dah, ies, ils, and iws.
*/
static void nrpn_update_keyer_timing(const int16_t vox) {
  const float wordDits = 50;
  const float sampleRate = AUDIO_SAMPLE_RATE;
  const float wpm = get_vox_speed(vox);
  const float weight = get_vox_weight(vox);
  const float ratio = get_vox_ratio(vox);
  //const float compensation = get_vox_comp(vox);
  const float farnsworth = get_vox_farns(vox);;
  const float ms_per_dit = (1000 * 60) / (wpm * wordDits);
  const float r = (ratio-50)/100.0;
  const float w = (weight-50)/100.0;
  // const float c = compensation / 10.0 / ms_per_dit; /* ms/10 / ms_per_dit */ 
  /* Serial.printf("nrpn_update_keyer sr %f, word %d, wpm %f, weight %d, ratio %d, comp %d, farns %d\n", 
     sampleRate, wordDits, wpm, weight, ratio, compensation, farnsworth); */
  /* Serial.printf("morse_keyer r %f, w %f, c %f\n", r, w, c); */
  /* samples_per_dit = (samples_per_second * second_per_minute) / (words_per_minute * dits_per_word);  */
  const unsigned ticksPerBaseDit = ((sampleRate * 60) / (wpm * wordDits));
  int ticksPerDit = (1+r+w) * ticksPerBaseDit; /* +c */
  int ticksPerDah = (3-r+w) * ticksPerBaseDit; /* +c */
  int ticksPerIes = (1  -w) * ticksPerBaseDit; /* -c */
  int ticksPerIls = (3  -w) * ticksPerBaseDit; /* -c */
  int ticksPerIws = (7  -w) * ticksPerBaseDit; /* -c */
    
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
  /* test for overflow */
  // ticksPerIws = min(ticksPerIws, 0x7fff);
  /* Serial.printf("morse_keyer base dit %u, dit %u, dah %u, ies %u, ils %u, iws %u\n", 
     ticksPerBaseDit, ticksPerDit, ticksPerDah, ticksPerIes, ticksPerIls, ticksPerIws); */
  /* storing msPerElement, because ticks gets too large */
  AudioNoInterrupts();
  set_vox_nrpn(vox, KYRP_PER_DIT, ticksPerDit/(AUDIO_SAMPLE_RATE/1000));
  set_vox_nrpn(vox, KYRP_PER_DAH, ticksPerDah/(AUDIO_SAMPLE_RATE/1000));
  set_vox_nrpn(vox, KYRP_PER_IES, ticksPerIes/(AUDIO_SAMPLE_RATE/1000));
  set_vox_nrpn(vox, KYRP_PER_ILS, ticksPerIls/(AUDIO_SAMPLE_RATE/1000));
  set_vox_nrpn(vox, KYRP_PER_IWS, ticksPerIws/(AUDIO_SAMPLE_RATE/1000));
  AudioInterrupts();
}

// echo the nprn settings back to the control echo channel
static void nrpn_echo(const int16_t nrpn, const int16_t value) {
  midi_send_nrpn(nrpn, value);
}  

static void nrpn_set_mixer(const int16_t nrpn, const int16_t value, AudioMixer4& mixer) {
  mixer.gain((nrpn-KYRP_MIXER)%4, dbdown(value));
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
  nrpn_set(KYRP_VOLUME, 64);

  /* soft controls */
  nrpn_set(KYRP_BUTTON_0,  6800&KYRV_MASK); /* off */
  nrpn_set(KYRP_BUTTON_1, -2700&KYRV_MASK); /* center */
  nrpn_set(KYRP_BUTTON_2, -1800&KYRV_MASK); /* up */
  nrpn_set(KYRP_BUTTON_3,  -500&KYRV_MASK); /* down */
  nrpn_set(KYRP_BUTTON_4, -2250&KYRV_MASK); /* hey google */
  nrpn_set(KYRP_PTT_ENABLE, 0);
  nrpn_set(KYRP_IQ_ENABLE, 0);
  nrpn_set(KYRP_IQ_ADJUST, 8096);
  nrpn_set(KYRP_TX_ENABLE, 0);
  nrpn_set(KYRP_ST_ENABLE, 1);
  nrpn_set(KYRP_IQ_BALANCE, 8096);
  nrpn_set(KYRP_ST_AUDIO_MODE, 0);
  nrpn_set(KYRP_ST_PAN, 8096);
  nrpn_set(KYRP_DEBOUNCE, 50);
  nrpn_set(KYRP_COMP,0);
  
  nrpn_set(KYRP_CHAN_RECV_CC, KYRC_CHAN_RECV_CC);
  nrpn_set(KYRP_CHAN_SEND_CC, KYRC_CHAN_SEND_CC);
  nrpn_set(KYRP_CHAN_SEND_NOTE_IN, KYRC_CHAN_SEND_NOTE_IN);
  nrpn_set(KYRP_CHAN_SEND_NOTE_OUT, KYRC_CHAN_SEND_NOTE_OUT);
  nrpn_set(KYRP_CHAN_RECV_NOTE_IN, KYRC_CHAN_RECV_NOTE_IN);
  nrpn_set(KYRP_CHAN_RECV_NOTE_OUT, KYRC_CHAN_RECV_NOTE_OUT);

  nrpn_set(KYRP_VERSION, KYRC_VERSION);

  nrpn_set(KYRP_NOTE_L_PAD, KYR_NOTE_L_PAD);
  nrpn_set(KYRP_NOTE_R_PAD, KYR_NOTE_R_PAD);
  nrpn_set(KYRP_NOTE_S_KEY, KYR_NOTE_S_KEY);
  nrpn_set(KYRP_NOTE_EXT_PTT, KYR_NOTE_EXT_PTT);
  nrpn_set(KYRP_NOTE_KEY_OUT, KYR_NOTE_KEY_OUT);
  nrpn_set(KYRP_NOTE_PTT_OUT, KYR_NOTE_PTT_OUT);

  nrpn_set(KYRP_VOLUME_POT, KYR_VOLUME_POT);
  nrpn_set(KYRP_ST_VOL_POT, KYR_ST_VOL_POT);
  nrpn_set(KYRP_ST_FREQ_POT, KYR_ST_FREQ_POT);
  nrpn_set(KYRP_SPEED_POT, KYR_SPEED_POT);

  /* morse code table */
  for (int i = KYRP_MORSE; i < KYRP_MIXER; i += 1) 








    nrpn_set(i, morse[i-KYRP_MORSE]);

  for (int i = KYRP_MIXER; i < KYRP_KEYER; i += 1)
    switch (i) {
    case KYRP_MIX_I2S_L0: case KYRP_MIX_I2S_R0: /* usb_in to i2s_out */
    case KYRP_MIX_I2S_L1: case KYRP_MIX_I2S_R1: /* sidetone to i2s_out */
    case KYRP_MIX_HDW_L0: case KYRP_MIX_HDW_R0: /* usb_in to hdw_out */
    case KYRP_MIX_HDW_L1: case KYRP_MIX_HDW_R1: /* sidetone to i2s_out */
    case KYRP_MIX_USB_L2: case KYRP_MIX_USB_R2:	/* keyed IQ to usb_out */
      nrpn_set(i, 127); continue;
    default:
      nrpn_set(i, 0); continue;
    }

  /* keyer defaults */
  nrpn_set(KYRP_SPEED,18);
  nrpn_set(KYRP_WEIGHT,50);
  nrpn_set(KYRP_RATIO,50);
  nrpn_set(KYRP_FARNS,0);
  nrpn_set(KYRP_TONE,600);
  nrpn_set(KYRP_LEVEL, 64);
  nrpn_set(KYRP_RISE_TIME, 50);	// 5.0 ms
  nrpn_set(KYRP_FALL_TIME, 50);	// 5.0 ms
  nrpn_set(KYRP_RISE_RAMP, KYRV_RAMP_HANN);
  nrpn_set(KYRP_FALL_RAMP, KYRV_RAMP_HANN);
  nrpn_set(KYRP_HEAD_TIME, 0);	// 0 ms
  nrpn_set(KYRP_TAIL_TIME, 0);	// 0 ms
  nrpn_set(KYRP_HANG_TIME, 8);	/* 8 dits */
  nrpn_set(KYRP_PAD_MODE, KYRV_MODE_A);
  nrpn_set(KYRP_PAD_SWAP, 0);
  nrpn_set(KYRP_PAD_ADAPT, KYRV_ADAPT_NORMAL);
  nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_ND7PA);
  nrpn_set(KYRP_AUTO_ILS, 1);
  nrpn_set(KYRP_AUTO_IWS, 0);
  
  /* voice specializations */
  for (int i = KYRP_VOX_TUNE; i < KYRP_LAST; i += 1)
    kyr_nrpn[i] = KYRV_NOT_SET;
}

#include "EEPROM.h"

static void nrpn_write_eeprom(void) {
  EEPROM.put(0, kyr_header); /* header */
  EEPROM.put(sizeof(kyr_header), kyr_nrpn); /* nrpns */
  EEPROM.put(sizeof(kyr_header)+sizeof(kyr_nrpn), kyr_header); /* header */
  EEPROM.put(sizeof(kyr_header)+sizeof(kyr_nrpn)+sizeof(kyr_header), kyr_msgs); /* stored messages */

  /* verify */
  for (int nrpn = 0; nrpn < KYRP_LAST; nrpn += 1) {
    int16_t value;
    EEPROM.get(sizeof(kyr_header)+nrpn*sizeof(int16_t), value);
    if (value != kyr_nrpn[nrpn])
      Serial.printf("nrpn_write_eeprom kyr_nrpn[%d] == %d != %d\n", nrpn, value, kyr_nrpn[nrpn]);
  }
  for (int i = 0; i < sizeof(kyr_msgs); i += 1) {
    int8_t value;
    EEPROM.get(sizeof(kyr_header)+sizeof(kyr_nrpn)+sizeof(kyr_header)+i, value);
    if (value != kyr_msgs[i])
      Serial.printf("nrpn_write_eeprom kyr_msgs[%d] == %d != %d\n", i, value, kyr_msgs[i]);
  }
}

static int nrpn_read_eeprom(void) {
  uint16_t header[3];
  EEPROM.get(0, header);
  if (header[0] != kyr_header[0] || header[1] != kyr_header[1] || header[2] != kyr_header[2]) return 0;
  EEPROM.get(sizeof(header)+sizeof(kyr_nrpn), header);
  if (header[0] != kyr_header[0] || header[1] != kyr_header[1] || header[2] != kyr_header[2]) return 0;
  EEPROM.get(sizeof(header), kyr_nrpn);
  EEPROM.get(sizeof(header)+sizeof(kyr_nrpn)+sizeof(header), kyr_msgs);
  /* apply nrpns */
  for (int nrpn = 0; nrpn < KYRP_LAST; nrpn += 1)
    if (kyr_nrpn[nrpn] != KYRV_NOT_SET)
      nrpn_set(nrpn, kyr_nrpn[nrpn]);
  return 1;
}

/*
** set a NRPN value.
** all are copied into the kyr_nrpn[] array.
** some are transformed into sample unit timings.
** some trigger calls to the sgtl5000 control interface.
** none are sent into the audio library processing loop,
** it pulls parameters as necessary.
*/
static void nrpn_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VOLUME:
    // nrpn_report("KYRP_VOLUME", kyr_nrpn[nrpn], value);
    codec_nrpn_set(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_INPUT_SELECT:
  case KYRP_MUTE_HEAD_PHONES:
  case KYRP_MUTE_LINE_OUT:
  case KYRP_LINE_OUT_LEVEL_L:
  case KYRP_LINE_OUT_LEVEL_R:
  case KYRP_LINE_IN_LEVEL_L:
  case KYRP_LINE_IN_LEVEL_R:
  case KYRP_MIC_PREAMP_GAIN:
    codec_nrpn_set(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_LINE_OUT_LEVEL:
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_L] = kyr_nrpn[KYRP_LINE_OUT_LEVEL_R] = value;
    codec_nrpn_set(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_LINE_IN_LEVEL:
    kyr_nrpn[KYRP_LINE_IN_LEVEL_L] = kyr_nrpn[KYRP_LINE_IN_LEVEL_R] = value;
    codec_nrpn_set(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
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
  case KYRP_ST_AUDIO_MODE:
  case KYRP_ST_PAN:
  case KYRP_DEBOUNCE:
  case KYRP_CHAN_SEND_CC:
  case KYRP_CHAN_RECV_CC:
  case KYRP_CHAN_SEND_NOTE_IN:
  case KYRP_CHAN_SEND_NOTE_OUT:
  case KYRP_CHAN_RECV_NOTE_IN:
  case KYRP_CHAN_RECV_NOTE_OUT:
  case KYRP_NOTE_L_PAD:
  case KYRP_NOTE_R_PAD:
  case KYRP_NOTE_S_KEY:
  case KYRP_NOTE_EXT_PTT:
  case KYRP_NOTE_KEY_OUT:
  case KYRP_NOTE_PTT_OUT:
    kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_VOLUME_POT:
  case KYRP_ST_VOL_POT:
  case KYRP_ST_FREQ_POT:
  case KYRP_SPEED_POT:
    input_nrpn_set(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
    
    // case KYRP_MORSE+(0..63): see default case
    
  case KYRP_MIX_USB_L0:
  case KYRP_MIX_USB_L1:
  case KYRP_MIX_USB_L2:
  case KYRP_MIX_USB_L3: nrpn_set_mixer(nrpn, value, l_usb_out); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_MIX_USB_R0:
  case KYRP_MIX_USB_R1:
  case KYRP_MIX_USB_R2:
  case KYRP_MIX_USB_R3: nrpn_set_mixer(nrpn, value, r_usb_out); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_MIX_I2S_L0:
  case KYRP_MIX_I2S_L1:
  case KYRP_MIX_I2S_L2:
  case KYRP_MIX_I2S_L3: nrpn_set_mixer(nrpn, value, l_i2s_out); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_MIX_I2S_R0:
  case KYRP_MIX_I2S_R1:
  case KYRP_MIX_I2S_R2:
  case KYRP_MIX_I2S_R3: nrpn_set_mixer(nrpn, value, r_i2s_out); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_MIX_HDW_L0:
  case KYRP_MIX_HDW_L1:
  case KYRP_MIX_HDW_L2:
  case KYRP_MIX_HDW_L3: nrpn_set_mixer(nrpn, value, l_hdw_out); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
  case KYRP_MIX_HDW_R0:
  case KYRP_MIX_HDW_R1:
  case KYRP_MIX_HDW_R2:
  case KYRP_MIX_HDW_R3: nrpn_set_mixer(nrpn, value, r_hdw_out); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;

    // keyer voice cases
#define keyer_case(VOX, VOXP) \
  case VOXP+KYRP_TONE:	    case VOXP+KYRP_LEVEL:     case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_HANG_TIME: \
  case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME: case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: \
  case VOXP+KYRP_PAD_MODE:  case VOXP+KYRP_PAD_SWAP:  case VOXP+KYRP_PAD_ADAPT: case VOXP+KYRP_AUTO_ILS:  case VOXP+KYRP_AUTO_IWS: \
  case VOXP+KYRP_PAD_KEYER: \
    kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break; \
  case VOXP+KYRP_SPEED:     case VOXP+KYRP_WEIGHT:    case VOXP+KYRP_RATIO:     case VOXP+KYRP_FARNS: \
    kyr_nrpn[nrpn] = value; nrpn_update_keyer_timing(VOX); nrpn_echo(nrpn, value); break;
    
    keyer_case(KYR_VOX_NONE, KYRP_VOX_NONE-KYRP_KEYER); // default keyer params
    keyer_case(KYR_VOX_TUNE, KYRP_VOX_TUNE-KYRP_KEYER); // tune params
    keyer_case(KYR_VOX_S_KEY, KYRP_VOX_S_KEY-KYRP_KEYER); // straight key keyer params
    keyer_case(KYR_VOX_PAD, KYRP_VOX_PAD-KYRP_KEYER);     // paddle keyer params
    keyer_case(KYR_VOX_WINK, KYRP_VOX_WINK-KYRP_KEYER); // winkey text keyer params
    keyer_case(KYR_VOX_KYR, KYRP_VOX_KYR-KYRP_KEYER);   // kyr text keyer params
    keyer_case(KYR_VOX_KYR, KYRP_VOX_BUT-KYRP_KEYER);   // button key params

#undef keyer_case    
    
    /* commands, keep no state in kyr_nrpn */
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
    for (int i = 0; i < KYRP_LAST; i += 1) 
      if (kyr_nrpn[i] != KYRV_NOT_SET) 
	nrpn_echo(i, kyr_nrpn[i]);
    nrpn_echo(nrpn, 0);
    break;
  case KYRP_SEND_WINK: 
    wink.send_text(value&127); 
    nrpn_echo(nrpn, value&127);
    break;
  case KYRP_SEND_KYR: 
    kyr.send_text(value&127);
    nrpn_echo(nrpn, value&127);
    break;
  case KYRP_MSG_INDEX: 
    kyr_index = min(sizeof(kyr_msgs)-1, max(0, value));
    nrpn_echo(nrpn, kyr_index);
    break;
  case KYRP_MSG_WRITE:
    nrpn_echo(nrpn, value&127);
    kyr_msgs[kyr_index++] = value&127;
    kyr_index = min(sizeof(kyr_msgs)-1, max(0, value));
    break;
  case KYRP_MSG_READ: 
    nrpn_echo(nrpn, kyr_msgs[kyr_index++]);
    kyr_index = min(sizeof(kyr_msgs)-1, max(0, value));
    break;
  case KYRP_PLAY_WINK:
    nrpn_echo(nrpn, value);
    break;
  case KYRP_PLAY_KYR:
    nrpn_echo(nrpn, value);
    break;

    /* information only, cause no side effects */
  case KYRP_VERSION: nrpn_echo(nrpn, KYRC_VERSION); break;
  case KYRP_NRPN_SIZE: nrpn_echo(nrpn, sizeof(kyr_nrpn)); break;
  case KYRP_MSG_SIZE: nrpn_echo(nrpn, sizeof(kyr_msgs)); break;

  default: 
    if ((nrpn >= KYRP_MORSE && nrpn < KYRP_MORSE+64)) {
      kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value);
      break;
    }
    Serial.printf("uncaught nrpn #%d with value %d\n", nrpn, value);
    break;
  }
}

static void nrpn_setup(void) {

  codec_enable();

  /* mute headphones */
  nrpn_set(KYRP_MUTE_HEAD_PHONES, 1);
  nrpn_set(KYRP_VOLUME, 0);

  /* codec setup */
  nrpn_set(KYRP_INPUT_SELECT, 0);
  nrpn_set(KYRP_MIC_PREAMP_GAIN, 0); // suggestion in audio docs
  // kyr_nrpn[KYRP_MIC_BIAS] = 7;	     // taken from audio library
  // nrpn_set(KYRP_MIC_IMPEDANCE, 1);   // taken from audio library
  nrpn_set(KYRP_MUTE_LINE_OUT, 1);
  nrpn_set(KYRP_LINE_IN_LEVEL, 5);
  nrpn_set(KYRP_LINE_OUT_LEVEL, 29);

  /* unmute headphones */
  nrpn_set(KYRP_MUTE_HEAD_PHONES, 0);

#if 0
  for (int i = 0; i < KYRP_LAST; i += 1) kyr_nrpn[i] = KYRV_NOT_SET;
#endif
  nrpn_set_defaults();
#if 0
  for (int i = 0; i < KYRP_LAST; i += 1)
    if (kyr_nrpn[i] == KYRV_NOT_SET) {
      if (i >= KYRP_VOX_TUNE) continue;
      Serial.printf("nrpn #%d is not initialized\n", i);
    }
#endif
}
