#if 0
// WM8960 controls
    bool enable(void);
    bool disable(void) { return false; }
    bool inputLevel(float n) { return false; } // range: 0.0f to 1.0f
    bool inputSelect(int n) { return false; }
    bool volume(float n);
    bool leftInputVolume(unsigned int v);
    bool rightInputVolume(unsigned int v);
    bool leftInputMute(unsigned int v);
    bool rightInputMute(unsigned int v);
    bool leftInputZC(unsigned int v);
    bool rightInputZC(unsigned int v);
    bool leftHeadphoneVolume(unsigned int v);
    bool rightHeadphoneVolume(unsigned int v);
    bool leftHeadphoneZC(unsigned int v);
    bool rightHeadphoneZC(unsigned int v);
    bool enableDAC6dBAttenuate(unsigned int v);
    bool muteDAC(unsigned int v);
    bool enableDeemphasis(unsigned int v);
    bool enableADCHPF(unsigned int v);
    bool leftDACVolume(unsigned int v);
    bool rightDACVolume(unsigned int v);
    bool leftADCVolume(unsigned int v);
    bool rightADCVolume(unsigned int v);
    bool leftInputPower(unsigned int v);
    bool rightInputPower(unsigned int v);
    bool leftADCPower(unsigned int v);
    bool rightADCPower(unsigned int v);
    bool micBiasPower(unsigned int v);
    bool leftDACPower(unsigned int v);
    bool rightDACPower(unsigned int v);
    bool leftHeadphonePower(unsigned int v);
    bool rightHeadphonePower(unsigned int v);
    bool leftSpeakerPower(unsigned int v);
    bool rightSpeakerPower(unsigned int v);
#endif
#if 0
// SGTL5000 controls
	bool enable(void);//For Teensy LC the SGTL acts as master, for all other Teensys as slave.
	bool enable(const unsigned extMCLK, const uint32_t pllFreq = (4096.0l * AUDIO_SAMPLE_RATE_EXACT) ); //With extMCLK > 0, the SGTL acts as Master
	bool disable(void) { return false; }
	bool volume(float n) { return volumeInteger(n * 129 + 0.499f); }
	bool inputLevel(float n) {return false;}
	bool muteHeadphone(void) { return write(0x0024, ana_ctrl | (1<<4)); }
	bool unmuteHeadphone(void) { return write(0x0024, ana_ctrl & ~(1<<4)); }
	bool muteLineout(void) { return write(0x0024, ana_ctrl | (1<<8)); }
	bool unmuteLineout(void) { return write(0x0024, ana_ctrl & ~(1<<8)); }
	bool inputSelect(int n) {
		if (n == AUDIO_INPUT_LINEIN) {
			return write(0x0020, 0x055) // +7.5dB gain (1.3Vp-p full scale)
			 && write(0x0024, ana_ctrl | (1<<2)); // enable linein
		} else if (n == AUDIO_INPUT_MIC) {
			return write(0x002A, 0x0173) // mic preamp gain = +40dB
			 && write(0x0020, 0x088)     // input gain +12dB (is this enough?)
			 && write(0x0024, ana_ctrl & ~(1<<2)); // enable mic
		} else {
			return false;
		}
	}
	bool headphoneSelect(int n) {
		if (n == AUDIO_HEADPHONE_DAC) {
			return write(0x0024, ana_ctrl | (1<<6)); // route DAC to headphones out
		} else if (n == AUDIO_HEADPHONE_LINEIN) {
			return write(0x0024, ana_ctrl & ~(1<<6)); // route linein to headphones out
		} else {
			return false;
		}
	}
	bool volume(float left, float right);
	bool micGain(unsigned int dB);
	bool lineInLevel(uint8_t n) { return lineInLevel(n, n); }
	bool lineInLevel(uint8_t left, uint8_t right);
	unsigned short lineOutLevel(uint8_t n);
	unsigned short lineOutLevel(uint8_t left, uint8_t right);
	unsigned short dacVolume(float n);
	unsigned short dacVolume(float left, float right);
	bool dacVolumeRamp();
	bool dacVolumeRampLinear();
	bool dacVolumeRampDisable();
	unsigned short adcHighPassFilterEnable(void);
	unsigned short adcHighPassFilterFreeze(void);
	unsigned short adcHighPassFilterDisable(void);
	unsigned short audioPreProcessorEnable(void);
	unsigned short audioPostProcessorEnable(void);
	unsigned short audioProcessorDisable(void);
	unsigned short eqFilterCount(uint8_t n);
	unsigned short eqSelect(uint8_t n);
	unsigned short eqBand(uint8_t bandNum, float n);
	void eqBands(float bass, float mid_bass, float midrange, float mid_treble, float treble);
	void eqBands(float bass, float treble);
	void eqFilter(uint8_t filterNum, int *filterParameters);
	unsigned short autoVolumeControl(uint8_t maxGain, uint8_t lbiResponse, uint8_t hardLimit, float threshold, float attack, float decay);
	unsigned short autoVolumeEnable(void);
	unsigned short autoVolumeDisable(void);
	unsigned short enhanceBass(float lr_lev, float bass_lev);
	unsigned short enhanceBass(float lr_lev, float bass_lev, uint8_t hpf_bypass, uint8_t cutoff);
	unsigned short enhanceBassEnable(void);
	unsigned short enhanceBassDisable(void);
	unsigned short surroundSound(uint8_t width);
	unsigned short surroundSound(uint8_t width, uint8_t select);
	unsigned short surroundSoundEnable(void);
	unsigned short surroundSoundDisable(void);
	void killAutomation(void) { semi_automated=false; }
	void setMasterMode(uint32_t freqMCLK_in);
#endif
static void codec_enable(void) {
  /* enable codec */
  /* this causes a loud pop with cap coupled head phone outputs */
  sgtl5000.enable();
  wm8960.enable();
}

static void codec_nrpn_set(uint16_t nrpn, uint16_t value) {
  switch (nrpn) {
  case KYRP_HEAD_PHONE_VOLUME: {
    float volume = value*7.87401574803e-3f; // 7 bit unsigned to 0.0 .. 1.0
    sgtl5000.volume(volume);
    wm8960.volume(volume);
    break;
  }
  case KYRP_INPUT_SELECT:
    sgtl5000.inputSelect(value);
    wm8960.inputSelect(value);
    break;
  case KYRP_MUTE_HEAD_PHONES:
    if (value) {
      sgtl5000.muteHeadphone();
      // wm8960.muteHeadphone();
    } else {
      sgtl5000.unmuteHeadphone();
      // wm8960.unmuteHeadphone();
    }
    break;
  case KYRP_MUTE_LINE_OUT:
    if (value) {
      sgtl5000.muteLineout(); 
      // wm8960.muteLineout(); 
    } else {
      sgtl5000.unmuteLineout();
      // wm8960.unmuteLineout();
    }
    break;
  case KYRP_LINE_OUT_LEVEL:
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]);
    // wm8960.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]);
    break;
  case KYRP_LINE_OUT_LEVEL_L:
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); 
    // wm8960.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); 
    break;
  case KYRP_LINE_OUT_LEVEL_R:
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]);
    // wm8960.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]);
    break;
  case KYRP_LINE_IN_LEVEL:
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]);
    wm8960.leftInputVolume(kyr_nrpn[KYRP_LINE_IN_LEVEL_L]);
    wm8960.rightInputVolume(kyr_nrpn[KYRP_LINE_IN_LEVEL_R]);
    break;
  case KYRP_LINE_IN_LEVEL_L:
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]);
    wm8960.leftInputVolume(kyr_nrpn[KYRP_LINE_IN_LEVEL_L]);
    break;
  case KYRP_LINE_IN_LEVEL_R:
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],kyr_nrpn[KYRP_LINE_IN_LEVEL_R]);
    wm8960.rightInputVolume(kyr_nrpn[KYRP_LINE_IN_LEVEL_R]);
    break;
  case KYRP_MIC_PREAMP_GAIN:
    // sgtl5000.micPreampGain(value);
    // wm8960.micPreampGain(value);
    break;
  default:
    Serial.printf("uncaught codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}
