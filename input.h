 
static void input_setup(void) {
}

static void input_nrpn_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case KYRP_VOLUME_POT:  
  case KYRP_ST_VOL_POT:
  case KYRP_ST_FREQ_POT:
  case KYRP_SPEED_POT:
    if (valid_pin(value)) pinMode(value, INPUT); break;
  }
}

static int input_read_pot(int pin, int *cooked) {
  // raw values are noisy 12 bit analog readings from pots
  // cooked values are 14 bit nrpn parameters
  // bin down to 8 bit values with hysteresis
  // multiply back up to 14 bit values
  if ( ! valid_pin(pin)) return 0;
  const int S1 = 4, S2 = S1+2;	/* bin at 8 bits */
  int raw = analogRead(pin)+analogRead(pin)+analogRead(pin)+analogRead(pin);
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

static int input_map_volume(int v) { return ((0x4000-v)>>7); }
static int input_map_st_freq(int v) { return ((0x4000-v)>>4)+300; }
static int input_map_speed(int v) {
  int speed = ((0x4000-v)>>8)+5;
  return speed <= 50 ? speed : 50+(speed-50)*5;
 }

static void input_loop(void) {
  static uint8_t loop = 0;
  static int raw_volume = 0, volume = 0;
  static int raw_st_vol = 0, st_vol = 0;
  static int raw_st_freq = 0, st_freq = 0;
  static int raw_speed = 0, speed = 0;
  // int report = 0;
  switch (loop++%4) {
  case 0:
    if (input_read_pot(get_nrpn(KYRP_VOLUME_POT), &raw_volume) && 
	input_map_volume(raw_volume) != volume) {
      volume = input_map_volume(raw_volume);
      nrpn_set(KYRP_HEAD_PHONE_VOLUME, volume);
      // report += 1;
    }
    break;
  case 1:
    if (input_read_pot(get_nrpn(KYRP_ST_VOL_POT), &raw_st_vol) &&
	input_map_volume(raw_st_vol) != st_vol) {
      st_vol = input_map_volume(raw_st_vol);
      nrpn_set(KYRP_LEVEL, st_vol);
      // report += 1;
    }
    break;
  case 2:
    if (input_read_pot(get_nrpn(KYRP_ST_FREQ_POT), &raw_st_freq) && 
	input_map_st_freq(raw_st_freq) != st_freq) {
      st_freq = input_map_st_freq(raw_st_freq);
      nrpn_set(KYRP_TONE, st_freq);
      // report += 1;
    }
    break;
  case 3:
    if (input_read_pot(get_nrpn(KYRP_SPEED_POT), &raw_speed) && 
	input_map_speed(raw_speed) != speed) {
      speed = input_map_speed(raw_speed);
      nrpn_set(KYRP_SPEED, speed);
      // report += 1;
    }
    break;
  }
  // if (report)
  //   Serial.printf("vol st_vol st_freq speed =  (%04x %4d) (%04x %4d) (%04x %4d) (%04x %4d)\n", 
  //		  raw_volume, volume, raw_st_vol, st_vol, raw_st_freq, st_freq, raw_speed, speed);
}
