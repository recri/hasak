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

/* summary report */
static void sreport(void) {
  float msPerIes = 1000.0f*get_vox_ies(get_active_vox())/AUDIO_SAMPLE_RATE;
  float msPerIls = 1000.0f*get_vox_ils(get_active_vox())/AUDIO_SAMPLE_RATE;
  float msPerIws = 1000.0f*get_vox_iws(get_active_vox())/AUDIO_SAMPLE_RATE;
  Serial.printf("\tsample rate %f buffer size %d\n", AUDIO_SAMPLE_RATE, AUDIO_BLOCK_SAMPLES);
  Serial.printf("\tactive %d ies/ils/iws %.1f/%.1f/%.1f ms\n", get_active_vox(), msPerIes, msPerIls, msPerIws);
  report("total", AudioStream::cpu_cycles_total, AudioStream::cpu_cycles_total_max);
  report("isr", isrCyclesPerAudioBuffer, isrCyclesPerAudioBufferMax);
  Serial.printf("%16s %2d %2d", "buffers", AudioMemoryUsage(), AudioMemoryUsageMax());
  Serial.printf("\n");
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

static void mixer_set(const char *p) {
  AudioMixer4& left = l_usb_out;
  AudioMixer4& right = r_usb_out;
  const char *dev = "unk";
  switch (p[0]) {
  case 'u': dev = "usb"; left = l_usb_out; right = r_usb_out; break;
  case 'i': dev = "i2s"; left = l_i2s_out; right = r_i2s_out; break;
  case 'h': dev = "hdw"; left = l_hdw_out; right = r_hdw_out; break;
  default: Serial.printf("unrecognized mixer string %s\n", p); return;
  }
  p += 1;
  switch (p[0]) {
  case 'l': /* left mixers */
    for (int i = 0; i < 4; i += 1) {
      Serial.printf("l_%s_out.gain(%d,%c)\n", dev, i, p[1+i]);
      left.gain(i,p[1+i]=='1'?1.0: 0.0);
    }
    break;
  case 'r': /* right mixers */
    for (int i = 0; i < 4; i += 1) {
      Serial.printf("r_%s_out.gain(%d,%c)\n", dev, i, p[1+i]);
      right.gain(i,p[1+i]=='1'?1.0: 0.0);
    }
    break;
  case '0': case '1': /* both mixers */
    for (int i = 0; i < 4; i += 1) {
      Serial.printf("*_%s_out.gain(%d,%c)\n", dev, i, p[i]);
      left.gain(i,p[i]=='1'?1.0: 0.0);
      right.gain(i,p[i]=='1'?1.0: 0.0);
    }
    break;
  }
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

void diagnostics_loop() {
  if (Serial.available()) {
    char *p = read_line();
    // Serial.printf("diag read '%s'\n", p);
    switch (*p) {
    default: break;
    case '?': 
      Serial.printf("hasak monitor usage:\n"
		    " s -> audio library resource usage\n"
		    " S -> detailed audio library resource usage\n"
		    " r -> reset usage counts\n"
		    " R -> reset detailed usage counts\n"
		    " a -> arbiter diagnostics\n"
		    " b -> button diagnostics\n"
		    " d -> debounce diagnostics\n"
		    " w... -> send ... up to newline to wink keyer\n"
		    " k... -> send ... up to newline to kyr keyer\n"
		    " W -> send lorem ipsum to wink keyer\n"
		    " K -> send lorem ipsum to kyr keyer\n"
		    " e -> default mixers\n"
		    " h#### | hl#### | hr#### -> set (both | left | right) hdw out mixers, # is 0|1\n"
		    " i#### | il#### | ir#### -> set (both | left | right) i2s out mixers, # is 0|1\n"
		    " u#### | ul#### | ur#### -> set (both | left | right) usb out mixers, # is 0|1\n"
		    );
      break;
    case 's': sreport(); /* short summary */; break;
    case 'S': Sreport(); /* long summary */ break;
    case 'r': sreset(); /* short reset */ break;
    case 'R': Sreset(); /* long reset */ break;
    case 'a': areport(); /* arbiter stats */ break;
    case 'b': breport(); /* button stats */ break;
    case 'd': dreport(); /* debounce stats */ break;
    case 'w': wink.send_text(p+1); break;
    case 'k': kyr.send_text(p+1); break;
    case 'W': wink.send_text(lorem); break;
    case 'K': kyr.send_text(lorem); break;
    case 'e': mixer_setup(); /* default mixers */ break;
    case 'u': 
    case 'i': 
    case 'h': mixer_set(p); break;
    }
  }
}  
