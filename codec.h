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
#if 0
	virtual bool volume(float volume) = 0;      // volume 0.0 to 1.0
	virtual bool inputLevel(float volume) = 0;  // volume 0.0 to 1.0
	virtual bool inputSelect(int n) = 0;
#endif
static bool codec_sgtl5000;
static bool codec_wm8960;

static int codec_identify(void) {
  if (codec_sgtl5000 && codec_wm8960) return 5000+8960;
  if (codec_sgtl5000) return 5000;
  if (codec_wm8960) return 8960;
  return 0;
}

static void codec_enable(void) {
  codec_sgtl5000 = sgtl5000.enable();
  codec_wm8960 = wm8960.enable();
}

static void codec_sgtl5000_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case NRPN_VOLUME: sgtl5000.volume(tenthdbtolinear(signed_value(value))); break;
  case NRPN_INPUT_LEVEL: sgtl5000.inputLevel(tenthdbtolinear(signed_value(value))); break;
  case NRPN_INPUT_SELECT: sgtl5000.inputSelect(value); break;
  default: Serial.printf("uncaught sgtl5000 codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void codec_wm8960_set(const int16_t nrpn, const int16_t value) {
  switch (nrpn) {
  case NRPN_VOLUME: 
    // Serial.printf("set volume %d -> %f\n", signed_value(value), tenthdbtolinear(signed_value(value))/127.0);
    wm8960.volume(tenthdbtolinear127(signed_value(value))/127.0); break;
  case NRPN_INPUT_LEVEL: wm8960.inputLevel(tenthdbtolinear(signed_value(value))); break;
  case NRPN_INPUT_SELECT: wm8960.inputSelect(value); break;
  default: Serial.printf("uncaught wm8960 codec nrpn #%d with value %d\n", nrpn, value); break;
  }
}

static void codec_listener(const int nrpn, int _, int value) {
  if (codec_sgtl5000) codec_sgtl5000_set(nrpn, value);
  if (codec_wm8960) codec_wm8960_set(nrpn, value);
}


static void codec_setup(void) {
  /* set nrpn listeners for our functions */
  nrpn_listen(NRPN_VOLUME, codec_listener);
  nrpn_listen(NRPN_INPUT_SELECT, codec_listener);
  nrpn_listen(NRPN_INPUT_LEVEL, codec_listener);
}

//static void codec_loop(void) {}
