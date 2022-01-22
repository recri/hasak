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
** The idea of set_vox_nrpn(vox, val) and get_vox_nrpn(vox) is that
** keyer voices may have customized parameters, but
** fall back to the common parameter set when no
** customized parameter has been set.
** So set_vox_nrpn() sets the bank of the kyr_nrpn that
** is vox specific, and get_vox_nrpn(vox, NRPN) looks in the vox
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
  const float wpm = get_vox_nrpn(vox, KYRP_SPEED);
  const float weight = get_vox_nrpn(vox, KYRP_WEIGHT);
  const float ratio = get_vox_nrpn(vox, KYRP_RATIO);
  const float compensation = tenth_ms_to_ms(signed_value(get_vox_nrpn(vox, KYRP_COMP)));
  const float farnsworth = get_vox_nrpn(vox, KYRP_FARNS);;
  const float ms_per_dit = (1000 * 60) / (wpm * wordDits);
  const float r = (ratio-50)/100.0;
  const float w = (weight-50)/100.0;
  const float c = compensation / ms_per_dit; /* ms/10 / ms_per_dit */ 
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
  set_vox_nrpn(vox, KYRP_PER_DIT, samples_to_ms(ticksPerDit));
  set_vox_nrpn(vox, KYRP_PER_DAH, samples_to_ms(ticksPerDah));
  set_vox_nrpn(vox, KYRP_PER_IES, samples_to_ms(ticksPerIes));
  set_vox_nrpn(vox, KYRP_PER_ILS, samples_to_ms(ticksPerIls));
  set_vox_nrpn(vox, KYRP_PER_IWS, samples_to_ms(ticksPerIws));
  AudioInterrupts();
}

// echo the nprn settings back to the control echo channel
static void nrpn_echo(const int16_t nrpn, const int16_t value) {
  midi_send_nrpn(nrpn, value);
}  

static void nrpn_update_mixer(const int16_t nrpn, const int16_t value) {
  if (nrpn == KYRP_OUT_ENABLE) {
    // Serial.printf("update_mixer(%d, 0x%03x)\n", nrpn, value);
    AudioNoInterrupts();
    kyr_nrpn[nrpn] = value;
    for (int i = KYRP_MIX_USB_L0; i <= KYRP_MIX_HDW_R3; i += 1)
      nrpn_update_mixer(i, get_nrpn(i));
    AudioInterrupts();
  } else if (nrpn >= KYRP_MIX_USB_L0 && nrpn <= KYRP_MIX_HDW_R3) {
    uint16_t index = (nrpn-KYRP_MIXER)/4; /* index of mixer in mix_out */
    uint16_t chan = (nrpn-KYRP_MIXER)%4;  /* channel of the mixer */
    uint16_t mask = 1<<(11-(((index>>1)<<2)+chan)); /* bit enabling channel */
    float gain = ((mask&get_nrpn(KYRP_OUT_ENABLE)) ? 1 : 0) * int_to_127ths(value);
    //Serial.printf("update_mixer(%d, %d) index %d chan %d mask %03x gain %f\n", 
    //		  nrpn, value, index, chan, mask, gain);
   mix_out[index]->gain(chan, gain);
  } else {
    Serial.printf("unexpected nrpn %d in nprn_update_mixer\n", nrpn);
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
  nrpn_set(KYRP_VERSION, KYRC_VERSION);

  nrpn_set(KYRP_VOLUME, 64);
  nrpn_set(KYRP_INPUT_SELECT, 0);
  nrpn_set(KYRP_INPUT_LEVEL, 64);

  /* soft controls */
  nrpn_set(KYRP_BUTTON_0,  6800&KYRV_MASK); /* off */
  nrpn_set(KYRP_BUTTON_1, -2700&KYRV_MASK); /* center */
  nrpn_set(KYRP_BUTTON_2, -1800&KYRV_MASK); /* up */
  nrpn_set(KYRP_BUTTON_3,  -500&KYRV_MASK); /* down */
  nrpn_set(KYRP_BUTTON_4, -2250&KYRV_MASK); /* hey google */
  nrpn_set(KYRP_PTT_ENABLE, 0);
  nrpn_set(KYRP_IQ_ENABLE, 0);
  nrpn_set(KYRP_IQ_ADJUST, 0);
  nrpn_set(KYRP_TX_ENABLE, 0);
  nrpn_set(KYRP_ST_ENABLE, 1);
  nrpn_set(KYRP_IQ_BALANCE, 0);
  nrpn_set(KYRP_ST_PAN, 0);
  nrpn_set(KYRP_DEBOUNCE, 50);
  nrpn_set(KYRP_COMP,0);
  
  nrpn_set(KYRP_CHAN_RECV_CC, KYRC_CHAN_RECV_CC);
  nrpn_set(KYRP_CHAN_SEND_CC, KYRC_CHAN_SEND_CC);
  nrpn_set(KYRP_CHAN_SEND_NOTE_IN, KYRC_CHAN_SEND_NOTE_IN);
  nrpn_set(KYRP_CHAN_SEND_NOTE_OUT, KYRC_CHAN_SEND_NOTE_OUT);
  nrpn_set(KYRP_CHAN_RECV_NOTE_IN, KYRC_CHAN_RECV_NOTE_IN);
  nrpn_set(KYRP_CHAN_RECV_NOTE_OUT, KYRC_CHAN_RECV_NOTE_OUT);


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

  /* output mixers */
  for (int i = KYRP_MIXER; i < KYRP_KEYER; i += 1) nrpn_set(i, 127);
  /* output mixers enable */
  nrpn_set(KYRP_OUT_ENABLE, 0b001011001100); /* IQ to usb, usb+sidetone to i2s and hdw */

  /* keyer defaults - common */
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
  nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_VK6PH);
  nrpn_set(KYRP_AUTO_ILS, 1);
  nrpn_set(KYRP_AUTO_IWS, 0);

  /* keyer defaults - per voice */
  nrpn_set(KYRP_TONE, 600);
  nrpn_set(KYRP_LEVEL, 64);
  nrpn_set(KYRP_SPEED,18);
  nrpn_set(KYRP_WEIGHT,50);
  nrpn_set(KYRP_RATIO,50);
  nrpn_set(KYRP_FARNS,0);
  
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
  for (unsigned i = 0; i < sizeof(kyr_msgs); i += 1) {
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
  case KYRP_INPUT_SELECT:
  case KYRP_INPUT_LEVEL:
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
  case KYRP_ST_PAN:

  case KYRP_DEBOUNCE:
  case KYRP_COMP:
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
    
  case KYRP_OUT_ENABLE:
    nrpn_update_mixer(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value); break;

    // case KYRP_MORSE+(0..63): see default case
    
    // case KYRP_MIXER+(0..23): see default case

    // keyer voice cases
#define keyer_case(VOX, VOXP) \
  case VOXP+KYRP_TONE:	    case VOXP+KYRP_LEVEL: \
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
  case KYRP_SAMPLE_RATE: nrpn_echo(nrpn, ((uint16_t)AUDIO_SAMPLE_RATE)/100); break;
				   
  default: 
    if (nrpn >= KYRP_MORSE && nrpn < KYRP_MORSE+64) {
      kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value);
      break;
    }
    if (nrpn >= KYRP_MIX_USB_L0 && nrpn <= KYRP_MIX_HDW_R3) {
      nrpn_update_mixer(nrpn, value); kyr_nrpn[nrpn] = value; nrpn_echo(nrpn, value);
      break;
    }
    Serial.printf("uncaught nrpn #%d with value %d\n", nrpn, value);
    break;
  }
}

static void nrpn_setup(void) {
  codec_enable();
  nrpn_set(KYRP_VOLUME, 64);
  nrpn_set_defaults();
}
