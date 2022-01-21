#if 0
	virtual bool volume(float volume) = 0;      // volume 0.0 to 1.0
	virtual bool inputLevel(float volume) = 0;  // volume 0.0 to 1.0
	virtual bool inputSelect(int n) = 0;
#endif
static bool codec_sgtl5000;
static bool codec_wm8960;

static void codec_enable(void) {
  codec_sgtl5000 = sgtl5000.enable();
  codec_wm8960 = wm8960.enable();
}

static void codec_sgtl5000_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VOLUME: sgtl5000.volume(value*0.007874f); break;
  case KYRP_INPUT_LEVEL: sgtl5000.inputLevel(value*0.007874f); break;
  case KYRP_INPUT_SELECT: sgtl5000.inputSelect(value); break;
  default: Serial.printf("uncaught sgtl5000 codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void codec_wm8960_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VOLUME: wm8960.volume(value*0.007874f); break;
  case KYRP_INPUT_LEVEL: wm8960.inputLevel(value*0.007874f); break;
  case KYRP_INPUT_SELECT: wm8960.inputSelect(value); break;
  default: Serial.printf("uncaught wm8960 codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void codec_nrpn_set(const int16_t nrpn, const int16_t value) {
  if (codec_sgtl5000) codec_sgtl5000_set(nrpn, value);
  if (codec_wm8960) codec_wm8960_set(nrpn, value);
}

