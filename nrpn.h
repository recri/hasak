/***************************************************************
** NRPN (non-registered parameter) handling.
***************************************************************/
/*
** This is where the NRPN's get stored.
*/
int16_t kyr_nrpn[KYRP_LAST];

/* set a nrpn without */
static inline void set_nrpn(int nrpn, int value) {
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
static inline void set_vox_nrpn(int vox, int nrpn, int value) {
  if (vox >= 0 && vox <= KYR_N_VOX && nrpn >= KYRP_KEYER && nrpn < KYRP_VOX_1)
    set_nrpn(nrpn+vox*(KYRP_VOX_1-KYRP_VOX_0), value);
}

//static inline void set_vox_dit(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_DIT, value); }
//static inline void set_vox_dah(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_DAH, value); }
//static inline void set_vox_ies(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_IES, value); }
//static inline void set_vox_ils(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_ILS, value); }
//static inline void set_vox_iws(int vox, int value) {  set_vox_nrpn(vox, KYRP_PER_IWS, value); }

/*
** Update the keyer timing, specified by speed, weight, ratio, comp, and farnsworth,
** and produce the samples per dit, dah, ies, ils, and iws.
*/
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
  ticksPerIws = min(ticksPerIws, 0x7fff);
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

  

/*
** set a NRPN value.
** all are copied into the kyr_nrpn[] array.
** some are transformed into sample unit timings.
** some trigger calls to the sgtl5000 control interface.
** none are sent into the audio library processing loop,
** it pulls parameters as necessary.
*/
static void nrpn_set(uint16_t nrpn, uint16_t value) {
  switch (nrpn) {
  case KYRP_HEAD_PHONE_VOLUME:
    // inspecting the sgtl5000 source finds only 7-8bits of precision for this.
    sgtl5000.volume(value/127.0); kyr_nrpn[nrpn] = value;  break; // fix.me - automate
  case KYRP_INPUT_SELECT:
    sgtl5000.inputSelect(value); kyr_nrpn[nrpn] = value; break;
  case KYRP_MUTE_HEAD_PHONES:
    if (value) sgtl5000.muteHeadphone(); else sgtl5000.unmuteHeadphone();    
    kyr_nrpn[nrpn] = value;  break;
  case KYRP_MUTE_LINE_OUT:
    if (value) sgtl5000.muteLineout(); else sgtl5000.unmuteLineout();
    kyr_nrpn[nrpn] = value; break;
  case KYRP_LINE_OUT_LEVEL:
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_L] = kyr_nrpn[KYRP_LINE_OUT_LEVEL_R] = value;
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_OUT_LEVEL_L:
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_L] = value;
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_OUT_LEVEL_R:
    kyr_nrpn[KYRP_LINE_OUT_LEVEL_R] = value;
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL:
    kyr_nrpn[KYRP_LINE_IN_LEVEL_L] = kyr_nrpn[KYRP_LINE_IN_LEVEL_R] = value;
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL_L:
    kyr_nrpn[KYRP_LINE_IN_LEVEL_L] = value;
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL_R:
    kyr_nrpn[KYRP_LINE_IN_LEVEL_R] = value;
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_MIC_PREAMP_GAIN:
    sgtl5000.micPreampGain(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_MIC_BIAS:
    sgtl5000.micBias(value);
    kyr_nrpn[nrpn] = value; break;
  case KYRP_MIC_IMPEDANCE:
    sgtl5000.micImpedance(value);
    kyr_nrpn[nrpn] = value; break;

  case KYRP_BUTTON_0:
  case KYRP_BUTTON_1:
  case KYRP_BUTTON_2:
  case KYRP_BUTTON_3:
  case KYRP_BUTTON_4:
  case KYRP_SEND_MIDI:
  case KYRP_RECV_MIDI:
  case KYRP_IQ_ENABLE:
  case KYRP_IQ_ADJUST:
  case KYRP_TX_ENABLE:
  case KYRP_ST_ENABLE:
  case KYRP_IQ_BALANCE:
  case KYRP_ST_AUDIO_MODE:
  case KYRP_ST_PAN:
    kyr_nrpn[nrpn] = value;  break;
    
    // case KYRP_MORSE+(0..63):
    
    // case KYRP_MIXER+(0..23):

    // default keyer params
#define VOX KYR_VOX_NONE
#define VOXP KYRP_VOX_0-KYRP_VOX_0
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME:
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT:
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS:
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME:
    kyr_nrpn[nrpn] = value; break;
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS:
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); break;
#undef VOX
#undef VOXP

    // straight key keyer params
#define VOX KYR_VOX_S_KEY
#define VOXP KYRP_VOX_1-KYRP_VOX_0
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME:
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT:
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS:
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME:
    kyr_nrpn[nrpn] = value; break;
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS:
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); break;
#undef VOX
#undef VOXP

    // paddle keyer params
#define VOX KYR_VOX_PAD
#define VOXP KYRP_VOX_2-KYRP_VOX_0
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME:
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT:
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS:
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME:
    kyr_nrpn[nrpn] = value; break;
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS:
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); break;
#undef VOX
#undef VOXP

    // winkey text keyer params
#define VOX KYR_VOX_WINK
#define VOXP KYRP_VOX_3-KYRP_VOX_0
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME:
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT:
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS:
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME:
    kyr_nrpn[nrpn] = value; break;
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS:
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); break;
#undef VOX
#undef VOXP

    // kyr text keyer params
#define VOX KYR_VOX_KYR
#define VOXP KYRP_VOX_4-KYRP_VOX_0
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME:
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT:
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS:
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME:
    kyr_nrpn[nrpn] = value; break;
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS:
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); break;
#undef VOX
#undef VOXP

    // but straight key params
#define VOX KYR_VOX_KYR
#define VOXP KYRP_VOX_5-KYRP_VOX_0
  case VOXP+KYRP_TONE: case VOXP+KYRP_HEAD_TIME: case VOXP+KYRP_TAIL_TIME: case VOXP+KYRP_RISE_TIME: case VOXP+KYRP_FALL_TIME:
  case VOXP+KYRP_RISE_RAMP: case VOXP+KYRP_FALL_RAMP: case VOXP+KYRP_PAD_MODE: case VOXP+KYRP_PAD_SWAP: case VOXP+KYRP_PAD_ADAPT:
  case VOXP+KYRP_PER_DIT: case VOXP+KYRP_PER_DAH: case VOXP+KYRP_PER_IES: case VOXP+KYRP_PER_ILS: case VOXP+KYRP_PER_IWS:
  case VOXP+KYRP_AUTO_ILS: case VOXP+KYRP_AUTO_IWS: case VOXP+KYRP_PAD_KEYER: case VOXP+KYRP_HANG_TIME:
    kyr_nrpn[nrpn] = value; break;
  case VOXP+KYRP_SPEED: case VOXP+KYRP_WEIGHT: case VOXP+KYRP_RATIO: case VOXP+KYRP_COMP: case VOXP+KYRP_FARNS:
    kyr_nrpn[nrpn] = value; nrpn_update_keyer(VOX); break;
#undef VOX
#undef VOXP

  default: 
    if ((nrpn >= KYRP_MORSE && nrpn < KYRP_MORSE+64) ||
	(nrpn >= KYRP_MIXER && nrpn < KYRP_MIXER+24)) {
      kyr_nrpn[nrpn] = value; break;
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

static void nrpn_setup(void) {

  /* enable codec */
  /* this causes a loud pop with cap coupled head phone outputs */
  sgtl5000.enable();

  /* mute headphones */
  nrpn_set(KYRP_MUTE_HEAD_PHONES, 1);
  nrpn_set(KYRP_HEAD_PHONE_VOLUME, 0);

  /* codec setup */
  nrpn_set(KYRP_INPUT_SELECT, 0);
  nrpn_set(KYRP_MIC_PREAMP_GAIN, 0); // suggestion in audio docs
  kyr_nrpn[KYRP_MIC_BIAS] = 7;	     // taken from audio library
  nrpn_set(KYRP_MIC_IMPEDANCE, 1);   // taken from audio library
  nrpn_set(KYRP_MUTE_LINE_OUT, 1);
  nrpn_set(KYRP_LINE_IN_LEVEL, 5);
  nrpn_set(KYRP_LINE_OUT_LEVEL, 29);

  /* unmute headphones */
  nrpn_set(KYRP_MUTE_HEAD_PHONES, 0);
  nrpn_set(KYRP_HEAD_PHONE_VOLUME, 40);

  /* soft controls */
  nrpn_set(KYRP_BUTTON_0, 6800);  /* off */
  nrpn_set(KYRP_BUTTON_1, -2700); /* center */
  nrpn_set(KYRP_BUTTON_2, -1800); /* up */
  nrpn_set(KYRP_BUTTON_3, -500);  /* down */
  nrpn_set(KYRP_BUTTON_4, -2250); /* hey google */
  nrpn_set(KYRP_SEND_MIDI, 1);
  nrpn_set(KYRP_RECV_MIDI, 0);
  nrpn_set(KYRP_IQ_ENABLE, 0);
  nrpn_set(KYRP_IQ_ADJUST, 8096);
  nrpn_set(KYRP_TX_ENABLE, 0);
  nrpn_set(KYRP_ST_ENABLE, 1);
  nrpn_set( KYRP_IQ_BALANCE, 8096);
  nrpn_set(KYRP_ST_AUDIO_MODE, 0);
  nrpn_set(KYRP_ST_PAN, 8096);

  /* morse code table */
  for (int i = 0; i < 64; i += 1) kyr_nrpn[KYRP_MORSE+i] = morse[i];

  /* mixer matrix */
  
  /* keyer defaults */
  nrpn_set(KYRP_SPEED,18);
  nrpn_set(KYRP_WEIGHT,50);
  nrpn_set(KYRP_RATIO,50);
  nrpn_set(KYRP_COMP,0);
  nrpn_set(KYRP_FARNS,0);
  nrpn_set(KYRP_TONE,600);
  nrpn_set(KYRP_RISE_TIME, 50);	// 5.0 ms
  nrpn_set(KYRP_FALL_TIME, 50);	// 5.0 ms
  nrpn_set(KYRP_RISE_RAMP, KYRP_RAMP_HANN);
  nrpn_set(KYRP_FALL_RAMP, KYRP_RAMP_HANN);
  nrpn_set(KYRP_TAIL_TIME, 0);	// 0 ms
  nrpn_set(KYRP_HEAD_TIME, 0);	// 0 ms
  nrpn_set(KYRP_HANG_TIME, 8);	/* 8 dits */
  nrpn_set(KYRP_PAD_MODE, KYRP_MODE_A);
  nrpn_set(KYRP_PAD_SWAP, 0);
  nrpn_set(KYRP_PAD_ADAPT, KYRP_ADAPT_NORMAL);
  nrpn_set(KYRP_PAD_KEYER, KYRP_KEYER_ND7PA);
  nrpn_set(KYRP_HANG_TIME, 7);
  
  /* voice specializations */
  for (int i = KYRP_VOX_1; i < KYRP_LAST; i += 1) kyr_nrpn[i] = KYRP_NOT_SET;
}
