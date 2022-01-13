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
static void report(const char *tag, uint32_t use, uint32_t umax) {
  Serial.printf("%16s %5.3f%% %5.3f%%", tag, timing_percent(use), timing_percent_max(umax));
  // Serial.printf("\n");
}

elapsedMicros totalTime;

/* summary report */
static void sreport(void) {
  float msPerIes = 1000.0f*get_vox_ies(get_active_vox())/AUDIO_SAMPLE_RATE;
  float msPerIls = 1000.0f*get_vox_ils(get_active_vox())/AUDIO_SAMPLE_RATE;
  float msPerIws = 1000.0f*get_vox_iws(get_active_vox())/AUDIO_SAMPLE_RATE;
  Serial.printf("\t%f sample rate %f buffer size %d F_CPU %.1f MHz\n", totalTime/1e6, AUDIO_SAMPLE_RATE, AUDIO_BLOCK_SAMPLES, F_CPU/1e6f);
  Serial.printf("\tactive %d ies/ils/iws %.1f/%.1f/%.1f ms\n", get_active_vox(), msPerIes, msPerIls, msPerIws);
  Serial.printf("\tnote %5d %5d nrpn %5d %5d\n", kyr_recv_note, kyr_send_note, kyr_recv_nrpn, kyr_send_nrpn);
  report("total", AudioStream::cpu_cycles_total, AudioStream::cpu_cycles_total_max);
  report("isr", isrCyclesPerAudioBuffer, isrCyclesPerAudioBufferMax);
  Serial.printf("%16s %2d %2d", "buffers", AudioMemoryUsage(), AudioMemoryUsageMax());
  Serial.printf("pollcount %ld\n", _pollcount);
}

static void treport(const char *p) {
  Serial.printf("\t%f %s\n", totalTime/1e6, p);
}

/* generic audio module report and reset */
static void mreport(const char *tag, AudioStream& str) {
  report(tag, str.cpu_cycles, str.cpu_cycles_max);
}

/* long report */
static void Sreport(void) {
  // report("total", AudioStream::cpu_cycles_total, AudioStream::cpu_cycles_total_max, AudioMemoryUsage(), AudioMemoryUsageMax(), 0, 0);
  // report("denom", cpuCyclesPerAudioBuffer, cpuCyclesPerAudioBufferMax, 0, 0, 0, 0);
  sreport();
  mreport("i2s_in", i2s_in);       mreport("usb_in", usb_in);       mreport("adc_in", adc_in);   Serial.println();
  mreport("i2s_out", i2s_out);     mreport("usb_out", usb_out);     mreport("hdw_out", hdw_out); Serial.println();
  mreport("l_pad", l_pad);         mreport("r_pad", r_pad);         mreport("s_key", s_key);     Serial.println();
  mreport("ptt sw", ptt_sw);       mreport("winkey", wink);         mreport("kyr", kyr);         Serial.println();
  mreport("paddle", paddle);       mreport("button", button);       mreport("arbiter", arbiter); Serial.println();
  mreport("tone_ramp", tone_ramp); mreport("key_ramp", key_ramp);   Serial.println();
  mreport("l_i2s_out", l_i2s_out); mreport("r_i2s_out", r_i2s_out); Serial.println();
  mreport("l_usb_out", l_usb_out); mreport("r_usb_out", r_usb_out); Serial.println();
  mreport("l_hdw_out", l_hdw_out); mreport("r_hdw_out", r_hdw_out); Serial.println();
  mreport("key_out", key_out);     mreport("ptt out", ptt_out);     Serial.println();
  mreport("up out", up_out);      mreport("down out", down_out);    Serial.println();
}

/* summary reset */
static void mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}

static void sreset(void) {
  cpuCyclesPerAudioBufferMax = cpuCyclesPerAudioBuffer;
  isrCyclesPerAudioBufferMax = isrCyclesPerAudioBuffer;
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

static void Sreset(void) {
  sreset();
  mreset(i2s_in);
  mreset(usb_in);
  mreset(adc_in);
  mreset(l_pad);
  mreset(r_pad);
  mreset(s_key);
  mreset(ptt_sw);
  mreset(wink);
  mreset(kyr);
  mreset(paddle);
  mreset(button);
  mreset(arbiter);
  mreset(tone_ramp);
  mreset(key_ramp);
  mreset(l_i2s_out);
  mreset(r_i2s_out);
  mreset(l_usb_out);
  mreset(r_usb_out);
  mreset(l_hdw_out);
  mreset(r_hdw_out);
  mreset(key_out);
  mreset(ptt_out);
  mreset(up_out);
  mreset(down_out);
  mreset(i2s_out);
  mreset(usb_out);
}

/* arbiter details */
static void areport(void) {
  Serial.printf("arbiter vox %d, stream %d, tail %d, head %d, delay %d\n",
		get_active_vox(), arbiter.active_stream, arbiter.active_tail, 
		arbiter.active_head, arbiter.active_delay);
  for (int i = 0; i < KYR_N_VOX; i += 1)
    Serial.printf("stream %d vox %d priority %d local %d\n", 
		  i, arbiter.vox[i], arbiter.priority[i], arbiter.local[i]);
  Serial.printf("keyq in %d items %d out %d, overrun %d, underrun %d, maxusage %d\n",
		arbiter.keyq.in, arbiter.keyq.items(), arbiter.keyq.out,
		arbiter.keyq.overrun, arbiter.keyq.underrun, arbiter.keyq.maxusage);
  Serial.printf("pttq in %d items %d out %d, overrun %d, underrun %d, maxusage %d\n",
		arbiter.pttq.in, arbiter.pttq.items(), arbiter.pttq.out,
		arbiter.pttq.overrun, arbiter.pttq.underrun, arbiter.pttq.maxusage);
}

/* button details */
static void breport(void) {
  Serial.printf("button thresholds: %d %d %d %d %d\n", get_button(0), get_button(1), get_button(2), get_button(3), get_button(4));
}

/* debounce details */
static void dreport(void) {
  Serial.printf("debounce: %d ms/10, %d samples\n", get_debounce(), tenth_ms_to_samples(get_debounce()));
}

static const char *lorem = 
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
static void mixer3_set(const char *dev, AudioMixer4& m, const char *p) {
  // Serial.printf("prepare for %s do_left %d do_right %d at \"%s\"\n", dev, dol, dor, p);
  for (int i = 0; i < 4; i += 1) {
    switch (*p++) {
    case '0': m.gain(i, 0.0f); continue;
    case '1': m.gain(i, 1.0f); continue;
    default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
    }
  }
}
static void mixer2_set(const char *dev, AudioMixer4& l, AudioMixer4& r, const char *p) {
  Serial.printf("recognized device %s\n", dev);
  switch (*p) {
  case 'l': 
    mixer3_set(dev, l, p+1);	/* left mixers */
    return;
  case 'r':
    mixer3_set(dev, r, p+1);	/* right mixers */
    return;
  case '0':
  case '1': 
    mixer3_set(dev, l, p); /* both mixers */
    mixer3_set(dev, r, p); /* both mixers */
    return;
  default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
  }
}
static void mixer_set(const char *p) {
  Serial.printf("mixer_set(\"%s\")\n", p);
  switch (*p) {
  case 'u': return mixer2_set("usb", l_usb_out, r_usb_out, p+1); 
  case 'i': return mixer2_set("i2s", l_i2s_out, r_i2s_out, p+1);
  case 'h': return mixer2_set("hdw", l_hdw_out, r_hdw_out, p+1);
  default: Serial.printf("unrecognized mixer string \"%s\"\n", p); return;
  }
}

static char random_char(void) {
  char c;
  do c = random(0,128); while ( ! wink.valid_text(c)); 
  return c;
}

static char *random_text(void) {
  static char buff[256];
  for (unsigned i = 0; i < sizeof(buff)-1; i += 1)
    buff[i] = random_char();
  buff[255] = 0;
  return buff;
}
    
static char *read_line() {
  static char buff[256];
  unsigned i = 0;
  for (int c = Serial.read(); c != '\n'; c = Serial.read())
    if (i < sizeof(buff)-1)
      buff[i++] = c;
  buff[i] = 0;
  return buff;
}

static const char *wink_send_ptr = NULL;
static const char *kyr_send_ptr = NULL;

static int send_some_text(const char *text, AudioInputText& input) {
  if (text == NULL) return 0;
  if (*text == 0) return 0;
  if ( ! input.valid_vox()) return 0;
  while (*text != 0 && input.can_send_text()) input.send_text(*text++);
  return *text != 0 && input.valid_vox();
}

static uint8_t logging = 0;

char debug_buffer[4][256];

void diagnostics_setup() { totalTime = 0; }

void diagnostics_loop() {
  // dump background debug messages
  if (logging)
    for (int i = 0; i < 4; i += 1)
      if (debug_buffer[i][0] != 0) {
	Serial.printf("%s\n", debug_buffer[i]);
	debug_buffer[i][0] = 0;
      }
  if ( ! send_some_text(wink_send_ptr, wink)) wink_send_ptr = NULL;
  if ( ! send_some_text(kyr_send_ptr, kyr)) kyr_send_ptr = NULL;
  if (Serial.available()) {
    char *p = read_line();
    // Serial.printf("diag read '%s'\n", p);
    switch (*p) {
    default: break;
    case '?': 
      Serial.printf("hasak monitor usage:\n"
		    " s -> audio library resource usage\n"
		    " S -> detailed audio library resource usage\n"
		    " t ... -> timestamp + ... to Serial\n"
		    " r -> reset usage counts\n"
		    " R -> reset detailed usage counts\n"
		    " a -> arbiter diagnostics\n"
		    " b -> button diagnostics\n"
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
    case 's': sreport(); break; /* short summary */
    case 'S': Sreport(); break; /* long summary */
    case 't': treport(p+1); break; /* timestamp */
    case 'r': sreset(); break;	/* short reset */ 
    case 'R': Sreset(); break;	/* long reset */
    case 'a': areport(); break; /* arbiter stats */
    case 'b': breport(); break; /* button stats */
    case 'd': dreport(); break; /* debounce stats */
    case 'l': logging ^= 1; Serial.printf("logging %s\n", logging?"on":"off"); break;
    case 'w': wink_send_ptr = p+1; break;
    case 'k': kyr_send_ptr = p+1; break;
    case 'W': wink_send_ptr = lorem; break;
    case 'K': kyr_send_ptr = lorem; break;
    case 'Z': wink_send_ptr = random_text(); break;
    case 'e': mixer_setup(); /* default mixers */ break;
    case 'h': mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    case 'i': mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    case 'u': mixer_set(p); Serial.printf("mixer_set(%s) returned\n", p); break;
    }
  }
}  
