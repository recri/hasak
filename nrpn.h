/***************************************************************
** NRPN (non-registered parameter) handling.
***************************************************************/
/*
** This is where the NRPN's get stored.
*/
int16_t kyr_nrpn[KYRP_LAST];

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
  if (vox >= 0 && vox <= KYR_N_VOX && nrpn >= KYRP_KEYER && nrpn < KYRP_VOX_1)
    set_nrpn(nrpn+vox*(KYRP_VOX_1-KYRP_VOX_0), value);
}

/*
** Update the keyer timing, specified by speed, weight, ratio, comp, and farnsworth,
** and produce the samples per dit, dah, ies, ils, and iws.
*/
static void nrpn_update_keyer(const int16_t vox) {
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
  int ticksPerDit = (1+r+w+c) * ticksPerBaseDit;
  int ticksPerDah = (3-r+w+c) * ticksPerBaseDit;
  int ticksPerIes = (1  -w-c) * ticksPerBaseDit;
  int ticksPerIls = (3  -w-c) * ticksPerBaseDit;
  int ticksPerIws = (7  -w-c) * ticksPerBaseDit;
    
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
** set a NRPN value.
** all are copied into the kyr_nrpn[] array.
** some are transformed into sample unit timings.
** some trigger calls to the sgtl5000 control interface.
** none are sent into the audio library processing loop,
** it pulls parameters as necessary.
*/
static void nrpn_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VERSION:
    kyr_nrpn[nrpn] = KYRC_VERSION; nrpn_echo(nrpn, value); break;
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
  case KYRP_CC_CHAN_SEND:
  case KYRP_CC_CHAN_RECV:
  case KYRP_NOTE_IN_CHAN_SEND:
  case KYRP_NOTE_OUT_CHAN_SEND:
  case KYRP_NOTE_IN_CHAN_RECV:
  case KYRP_NOTE_OUT_CHAN_RECV:
  case KYRP_L_PAD_NOTE:
  case KYRP_R_PAD_NOTE:
  case KYRP_S_KEY_NOTE:
  case KYRP_EXT_PTT_NOTE:
  case KYRP_KEY_OUT_NOTE:
  case KYRP_PTT_OUT_NOTE:
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

    // keyer case table
#define keyer_case(VOX, VOXP) \
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME: \
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT: \
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS: \
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME: case VOXP+KYRP_LEVEL: \
    kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break; \
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS: \
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); nrpn_echo(nrpn, value); break;
    
    keyer_case(KYR_VOX_NONE, KYRP_VOX_0-KYRP_VOX_0);    // default keyer params
    keyer_case(KYR_VOX_TUNE, KYRP_VOX_1-KYRP_VOX_0);    // tune params
    keyer_case(KYR_VOX_S_KEY, KYRP_VOX_2-KYRP_VOX_0);    // straight key keyer params
    keyer_case(KYR_VOX_PAD, KYRP_VOX_3-KYRP_VOX_0);    // paddle keyer params
    keyer_case(KYR_VOX_WINK, KYRP_VOX_4-KYRP_VOX_0);    // winkey text keyer params
    keyer_case(KYR_VOX_KYR, KYRP_VOX_5-KYRP_VOX_0);    // kyr text keyer params
    keyer_case(KYR_VOX_KYR, KYRP_VOX_6-KYRP_VOX_0);    // button key params
    
  default: 
    if ((nrpn >= KYRP_MORSE && nrpn < KYRP_MORSE+64)) {
      kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;
    }
    Serial.printf("uncaught nrpn #%d with value %d\n", nrpn, value); break;
  }
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
  nrpn_set(KYRP_BUTTON_0,  6800); /* off */
  nrpn_set(KYRP_BUTTON_1, -2700); /* center */
  nrpn_set(KYRP_BUTTON_2, -1800); /* up */
  nrpn_set(KYRP_BUTTON_3,  -500); /* down */
  nrpn_set(KYRP_BUTTON_4, -2250); /* hey google */
  nrpn_set(KYRP_PTT_ENABLE, 0);
  nrpn_set(KYRP_IQ_ENABLE, 0);
  nrpn_set(KYRP_IQ_ADJUST, 8096);
  nrpn_set(KYRP_TX_ENABLE, 0);
  nrpn_set(KYRP_ST_ENABLE, 1);
  nrpn_set(KYRP_IQ_BALANCE, 8096);
  nrpn_set(KYRP_ST_AUDIO_MODE, 0);
  nrpn_set(KYRP_ST_PAN, 8096);
  nrpn_set(KYRP_DEBOUNCE, 50);
  
  nrpn_set(KYRP_CC_CHAN_RECV, KYRC_CC_CHAN_RECV);
  nrpn_set(KYRP_CC_CHAN_SEND, KYRC_CC_CHAN_SEND);
  nrpn_set(KYRP_NOTE_IN_CHAN_SEND, KYRC_NOTE_IN_CHAN_SEND);
  nrpn_set(KYRP_NOTE_OUT_CHAN_SEND, KYRC_NOTE_OUT_CHAN_SEND);
  nrpn_set(KYRP_NOTE_IN_CHAN_RECV, KYRC_NOTE_IN_CHAN_RECV);
  nrpn_set(KYRP_NOTE_OUT_CHAN_RECV, KYRC_NOTE_OUT_CHAN_RECV);

  nrpn_set(KYRP_VERSION, KYRC_VERSION);

  nrpn_set(KYRP_L_PAD_NOTE, KYR_L_PAD_NOTE);
  nrpn_set(KYRP_R_PAD_NOTE, KYR_R_PAD_NOTE);
  nrpn_set(KYRP_S_KEY_NOTE, KYR_S_KEY_NOTE);
  nrpn_set(KYRP_EXT_PTT_NOTE, KYR_EXT_PTT_NOTE);
  nrpn_set(KYRP_KEY_OUT_NOTE, KYR_KEY_OUT_NOTE);
  nrpn_set(KYRP_PTT_OUT_NOTE, KYR_PTT_OUT_NOTE);

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
  nrpn_set(KYRP_COMP,0);
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
  for (int i = KYRP_VOX_1; i < KYRP_LAST; i += 1) kyr_nrpn[i] = KYRV_NOT_SET;
}

#include "EEPROM.h"

static void nrpn_write_eeprom(void) {
}

static int nrpn_read_eeprom(void) {
  return 0;
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
      if (i >= KYRP_VOX_1) continue;
      Serial.printf("nrpn #%d is not initialized\n", i);
    }
#endif
}
