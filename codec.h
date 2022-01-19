static bool codec_sgtl5000;
static bool codec_wm8960;

static void codec_enable(void) {
  codec_sgtl5000 = sgtl5000.enable();
  codec_wm8960 = wm8960.enable();
}

static void codec_sgtl5000_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VOLUME:
    sgtl5000.volume(value*0.007874f); break;
  case KYRP_MIC_PREAMP_GAIN:
    break;
  case KYRP_INPUT_SELECT:
    sgtl5000.inputSelect(value); break;
  case KYRP_MUTE_HEAD_PHONES: 
    if (value) sgtl5000.muteHeadphone(); else sgtl5000.unmuteHeadphone(); break;
  case KYRP_MUTE_LINE_OUT:
    if (value) sgtl5000.muteLineout(); else sgtl5000.unmuteLineout(); break;
  case KYRP_LINE_OUT_LEVEL:
    sgtl5000.lineOutLevel(value,value); break;
  case KYRP_LINE_OUT_LEVEL_L:
    sgtl5000.lineOutLevel(value,kyr_nrpn[KYRP_LINE_OUT_LEVEL_R]); break;
  case KYRP_LINE_OUT_LEVEL_R:
    sgtl5000.lineOutLevel(kyr_nrpn[KYRP_LINE_OUT_LEVEL_L],value); break;
  case KYRP_LINE_IN_LEVEL:
    sgtl5000.lineInLevel(value, value); break;
  case KYRP_LINE_IN_LEVEL_L:
    sgtl5000.lineInLevel(value,kyr_nrpn[KYRP_LINE_IN_LEVEL_R]); break;
  case KYRP_LINE_IN_LEVEL_R:
    sgtl5000.lineInLevel(kyr_nrpn[KYRP_LINE_IN_LEVEL_L],value); break;
  default:
    Serial.printf("uncaught sgtl5000 codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void codec_wm8960_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VOLUME:
    wm8960.volume(value*0.007874f); break;
  case KYRP_MIC_PREAMP_GAIN:
    break;
  case KYRP_INPUT_SELECT:
    wm8960.inputSelect(value); break;
  case KYRP_MUTE_HEAD_PHONES: 
    break;
  case KYRP_MUTE_LINE_OUT:
    break;
  case KYRP_LINE_OUT_LEVEL:
    break;
  case KYRP_LINE_OUT_LEVEL_L:
    break;
  case KYRP_LINE_OUT_LEVEL_R:
    break;
  case KYRP_LINE_IN_LEVEL:
    wm8960.leftInputVolume(value); wm8960.rightInputVolume(value); break;
  case KYRP_LINE_IN_LEVEL_L:
    wm8960.leftInputVolume(value); break;
  case KYRP_LINE_IN_LEVEL_R:
    wm8960.rightInputVolume(value); break;
  default:
    Serial.printf("uncaught wm8960 codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void codec_nrpn_set(const int16_t nrpn, const int16_t value) {
  if (codec_sgtl5000) codec_sgtl5000_set(nrpn, value);
  if (codec_wm8960) codec_wm8960_set(nrpn, value);
}

