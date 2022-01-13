 
static void input_setup(void) {
#if KYRC_ENABLE_POTS
  pinMode(KYR_VOLUME_POT, INPUT);
  pinMode(KYR_ST_VOL_POT, INPUT);
  pinMode(KYR_ST_FREQ_POT, INPUT);
  pinMode(KYR_SPEED_POT, INPUT);
  analogReadRes(12);		/* 12 bits of resolution */
  analogReadAveraging(32);	/* 32 hardware read (maximum) averaging,  */
#endif
}

static int read_pot(int pin, int *cooked) {
  // raw values are noisy 12 bit analog readings from pots
  // cooked values are 14 bit nrpn parameters
  // bin down to 8 bit values with hysteresis
  // multiply back up to 14 bit values
  const int S1 = 4, S2 = S1+2;	/* bin at 8 bits */
  int raw = analogRead(pin);
  int rawbin = raw >> S1, cookedbin = *cooked >> S2;
  if (rawbin > cookedbin+1) {
    *cooked = ((rawbin-1)<<S2);
    return 1;
  } else if (rawbin < cookedbin-1) {
    *cooked = ((rawbin+1)<<S2);
    return 1;
  } else {
    return 0;
  }
}

static int map_volume(int v) { return ((0x4000-v)>>7); }
static int map_st_freq(int v) { return ((0x4000-v)>>4)+300; }
static int map_speed(int v) {
  int speed = ((0x4000-v)>>8)+5;
  return speed <= 50 ? speed : 50+(speed-50)*5;
 }

static void input_loop(void) {
#if KYRC_ENABLE_POTS
  static uint8_t loop = 0;
  static int raw_volume = 0, volume = 0;
  static int raw_st_vol = 0, st_vol = 0;
  static int raw_st_freq = 0, st_freq = 0;
  static int raw_speed = 0, speed = 0;
  // int report = 0;
  switch (loop++%4) {
  case 0:
    if (read_pot(KYR_VOLUME_POT, &raw_volume) && 
	map_volume(raw_volume) != volume) {
      volume = map_volume(raw_volume);
      nrpn_set(KYRP_HEAD_PHONE_VOLUME, volume);
      // report += 1;
    }
    break;
  case 1:
    if (read_pot(KYR_ST_VOL_POT, &raw_st_vol) &&
	map_volume(raw_st_vol) != st_vol) {
      st_vol = map_volume(raw_st_vol);
      nrpn_set(KYRP_LEVEL, st_vol);
      // report += 1;
    }
    break;
  case 2:
    if (read_pot(KYR_ST_FREQ_POT, &raw_st_freq) && 
	map_st_freq(raw_st_freq) != st_freq) {
      st_freq = map_st_freq(raw_st_freq);
      nrpn_set(KYRP_TONE, st_freq);
      // report += 1;
    }
    break;
  case 3:
    if (read_pot(KYR_SPEED_POT, &raw_speed) && 
	map_speed(raw_speed) != speed) {
      speed = map_speed(raw_speed);
      nrpn_set(KYRP_SPEED, speed);
      // report += 1;
    }
    break;
  }
  // if (report)
  //   Serial.printf("vol st_vol st_freq speed =  (%04x %4d) (%04x %4d) (%04x %4d) (%04x %4d)\n", 
  //		  raw_volume, volume, raw_st_vol, st_vol, raw_st_freq, st_freq, raw_speed, speed);
#endif
}
