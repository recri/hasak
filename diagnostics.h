/*
** diagnostics
*/
void ioreport(const char *tag, int usage, int max, int over, int under) {
  Serial.printf(" %s usage %d max %d", tag, usage, max);
  if (over != 0) Serial.printf(" overruns %d", over);
  if (under != 0) Serial.printf(" underruns %d", under);
  // Serial.printf("\n");
}
void ireport(const char *tag, AudioInputSample &sample) {
  ioreport(tag, sample.processorUsage(), sample.processorUsage(), sample.overruns(), sample.underruns());
}

void ireset(AudioInputSample &sample) {
  sample.reset();
  sample.processorUsageMaxReset();
}

void oreport(const char *tag, AudioOutputSample &sample) {
  ioreport(tag, sample.processorUsage(), sample.processorUsage(), sample.overruns(), sample.underruns());
}

void oreset(AudioOutputSample &sample) {
  sample.reset();
  sample.processorUsageMaxReset();
}

void preport(const char *tag, int16_t *p, int n) {
  int count = 0;
  int words = 0;
  int delay = 1.0/AUDIO_SAMPLE_RATE * 1e6;
  Serial.printf("%s %dus ", tag, delay);
  words += 2;
  for (int i = 0; i < n; i += 1) {
    if (*p != 0) {
      if (count >= 0) count += 1;
      else {
	Serial.printf("%d", count);
	count = 1;
	words += 1;
      }
    } else {
      if (count <= 0) count -= 1;
      else {
	Serial.printf("+%d", count);
	count = -1;
	words += 1;
      }
    }
    if (words > 10) { Serial.printf("\n"); words = 0; }
    delayMicroseconds(delay);
  }
  if (count > 0) Serial.printf("+%d\n", count);
  else Serial.printf("%d\n", count);
}

void mreport(const char *tag, AudioStream &str) {
  Serial.printf(" %s usage %d max %d", tag, str.processorUsage(), str.processorUsageMax());
}

void mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}

const char *lorem = 
  "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
  "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
  "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
  "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
  "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
  "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
  "culpa qui officia deserunt mollit anim id est laborum.";

void diagnostics_loop() {
  if (Serial.available()) {
    uint8_t c = Serial.read();
    // Serial.printf("diag read '%c'\n", c);
    switch (c) {
    case '?': 
      Serial.printf("hasak monitor usage:\n"
		    " 0 -> audio library resource usage\n"
		    " !... -> send ... up to newline to wink keyer\n"
		    " @... -> send ... up to newline to kyr keyer\n"
		    " l -> send lorem ipsum to wink keyer\n"
		    " 1 -> probe1 trace\n"
		    " 2 -> probe2 trace\n"
		    " r -> reset usage counts\n"
		    );
      break;
    case '!':
      for (int c = Serial.read(); c != '\n'; c = Serial.read())
	wink.send_text(c);
      break;
    case '@':
      for (int c = Serial.read(); c != '\n'; c = Serial.read())
	kyr.send_text(c);
      break;
    case 'l':
      wink.send_text(lorem);
      break;
    case 'D': // default mixers
      for (int i = 0; i < 4; i += 1) {
	l_usb_out.gain(i,1.0);
	r_usb_out.gain(i,1.0);
      }
      r_usb_out.gain(3,0.0);	/* turn off adc */
      break;
    case 'L': // latency test
      for (int i = 0; i < 4; i += 1) {
	l_usb_out.gain(i,0.0);
	r_usb_out.gain(i,0.0);
      }
      l_usb_out.gain(0,1.0);
      r_usb_out.gain(2,1.0);
      break;
    case 'A': // ADC input visualization
      for (int i = 0; i < 4; i += 1) {
	l_usb_out.gain(i,0.0);
	r_usb_out.gain(i,0.0);
      }
      l_usb_out.gain(2,1.0);
      r_usb_out.gain(3,1.0);
      break;
    case '0':
      Serial.printf(" cpu usage %d max %d", AudioProcessorUsage(), AudioProcessorUsageMax());
      Serial.printf(" mem usage %d max %d", AudioMemoryUsage(), AudioMemoryUsageMax());
      Serial.println();
      mreport("i2s_in", i2s_in);
      mreport("usb_in", usb_in);
      mreport("i2s_out", i2s_out);
      mreport("usb_out", usb_out);
      Serial.println();
      ireport("left paddle", l_pad);
      ireport("right paddle", r_pad);
      ireport("straight key", s_key);
      ireport("ptt switch", ptt_sw);
      Serial.println();
      mreport("winkey text", wink); 
      mreport("kyr text", kyr);
      mreport("paddle", paddle);
      Serial.println();
      mreport("arbiter", arbiter);
      mreport("key_ramp", key_ramp);
      mreport("and_not_kyr", and_not_kyr);
      mreport("ptt_delay", ptt_delay);
      Serial.println();
      mreport("l_i2s_out", l_i2s_out);
      mreport("r_i2s_out", r_i2s_out);
      mreport("l_usb_out", l_usb_out);
      mreport("r_usb_out", r_usb_out);
      mreport("l_hdw_out", l_hdw_out);
      mreport("r_hdw_out", r_hdw_out);
      Serial.println();
      oreport("key out", key_out); 
      oreport("ptt out", ptt_out);
      Serial.println();
      break;
    case '1': preport("probe1", &_probe1, 5*44100); break;
    case '2': preport("probe2", &_probe2, 5*44100); break;
    case 'r':
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
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
      mreset(key_ramp);
      mreset(and_not_kyr);
      mreset(ptt_delay);
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
      break;
    default:
      //Serial.print("buttonpolls: "); 
      //Serial.println(buttonpolls); 
      break;
    }
  }
}  
