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
#if defined(KYR_ENABLE_ADC_IN)
  diag_mreport("i2s_in", i2s_in);       diag_mreport("usb_in", usb_in);       diag_mreport("adc_in", adc_in);   Serial.println();
#else
  diag_mreport("i2s_in", i2s_in);       diag_mreport("usb_in", usb_in);       Serial.println();
#endif
#if defined(KYR_ENABLE_HDW_OUT)
  diag_mreport("i2s_out", i2s_out);     diag_mreport("usb_out", usb_out);     diag_mreport("hdw_out", hdw_out); Serial.println();
#else
  diag_mreport("i2s_out", i2s_out);     diag_mreport("usb_out", usb_out);     Serial.println();
#endif
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
#if defined(KYR_ENABLE_ADC_IN)
  diag_mreset(adc_in);
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
  Serial.printf("debounce: %.3f ms, %d samples\n", samples_to_ms(nrpn_get(KYRP_DEBOUNCE)), nrpn_get(KYRP_DEBOUNCE));
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
static void diag_mixer3_set(const char *dev, AudioMixer4& m, const char *p) {
  // Serial.printf("prepare for %s do_left %d do_right %d at \"%s\"\n", dev, dol, dor, p);
  for (int i = 0; i < 4; i += 1) {
    switch (*p++) {
    case '0': m.gain(i, 0.0f); continue;
    case '1': m.gain(i, 1.0f); continue;
    default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
    }
  }
}
static void diag_mixer2_set(const char *dev, AudioMixer4& l, AudioMixer4& r, const char *p) {
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

static char diag_random_char(void) {
  char c;
  do c = random(0,128); while ( ! keyer_text.valid_text(c)); 
  return c;
}

static char *diag_random_text(void) {
  static char buff[256];
  for (unsigned i = 0; i < sizeof(buff)-1; i += 1)
    buff[i] = diag_random_char();
  buff[255] = 0;
  return buff;
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

static const char *diag_text_send_ptr = NULL;
static const char *diag_text2_send_ptr = NULL;

static int diag_send_some_text(const char *text, KeyerText& input) {
  if (text == NULL) return 0;
  if (*text == 0) return 0;
  if ( ! input.valid_vox()) return 0;
  while (*text != 0 && input.can_send_text()) input.send_text(*text++);
  return *text != 0 && input.valid_vox();
}

static uint8_t diag_logging = 0;

char diag_debug_buffer[4][256];

void diag_nrpn_report(void) {
  Serial.printf("\tVOLUME %.2f", nrpn_to_db(nrpn_get(KYRP_VOLUME)));
  Serial.printf("\tINPUT_SELECT %d", nrpn_get(KYRP_INPUT_SELECT));
  Serial.printf("\tINPUT_LEVEL %.2f", nrpn_to_db(nrpn_get(KYRP_INPUT_LEVEL)));
  Serial.println("");
  Serial.printf("\tBUTTON_* %d, %d, %d, %d, %d", 
		signed_value(nrpn_get(KYRP_BUTTON_0)), signed_value(nrpn_get(KYRP_BUTTON_1)),
		signed_value(nrpn_get(KYRP_BUTTON_2)), signed_value(nrpn_get(KYRP_BUTTON_3)),
		signed_value(nrpn_get(KYRP_BUTTON_4)));
  Serial.println("");
  Serial.printf("\tPTT_EN %d", nrpn_get(KYRP_PTT_ENABLE));
  Serial.printf("\tTX_EN %d", nrpn_get(KYRP_TX_ENABLE));
  Serial.printf("\tST_EN %d", nrpn_get(KYRP_ST_ENABLE));
  Serial.printf("\tIQ_EN %d", nrpn_get(KYRP_IQ_ENABLE));
  Serial.printf("\tADJ %d", signed_value(nrpn_get(KYRP_IQ_ADJUST)));
  Serial.printf("\tBAL %d", signed_value(nrpn_get(KYRP_IQ_BALANCE)));
  Serial.println("");
  Serial.printf("\tST_PAN %d", nrpn_get(KYRP_ST_PAN));

  Serial.printf("\tDEBOUNCE %d", nrpn_get(KYRP_DEBOUNCE));

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

  Serial.printf("\tCHAN_CC %d", nrpn_get(KYRP_CHAN_CC));
  Serial.printf("\tNOTE %d", nrpn_get(KYRP_CHAN_NOTE));
  Serial.println("");

  Serial.printf("\tNOTE_L_PAD %d", nrpn_get(KYRP_NOTE_L_PAD));
  Serial.printf("\t_R_PAD %d", nrpn_get(KYRP_NOTE_R_PAD));
  Serial.printf("\t_S_KEY %d", nrpn_get(KYRP_NOTE_S_KEY));
  Serial.printf("\t_EXT_PTT %d", nrpn_get(KYRP_NOTE_EXT_PTT));
  Serial.printf("\t_KEY_OUT %d", nrpn_get(KYRP_NOTE_KEY_OUT));
  Serial.printf("\t_PTT_OUT %d", nrpn_get(KYRP_NOTE_PTT_OUT));
  Serial.println("");

  Serial.printf("\tADC_ENABLE 0x%02x", nrpn_get(KYRP_ADC_ENABLE));
  Serial.printf("\tADC*_CONTROL %d", nrpn_get(KYRP_ADC0_CONTROL));
  Serial.printf(" %d", nrpn_get(KYRP_ADC1_CONTROL));
  Serial.printf(" %d", nrpn_get(KYRP_ADC2_CONTROL));
  Serial.printf(" %d", nrpn_get(KYRP_ADC3_CONTROL));
  Serial.printf(" %d", nrpn_get(KYRP_ADC4_CONTROL));
  Serial.println();

  /* 64 morse code translations */
  Serial.printf("\tOUT_ENABLE 0x%04x", nrpn_get(KYRP_OUT_ENABLE));
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
  if ( ! diag_send_some_text(diag_text_send_ptr, keyer_text)) diag_text_send_ptr = NULL;
  if ( ! diag_send_some_text(diag_text2_send_ptr, keyer_text2)) diag_text2_send_ptr = NULL;

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
		    " w... -> send ... up to newline to wink keyer\n"
		    " k... -> send ... up to newline to kyr keyer\n"
		    " W -> send lorem ipsum to wink keyer\n"
		    " K -> send lorem ipsum to kyr keyer\n"
		    " Z -> send random to wink keyer\n"
		    " e -> default mixers\n"
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
    case 'l': diag_logging ^= 1; Serial.printf("logging %s\n", diag_logging?"on":"off"); break;
    case 'E': elapsed_test ^= 1; timer1 = -10; timer2 = 0; break;
    case 'w': diag_text_send_ptr = p+1; break;
    case 'k': diag_text2_send_ptr = p+1; break;
    case 'W': diag_text_send_ptr = diag_lorem; break;
    case 'K': diag_text2_send_ptr = diag_lorem; break;
    case 'Z': diag_text_send_ptr = diag_random_text(); break;
    case 'e': diag_mixer_set("i1100"); diag_mixer_set("h1100"); diag_mixer_set("u0010"); break;
    case 'h': diag_mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    case 'i': diag_mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    case 'u': diag_mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    }
  }
}  
