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
/*
** diagnostics / command line interface
*/
/* usage report line */
static void diag_report(const char *tag, uint32_t use, uint32_t umax) {
  Serial.printf("%16s %5.3f%% %5.3f%%", tag, timing_percent(use), timing_percent_max(umax));
  // Serial.printf("\n");
}

elapsedMicros diag_totalTime;

/* summary report */
static void diag_sreport(void) {
  static elapsedLoops eLoops;
  static elapsedSamples eSamples;
  static elapsedUpdates eUpdates;
  unsigned loops = eLoops; eLoops = 0;
  unsigned samples = eSamples; eSamples = 0;
  unsigned updates = eUpdates; eUpdates = 0;

  Serial.printf("Short summary:\n");
  Serial.printf("\ttime %f(s), sample rate %f(sps) buffer size %d(bytes) F_CPU %.1f(MHz)\n", 
		diag_totalTime/1e6, AUDIO_SAMPLE_RATE, AUDIO_BLOCK_SAMPLES, F_CPU/1e6f);
  Serial.printf("\tdit/dah/ies/ils/iws %ld/%ld/%ld/%ld/%ld samples\n",
		xnrpn_get(KYRP_XPER_DIT), xnrpn_get(KYRP_XPER_DAH),
		xnrpn_get(KYRP_XPER_IES), xnrpn_get(KYRP_XPER_ILS), xnrpn_get(KYRP_XPER_IWS));
  // Serial.printf("\tnote %5d %5d nrpn %5d %5d\n", kyr_recv_note, kyr_send_note, kyr_recv_nrpn, kyr_send_nrpn);
  diag_report("total", AudioStream::cpu_cycles_total, AudioStream::cpu_cycles_total_max);
  diag_report("isr", timing_isrCyclesPerAudioBuffer, timing_isrCyclesPerAudioBufferMax);
  Serial.printf("%16s %2d %2d\n", "buffers", AudioMemoryUsage(), AudioMemoryUsageMax());
  Serial.printf("\tupdates %ld samples %ld, loops %ld, sample/update %.2f loop/sample %.2f\n",
		updates, samples, loops, (float)samples/updates, (float)loops/samples);
}

static void diag_treport(const char *p) {
  Serial.printf("\t%f %s\n", diag_totalTime/1e6, p);
}

/* generic audio module report and reset */
static void diag_mreport(const char *tag, AudioStream& str) {
  diag_report(tag, str.cpu_cycles, str.cpu_cycles_max);
}

/* long report */
static void diag_Sreport(void) {
  // report("total", AudioStream::cpu_cycles_total, AudioStream::cpu_cycles_total_max, AudioMemoryUsage(), AudioMemoryUsageMax(), 0, 0);
  // report("denom", cpuCyclesPerAudioBuffer, cpuCyclesPerAudioBufferMax, 0, 0, 0, 0);
  diag_sreport();
  Serial.printf("Extended summary:\n");
  diag_mreport("i2s_in", i2s_in);       diag_mreport("usb_in", usb_in); 
#if KYR_ENABLE_HDW_IN > 0
  diag_mreport("hdw_in", hdw_in);
#endif
   Serial.println();

  diag_mreport("i2s_in", i2s_in);       diag_mreport("usb_in", usb_in);       Serial.println();
  diag_mreport("i2s_out", i2s_out);     diag_mreport("usb_out", usb_out);
#if KYR_ENABLE_HDW_OUT > 0
#if 0
  diag_mreport("hdw_out", hdw_out); 
#endif
#endif
  Serial.println();

  diag_mreport("tone_ramp", tone_ramp); diag_mreport("key_ramp", key_ramp);   Serial.println();
  diag_mreport("l_i2s_out", l_i2s_out); diag_mreport("r_i2s_out", r_i2s_out); Serial.println();
  diag_mreport("l_usb_out", l_usb_out); diag_mreport("r_usb_out", r_usb_out); Serial.println();
  diag_mreport("l_hdw_out", l_hdw_out); diag_mreport("r_hdw_out", r_hdw_out); Serial.println();
}

/* summary reset */
static void diag_mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}

static void diag_sreset(void) {
  timing_cpuCyclesPerAudioBufferMax = timing_cpuCyclesPerAudioBuffer;
  timing_isrCyclesPerAudioBufferMax = timing_isrCyclesPerAudioBuffer;
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

static void diag_Sreset(void) {
  diag_sreset();
  diag_mreset(i2s_in);
  diag_mreset(usb_in);
#if defined(KYR_ENABLE_HDW_IN)
  diag_mreset(hdw_in);
#endif
  diag_mreset(tone_ramp);
  diag_mreset(key_ramp);
  diag_mreset(l_i2s_out);
  diag_mreset(r_i2s_out);
  diag_mreset(l_usb_out);
  diag_mreset(r_usb_out);
  diag_mreset(l_hdw_out);
  diag_mreset(r_hdw_out);
  diag_mreset(i2s_out);
  diag_mreset(usb_out);
}

/* debounce details */
static void diag_dreport(void) {
  Serial.printf("debounce: %.3f ms, %d samples\n", samples_to_ms(nrpn_get(KYRP_PIN_DEBOUNCE)), nrpn_get(KYRP_PIN_DEBOUNCE));
}

static const char *diag_lorem = 
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
  "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
  "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
  "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
  "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
  "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
  "culpa qui officia deserunt mollit anim id est laborum.";

/*
** output mixer on/off
** channel 1 usb_in for i2s_out or hdw_in, i2s_in for usb_out
** channel 2 sidetone
** channel 3 keyed IQ
** channel 4 - test stream
** loop over as many specs as are present
*/
static void diag_mixer3_set(const char *dev, RAudioMixer4& m, const char *p) {
  // Serial.printf("prepare for %s do_left %d do_right %d at \"%s\"\n", dev, dol, dor, p);
  for (int i = 0; i < 4; i += 1) {
    switch (*p++) {
    case '0': nrpn_set(m.enable_nrpn+i, 0); continue;
    case '1': nrpn_set(m.enable_nrpn+i, 1); continue;
    default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
    }
  }
}
static void diag_mixer2_set(const char *dev, RAudioMixer4& l, RAudioMixer4& r, const char *p) {
  Serial.printf("recognized device %s\n", dev);
  switch (*p) {
  case 'l': 
    diag_mixer3_set(dev, l, p+1);	/* left mixers */
    return;
  case 'r':
    diag_mixer3_set(dev, r, p+1);	/* right mixers */
    return;
  case '0':
  case '1': 
    diag_mixer3_set(dev, l, p); /* both mixers */
    diag_mixer3_set(dev, r, p); /* both mixers */
    return;
  default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
  }
}
static void diag_mixer_set(const char *p) {
  Serial.printf("mixer_set(\"%s\")\n", p);
  switch (*p) {
  case 'u': return diag_mixer2_set("usb", l_usb_out, r_usb_out, p+1); 
  case 'i': return diag_mixer2_set("i2s", l_i2s_out, r_i2s_out, p+1);
  case 'h': return diag_mixer2_set("hdw", l_hdw_out, r_hdw_out, p+1);
  default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
  }
}

static void diag_mixer_dump_one(const char *lbl, RAudioMixer4& mix, int level, int enable) {
  Serial.printf("%s: ", lbl);
  for (int i = 0; i < 4; i += 1) Serial.printf(" %4d %d %d, ", signed_value(nrpn_get(level+i)), nrpn_get(enable+i), mix.last_value(i));
  Serial.printf("\n");
}

static void diag_mixer_dump(void) {
  diag_mixer_dump_one("l_usb_out", l_usb_out, KYRP_MIX_USB_L0,KYRP_MIX_EN_USB_L0);
  diag_mixer_dump_one("r_usb_out", r_usb_out, KYRP_MIX_USB_R0,KYRP_MIX_EN_USB_R0);
  diag_mixer_dump_one("l_i2s_out", l_i2s_out, KYRP_MIX_I2S_L0,KYRP_MIX_EN_I2S_L0);
  diag_mixer_dump_one("r_i2s_out", r_i2s_out, KYRP_MIX_I2S_R0,KYRP_MIX_EN_I2S_R0);
  diag_mixer_dump_one("l_hdw_out", l_hdw_out, KYRP_MIX_HDW_L0,KYRP_MIX_EN_HDW_L0);
  diag_mixer_dump_one("r_hdw_out", r_hdw_out, KYRP_MIX_HDW_R0,KYRP_MIX_EN_HDW_R0);
}
static char diag_random_char(void) {
  char c;
  do c = random(0,128); while ( ! cwkey_text.valid_text(c)); 
  return c;
}

static char *diag_random_text(void) {
  static char buff[256];
  for (unsigned i = 0; i < sizeof(buff)-1; i += 1)
    buff[i] = diag_random_char();
  buff[255] = 0;
  return buff;
}


static const char *diag_text_send_ptr = NULL;
static const char *diag_text2_send_ptr = NULL;

static int diag_send_some_text(const char *text, KeyerText& input) {
  if (text == NULL) return 0;
  if (*text == 0) return 0;
  if ( ! input.valid_vox()) return 0;
  while (*text != 0 && input.can_send_text()) {
    // Serial.printf("sending %d\n", *text);
    input.send_text(*text++);
  }
  return *text != 0 && input.valid_vox();
}

static uint8_t diag_logging = 0;

char diag_debug_buffer[4][256];

struct { short nrpn; const char*name; } diag_nrpn[] = {
  -1, "MIDI",
  KYRP_MIDI_INPUTS, "_INPUTS",
  KYRP_MIDI_OUTPUTS, "_OUTPUTS",
  KYRP_MIDI_ECHOES, "_ECHOES",
  KYRP_MIDI_SENDS, "_SENDS",
  KYRP_MIDI_NOTES, "_NOTES",
  KYRP_MIDI_CTRLS, "_CTRLS",
  KYRP_MIDI_NRPNS, "_NRPNS",
  -1, "LISTENER", 
  KYRP_LISTENER_NODES, "_NODES",
  KYRP_LISTENER_LISTS, "_LISTS",
  KYRP_LISTENER_CALLS, "_CALLS",
  KYRP_LISTENER_FIRES, "_FIRES",
  KYRP_LISTENER_LOOPS, "_LOOPS",
  -1, "COUNTS",
  KYRP_LOOP, "LOOP",
  KYRP_SAMPLE, "SAMPLE",
  KYRP_UPDATE, "UPDATE",
  KYRP_MILLI, "1MS",
  KYRP_10MILLI, "10MS",
  KYRP_100MILLI, "100MS",

  -1, "MIDI",
  KYRP_CHANNEL, "CHANNEL",
  KYRP_INPUT_ENABLE, "IN_EN",
  KYRP_OUTPUT_ENABLE, "OUT_EN",
  KYRP_ECHO_ENABLE, "ECHO_EN",
  KYRP_LISTENER_ENABLE, "LISTEN_EN",

  -1, "ENABLE",
  KYRP_PIN_ENABLE, "PIN_EN",
  KYRP_POUT_ENABLE, "POUT_EN",
  KYRP_PADC_ENABLE, "ADC_EN",
  KYRP_ST_ENABLE, "ST_EN",
  KYRP_TX_ENABLE, "TX_EN",
  KYRP_IQ_ENABLE, "IQ_EN",
  KYRP_PTT_REQUIRE, "PTT_REQ",

  -1, "ENABLE",
  KYRP_RKEY_ENABLE, "RKEY_EN",
  KYRP_CW_AUTOPTT, "APTT",
  KYRP_RX_MUTE, "RX_MUTE",
  KYRP_MIC_HWPTT, "MIC_HWPTT",
  KYRP_CW_HWPTT, "CW_HWPTT",
  KYRP_HDW_IN_ENABLE, "HDW_IN_EN",
  KYRP_HDW_OUT_ENABLE, "HDW_OUT_EN",

  -1, "LEVELS",
  KYRP_VOLUME, "MAIN",
  KYRP_LEVEL, "ST",
  KYRP_IQ_LEVEL, "IQ",
  KYRP_I2S_LEVEL, "I2S",
  KYRP_HDW_LEVEL, "HDW",
  KYRP_ST_BALANCE, "ST_BAL",
  KYRP_IQ_BALANCE, "IQ_BAL",

  -1, "KEY",
  KYRP_TONE, "TON",
  KYRP_SPEED, "SPD",
  KYRP_SPEED_FRAC, "_FRAC",
  KYRP_WEIGHT, "WGT",
  KYRP_RATIO, "RAT",
  KYRP_FARNS, "FRNS",
  KYRP_COMP, "COMP",

  -1, "PTT",
  KYRP_HEAD_TIME, "HEAD",
  KYRP_TAIL_TIME, "TAIL",
  KYRP_HANG_TIME, "HANG",

  -1, "SHAPE",
  KYRP_RISE_TIME, "RISE",
  KYRP_RISE_RAMP, "RAMP",
  KYRP_FALL_TIME, "FALL",
  KYRP_FALL_RAMP, "RAMP",

  -1, "PAD", 
  KYRP_PAD_MODE, "_MODE",
  KYRP_PAD_SWAP, "_SWAP",
  KYRP_PAD_ADAPT, "_ADAPT",
  KYRP_AUTO_ILS, "AILS",
  KYRP_AUTO_IWS, "AIWS",
  KYRP_PAD_KEYER, "_KEYER",

  -1, "MISC",
  KYRP_ACTIVE_ST, "ACTIVE_ST",
  KYRP_MIXER_SLEW_RAMP, "L_SLEW_RMP",
  KYRP_MIXER_SLEW_TIME, "L_SLEW",
  KYRP_FREQ_SLEW_RAMP, "F_SLEW_RMP",
  KYRP_FREQ_SLEW_TIME, "F_SLEW",

  -1, "MISC",
  KYRP_MIX_ENABLE, "MIX_EN",
  KYRP_MIX_ENABLE_L, "MIX_EN_L",
  KYRP_MIX_ENABLE_R, "MIX_EN_R",
  KYRP_PIN_DEBOUNCE, "PIN_DEB",
  KYRP_POUT_LOGIC, "POUT_LOG",
  KYRP_PADC_RATE, "ADC_RATE",
  KYRP_XIQ_FREQ, "XIQ_FREQ",
  KYRP_XIQ_FREQ+1, "",
  
  KYRP_IQ_USB, "IQ_USB",

  -1, "PIN",
  KYRP_PIN0_PIN, "P0",
  KYRP_PIN1_PIN, "P1",
  KYRP_PIN2_PIN, "P2",
  KYRP_PIN3_PIN, "P3",
  KYRP_PIN4_PIN, "P4",
  KYRP_PIN5_PIN, "P5",
  KYRP_PIN6_PIN, "P6",
  KYRP_PIN7_PIN, "P7",

  -1, "POUT",
  KYRP_POUT0_PIN, "P0",
  KYRP_POUT1_PIN, "P1",
  KYRP_POUT2_PIN, "P2",
  KYRP_POUT3_PIN, "P3",
  KYRP_POUT4_PIN, "P4",
  KYRP_POUT5_PIN, "P5",
  KYRP_POUT6_PIN, "P6",
  KYRP_POUT7_PIN, "P7",

  -1, "ADC",
  KYRP_PADC0_PIN, "P0", KYRP_PADC0_VAL, "V", KYRP_PADC0_NRPN, "N",
  KYRP_PADC1_PIN, "P1", KYRP_PADC1_VAL, "V", KYRP_PADC1_NRPN, "N",
  KYRP_PADC2_PIN, "P2", KYRP_PADC2_VAL, "V", KYRP_PADC2_NRPN, "N",
  KYRP_PADC3_PIN, "P3", KYRP_PADC3_VAL, "V", KYRP_PADC3_NRPN, "N",
  -1, "ADC",
  KYRP_PADC4_PIN, "P4", KYRP_PADC4_VAL, "V", KYRP_PADC4_NRPN, "N",
  KYRP_PADC5_PIN, "P5", KYRP_PADC5_VAL, "V", KYRP_PADC5_NRPN, "N",
  KYRP_PADC6_PIN, "P6", KYRP_PADC6_VAL, "V", KYRP_PADC6_NRPN, "N",
  KYRP_PADC7_PIN, "P7", KYRP_PADC7_VAL, "V", KYRP_PADC7_NRPN, "N",

  0, NULL
};
void diag_nrpn_report(void) {
  int n = 0;
  for (int i = 0; diag_nrpn[i].nrpn != 0; i += 1) {
    if (++n > 14) { Serial.printf("\n"); n = 0; }
    if (diag_nrpn[i].nrpn == -1) {
      Serial.printf("\n%s: ", diag_nrpn[i].name); n = 0;
    } else
      Serial.printf(" %s %d,", diag_nrpn[i].name, nrpn_get(diag_nrpn[i].nrpn));
  }
  Serial.printf("\n");
#if 0
  Serial.printf("\tVOLUME %.2f", nrpn_to_db(nrpn_get(KYRP_VOLUME)));
  Serial.printf("\tINPUT_SELECT %d", nrpn_get(KYRP_INPUT_SELECT));
  Serial.printf("\tINPUT_LEVEL %.2f", nrpn_to_db(nrpn_get(KYRP_INPUT_LEVEL)));
  Serial.println("");
  Serial.printf("\tPTT_EN %d", nrpn_get(KYRP_PTT_REQUIRE));
  Serial.printf("\tTX_EN %d", nrpn_get(KYRP_TX_ENABLE));
  Serial.printf("\tST_EN %d", nrpn_get(KYRP_ST_ENABLE));
  Serial.printf("\tIQ_EN %d", nrpn_get(KYRP_IQ_ENABLE));
  Serial.printf("\tIQ_BAL %d", signed_value(nrpn_get(KYRP_IQ_BALANCE)));
  Serial.printf("\tST_BAL %d", nrpn_get(KYRP_ST_BALANCE));
  Serial.println("");

  Serial.printf("\tDEBOUNCE %d", nrpn_get(KYRP_PIN_DEBOUNCE));

  Serial.printf("\tHEAD_TIME %d", nrpn_get(KYRP_HEAD_TIME));
  Serial.printf("\tTAIL_TIME %d", nrpn_get(KYRP_TAIL_TIME));
  Serial.printf("\tHANG_TIME %d", nrpn_get(KYRP_HANG_TIME));
  Serial.println("");

  Serial.printf("\tRISE_TIME %d", nrpn_get(KYRP_RISE_TIME));
  Serial.printf("\tFALL_TIME %d", nrpn_get(KYRP_FALL_TIME));
  Serial.printf("\tRISE_RAMP %d", nrpn_get(KYRP_RISE_RAMP));
  Serial.printf("\tFALL_RAMP %d", nrpn_get(KYRP_FALL_RAMP));
  Serial.println("");

  Serial.printf("\tPAD_MODE %d", nrpn_get(KYRP_PAD_MODE));
  Serial.printf("\tPAD_SWAP %d", nrpn_get(KYRP_PAD_SWAP));
  Serial.printf("\tPAD_ADAPT %d", nrpn_get(KYRP_PAD_ADAPT));
  Serial.printf("\tAUTO_ILS %d", nrpn_get(KYRP_AUTO_ILS));
  Serial.printf("\tAUTO_IWS %d", nrpn_get(KYRP_AUTO_IWS));
  Serial.printf("\tPAD_KEYER %d", nrpn_get(KYRP_PAD_KEYER));
  Serial.println("");

  Serial.printf("\tCHANNEL %d", nrpn_get(KYRP_CHANNEL));
  Serial.println("");

  Serial.printf("\tPADC_ENABLE %d", nrpn_get(KYRP_PADC_ENABLE));
  Serial.printf("\tPADC*_NRPN %d", nrpn_get(KYRP_PADC0_NRPN));
  Serial.printf(" %d", nrpn_get(KYRP_PADC1_NRPN));
  Serial.printf(" %d", nrpn_get(KYRP_PADC2_NRPN));
  Serial.printf(" %d", nrpn_get(KYRP_PADC3_NRPN));
  Serial.printf(" %d", nrpn_get(KYRP_PADC4_NRPN));
  Serial.println();

  /* 64 morse code translations */
h  Serial.printf("\tMIX_ENABLE 0x%04x", nrpn_get(KYRP_MIX_ENABLE));
  Serial.println();

  Serial.printf("\tUSB_L0 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_L0)));
  Serial.printf("\t1 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_L1)));
  Serial.printf("\t2 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_L2)));
  Serial.printf("\t3 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_L3)));

  Serial.printf("\tR0 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_R0)));
  Serial.printf("\t1 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_R1)));
  Serial.printf("\t2 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_R2)));
  Serial.printf("\t3 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_USB_R3)));
  Serial.println();

  Serial.printf("\tI2S_L0 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_L0)));
  Serial.printf("\t1 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_L1)));
  Serial.printf("\t2 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_L2)));
  Serial.printf("\t3 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_L3)));

  Serial.printf("\tR0 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_R0)));
  Serial.printf("\t1 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_R1)));
  Serial.printf("\t2 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_R2)));
  Serial.printf("\t3 %.2f", nrpn_to_db(nrpn_get(KYRP_MIX_I2S_R3)));
  Serial.println();

  Serial.printf("\tHDW_L0 %d", nrpn_get(KYRP_MIX_HDW_L0));
  Serial.printf("\t1 %d", nrpn_get(KYRP_MIX_HDW_L1));
  Serial.printf("\t2 %d", nrpn_get(KYRP_MIX_HDW_L2));
  Serial.printf("\t3 %d", nrpn_get(KYRP_MIX_HDW_L3));

  Serial.printf("\tR0 %d", nrpn_get(KYRP_MIX_HDW_R0));
  Serial.printf("\t1 %d", nrpn_get(KYRP_MIX_HDW_R1));
  Serial.printf("\t2 %d", nrpn_get(KYRP_MIX_HDW_R2));
  Serial.printf("\t3 %d", nrpn_get(KYRP_MIX_HDW_R3));
  Serial.println();

  Serial.printf("\tTONE %.1f", nrpn_to_hertz(nrpn_get(KYRP_TONE)));
  Serial.printf("\tLEVEL %.2f", nrpn_to_db(nrpn_get(KYRP_LEVEL)));
  Serial.printf("\tSPEED %d", nrpn_get(KYRP_SPEED));
  Serial.printf("\tSPEED_FRAC %d", nrpn_get(KYRP_SPEED_FRAC));
  Serial.printf("\tFARNS %d", nrpn_get(KYRP_FARNS));
  Serial.printf("\tWEIGHT %d", nrpn_get(KYRP_WEIGHT));
  Serial.printf("\tRATIO %d", nrpn_get(KYRP_RATIO));
  Serial.printf("\tCOMP %.2f", samples_to_ms(nrpn_get(KYRP_COMP)));
  Serial.print("\n\t");
  
  /* keyer timings in samples for paddle and text keyers - scratch values */
  Serial.printf("\tDIT %ld", xnrpn_get(KYRP_XPER_DIT));
  Serial.printf("\tDAH %ld", xnrpn_get(KYRP_XPER_DAH));
  Serial.printf("\tIES %ld", xnrpn_get(KYRP_XPER_IES));
  Serial.printf("\tILS %ld", xnrpn_get(KYRP_XPER_ILS));
  Serial.printf("\tIWS %ld", xnrpn_get(KYRP_XPER_IWS));
  Serial.println("");
#endif
}

static char *diag_read_line() {
  static char buff[256];
  unsigned i = 0;
  for (int c = Serial.read(); c != '\n'; c = Serial.read())
    if (i < sizeof(buff)-1)
      buff[i++] = c;
  buff[i] = 0;
  return buff;
}

void diagnostics_setup() { diag_totalTime = 0; }

void diagnostics_loop() {
  // dump background debug messages
  static int elapsed_test = 0;
  static elapsedSamples timer1, timer2;
  if (diag_logging)
    for (int i = 0; i < 4; i += 1)
      if (diag_debug_buffer[i][0] != 0) {
	Serial.printf("%s\n", diag_debug_buffer[i]);
	diag_debug_buffer[i][0] = 0;
      }
  if ( ! diag_send_some_text(diag_text_send_ptr, cwkey_text)) diag_text_send_ptr = NULL;
#if KYR_N_TEXT > 1
  if ( ! diag_send_some_text(diag_text2_send_ptr, cwkey_text2)) diag_text2_send_ptr = NULL;
#endif
  
  if (elapsed_test) {
    if ((int)timer1 <= 0 && timer2 > 0) {
      Serial.printf("timer1 %d timer2 %d\n", (int)timer1, (int)timer2);
      timer2 = 0;
      if (timer1 == 0) elapsed_test = 0;
    }
  }

  if (Serial.available()) {
    char *p = diag_read_line();
    // Serial.printf("diag read '%s'\n", p);
    switch (*p) {
    default: break;
    case '?': 
      Serial.printf("hasak monitor usage:\n"
		    " n -> nrpn dump\n"
		    " s -> audio library resource usage\n"
		    " S -> detailed audio library resource usage\n"
		    " t ... -> timestamp + ... to Serial\n"
		    " r -> reset usage counts\n"
		    " R -> reset detailed usage counts\n"
		    " d -> debounce diagnostics\n"
		    " l -> logging toggle\n"
		    " w... -> send ... up to newline to text keyer\n"
		    " W -> send lorem ipsum to text keyer\n"
#if KYR_N_TEXT > 1
		    " k... -> send ... up to newline to text2 keyer\n"
		    " K -> send lorem ipsum to text2 keyer\n"
#endif
		    " Z -> send random to text keyer\n"
		    " e -> default mixers\n"
		    " f -> check mixers\n"
		    " h#### | hl#### | hr#### -> set (both | left | right) hdw out mixers, # is 0|1\n"
		    " i#### | il#### | ir#### -> set (both | left | right) i2s out mixers, # is 0|1\n"
		    " u#### | ul#### | ur#### -> set (both | left | right) usb out mixers, # is 0|1\n"
		    );
      break;
    case 'n': diag_nrpn_report(); break; /* full nrpn dump */
    case 's': diag_sreport(); break; /* short summary */
    case 'S': diag_Sreport(); break; /* long summary */
    case 't': diag_treport(p+1); break; /* timestamp */
    case 'r': diag_sreset(); break;	/* short reset */ 
    case 'R': diag_Sreset(); break;	/* long reset */
    case 'd': diag_dreport(); break; /* debounce stats */
    case 'E': elapsed_test ^= 1; timer1 = -10; timer2 = 0; break;
    case 'l': diag_logging ^= 1; Serial.printf("logging %s\n", diag_logging?"on":"off"); break;
    case 'w': diag_text_send_ptr = p+1; break;
    case 'W': diag_text_send_ptr = diag_lorem; break;
    case 'Z': diag_text_send_ptr = diag_random_text(); break;
#if KYR_N_TEXT > 1
    case 'k': diag_text2_send_ptr = p+1; break;
    case 'K': diag_text2_send_ptr = diag_lorem; break;
#endif
    case 'e': diag_mixer_set("i1100"); diag_mixer_set("h1100"); diag_mixer_set("u0010"); break;
    case 'f': diag_mixer_dump(); break;
    case 'h': diag_mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    case 'i': diag_mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    case 'u': diag_mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    }
  }
}  
