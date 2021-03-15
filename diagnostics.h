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
** diagnostics
*/

/*
** keep track of cycles during each sample buffer time
** used to normalize the cycle usage from the audio library
*/
uint32_t microsPerAudioBuffer = AUDIO_BLOCK_SAMPLES / AUDIO_SAMPLE_RATE * 1000000;
elapsedMicros perAudioBuffer;
uint32_t cpuCyclesRaw, cpuCyclesPerAudioBuffer, cpuCyclesPerAudioBufferMax;

/* usage report line */
static void report(const char *tag, int use, int umax, int mem, int mmax, int over, int under) {
  Serial.printf("%16s %5d %5d", tag, use, umax);
  if (mem || mmax || over || under) {
    Serial.printf(" %2d %2d", mem, mmax);
    if (over || under)
      Serial.printf(" ++%-5d --%-5d", over, under);
  }
  Serial.printf("\n");
}
/* report input/output module usage */
static void ireport(const char *tag, AudioInputSample &sample) {
  report(tag, sample.cpu_cycles, sample.cpu_cycles_max, 0, 0, sample.overruns(), sample.underruns());
}

static void oreport(const char *tag, AudioOutputSample &sample) {
  report(tag, sample.cpu_cycles, sample.cpu_cycles_max, 0, 0, sample.overruns(), sample.underruns());
}

/* generic audio module report and reset */
static void mreport(const char *tag, AudioStream &str) {
  report(tag, str.cpu_cycles, str.cpu_cycles_max, 0, 0, 0, 0);
}

/* summary report */
static void sreport(void) {
  float total = 100*(float)AudioStream::cpu_cycles_total/(float)cpuCyclesPerAudioBuffer;
  float totalMax =  100*(float)AudioStream::cpu_cycles_total_max/(float)cpuCyclesPerAudioBufferMax;
  Serial.printf("%16s %5.3f %5.3f %2d %2d active %d adc %d\n", "total", total, totalMax, AudioMemoryUsage(), AudioMemoryUsageMax(), get_active_vox(), _adc_out);
}

static void Sreport(void) {
  // report("total", AudioStream::cpu_cycles_total, AudioStream::cpu_cycles_total_max, AudioMemoryUsage(), AudioMemoryUsageMax(), 0, 0);
  // report("denom", cpuCyclesPerAudioBuffer, cpuCyclesPerAudioBufferMax, 0, 0, 0, 0);
  sreport();
  mreport("i2s_in", i2s_in);
  mreport("usb_in", usb_in);
  mreport("i2s_out", i2s_out);
  mreport("usb_out", usb_out);
  ireport("l_pad", l_pad);
  ireport("r_pad", r_pad);
  ireport("s_key", s_key);
  ireport("ptt sw", ptt_sw);
  mreport("winkey", wink); 
  mreport("kyr", kyr);
  mreport("paddle", paddle);
  mreport("arbiter", arbiter);
  mreport("tone_ramp", tone_ramp);
  mreport("key_ramp", key_ramp);
  mreport("l_i2s_out", l_i2s_out);
  mreport("r_i2s_out", r_i2s_out);
  mreport("l_usb_out", l_usb_out);
  mreport("r_usb_out", r_usb_out);
  mreport("l_hdw_out", l_hdw_out);
  mreport("r_hdw_out", r_hdw_out);
  oreport("key_out", key_out); 
  oreport("ptt out", ptt_out);
}

/* summary reset */
static void ireset(AudioInputSample &sample) {
  sample.reset();
  sample.processorUsageMaxReset();
}

static void oreset(AudioOutputSample &sample) {
  sample.reset();
  sample.processorUsageMaxReset();
}

static void mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}

static void sreset(void) {
  cpuCyclesPerAudioBufferMax = cpuCyclesPerAudioBuffer;
  AudioProcessorUsageMaxReset();
  AudioMemoryUsageMaxReset();
}

static void Sreset(void) {
  sreset();
  mreset(i2s_in);
  mreset(usb_in);
  ireset(l_pad);
  ireset(r_pad);
  ireset(s_key);
  ireset(ptt_sw);
  mreset(wink);
  mreset(kyr);
  mreset(paddle);
  mreset(arbiter);
  mreset(tone_ramp);
  mreset(key_ramp);
  mreset(l_i2s_out);
  mreset(r_i2s_out);
  mreset(l_usb_out);
  mreset(r_usb_out);
  mreset(l_hdw_out);
  mreset(r_hdw_out);
  oreset(key_out);
  oreset(ptt_out);
  mreset(i2s_out);
  mreset(usb_out);
}

static void usb_out_mix(int l, int r) {
  for (int i = 0; i < 4; i += 1) {
    l_usb_out.gain(i,0.0);
    r_usb_out.gain(i,0.0);
  }
  l_usb_out.gain(l,1.0); 	/* left channel */
  r_usb_out.gain(r,1.0);	/* right channel */
}

const char *lorem = 
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
  "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
  "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
  "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
  "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
  "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
  "culpa qui officia deserunt mollit anim id est laborum.";

char *read_line() {
  static char buff[128];
  unsigned i = 0;
  for (int c = Serial.read(); c != '\n'; c = Serial.read())
    if (i < sizeof(buff)-1) buff[i++] = c;
  buff[i] = 0;
  return buff;
}

void diagnostics_loop() {
  // accumulate cycle count to normalize audio library usage
  if (perAudioBuffer >= microsPerAudioBuffer) {
    perAudioBuffer = 0;
    cpuCyclesPerAudioBuffer = (ARM_DWT_CYCCNT - cpuCyclesRaw) >> 6;
    cpuCyclesRaw = ARM_DWT_CYCCNT;
    cpuCyclesPerAudioBufferMax = max(cpuCyclesPerAudioBufferMax, cpuCyclesPerAudioBuffer);
     if (cpuCyclesPerAudioBufferMax > 100000) cpuCyclesPerAudioBufferMax = cpuCyclesPerAudioBuffer;
  }

  if (Serial.available()) {
    uint8_t c = Serial.read();
    // Serial.printf("diag read '%c'\n", c);
    switch (c) {
    default: break;
    case '?': 
      Serial.printf("hasak monitor usage:\n"
		    " s -> audio library resource usage\n"
		    " S -> detailed audio library resource usage\n"
		    " r -> reset usage counts\n"
		    " R -> reset detailed usage counts\n"
		    " w... -> send ... up to newline to wink keyer\n"
		    " k... -> send ... up to newline to kyr keyer\n"
		    " a -> usb_out shows microphone and sidetone\n"
		    " b -> usb_out shows microphone and test stream\n"
		    " c -> usb_out shows test stream and sidetone\n"
		    " d -> usb_out shows channel 3\n"
		    " e -> default mixers\n"
		    " t -> send lorem ipsum to wink keyer\n"
		    " u -> send lorem ipsum to wink keyer\n"
		    );
      break;
    case 's': sreport(); /* short summary */; break;
    case 'r': sreset(); /* short reset */; break;
    case 'S': Sreport(); /* long summary */ break;
    case 'R': Sreset(); /* long reset */ break;
    case 'w': wink.send_text(read_line()); break;
    case 'k': kyr.send_text(read_line()); break;
    case 't': wink.send_text(lorem); break;
    case 'u': wink.send_text(lorem); break;
    case 'a': usb_out_mix(0, 2); /* microphone, sidetone */ break;
    case 'b': usb_out_mix(0, 3); /* microphone, test */ break;
    case 'c': usb_out_mix(3, 2); /* test, sidetone */ break;
    case 'd': usb_out_mix(3,3);	/* test1, test2 */ break;
    case 'e': // default mixers
      for (int i = 0; i < 4; i += 1) {
	l_usb_out.gain(i,1.0);
	r_usb_out.gain(i,1.0);
      }
      break;
    }
  }
}  
