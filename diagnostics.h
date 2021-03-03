/*
** diagnostics
*/
void ioreport(const char *tag, int usage, int max, int over, int under) {
  Serial.print(tag);
  Serial.print(" usage "); Serial.print(usage);
  Serial.print(" max usage "); Serial.print(max);
  Serial.print(" overruns: "); Serial.print(over);
  Serial.print(" underruns "); Serial.print(under);
  Serial.println();
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
  Serial.print(tag);
  Serial.print(" usage "); Serial.print(str.processorUsage());
  Serial.print(" max "); Serial.print(str.processorUsageMax());
  Serial.println();
}

void mreset(AudioStream &str) {
  str.processorUsageMaxReset();
}


void diagnostics_loop() {
  if (Serial.available()) {
    switch (Serial.read()) {
    case '?': 
      Serial.println("hasak monitor usage:");
      Serial.println(" 0 -> audio library resource usage");
      Serial.println(" 1 -> probe1 trace");
      Serial.println(" 2 -> probe2 trace");
      Serial.println(" r -> reset usage counts");
      break;
    case '0':
      Serial.print(" cpu usage "); Serial.print(AudioProcessorUsage()); 
      Serial.print(" max "); Serial.print(AudioProcessorUsageMax());
      // Serial.println();
      Serial.print(" mem usage "); Serial.print(AudioMemoryUsage());
      Serial.print(" max "); Serial.print(AudioMemoryUsageMax());
      Serial.println();
      ireport("left paddle", l_pad);
      ireport("right paddle", r_pad);
      ireport("straight key", s_key);
      ireport("ptt switch", ptt_sw);
      mreport("paddle", paddle);
      mreport("arbiter", arbiter);
      mreport("key_ramp", key_ramp);
      //mreport("sidetone", sine_I);
      //mreport("I_ramp", I_ramp);
      mreport("and_not_kyr", and_not_kyr);
      mreport("ptt_delay", ptt_delay);
      oreport("key out", key_out); 
      oreport("ptt out", ptt_out);
      break;
    case '1': preport("probe1", &_probe1, 5*44100); break;
    case '2': preport("probe2", &_probe2, 5*44100); break;
    case 'r':
      AudioProcessorUsageMaxReset();
      AudioMemoryUsageMaxReset();
      ireset(l_pad);
      ireset(r_pad);
      ireset(s_key);
      ireset(ptt_sw);
      mreset(paddle);
      mreset(arbiter);
      mreset(key_ramp);
      //mreset(sine_I);
      //mreset(I_ramp);
      mreset(and_not_kyr);
      mreset(ptt_delay);
      oreset(key_out);
      oreset(ptt_out);
      break;
    default:
      //Serial.print("buttonpolls: "); 
      //Serial.println(buttonpolls); 
      break;
    }
  }
}  
