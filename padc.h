/* -*- mode: c++; tab-width: 8 -*- */
/*
 * hasak (ham and swiss army knife) keyer for Teensy 4.X, 3.X
 * Copyright (c) 2021,2022 by Roger Critchlow, Charlestown, MA, USA
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

#include <ResponsiveAnalogRead.h>

static ResponsiveAnalogRead *padc_adc[KYR_N_PADC];

static void padc_milli(int nrpn, int _) {
  if (nrpn_get(NRPN_PADC_ENABLE)) {
    static uint16_t count = 0;
    count += 1;
    for (int i = 0; i < KYR_N_PADC; i += 1) {
      if (padc_adc[i] != NULL) {
	if ((count % KYR_N_PADC) == i) 
	  padc_adc[i]->update();
	if ((count % nrpn_get(NRPN_PADC_RATE)) == i) {
	  int16_t raw_value = padc_adc[i]->getValue();
	  int val_nrpn = NRPN_PADC0_VAL+i*(NRPN_PADC1_VAL-NRPN_PADC0_VAL);
	  if (raw_value != nrpn_get(val_nrpn)) {
	    nrpn_set(val_nrpn, raw_value);
	  }
	}
      }
    }
  }
}

static void padc_pin_listener(int nrpn, int _) {
  const int pin = nrpn_get(nrpn);
  if ( ! pin_valid(pin) || ! pin_analog(pin) || pin_i2s(pin) || pin_i2c(pin)) {
    if (pin != 127)
      nrpn_set(nrpn, 127); // this should trigger a loop detection in HasakMidi
  } else {
    // Serial.printf("padc_pin_listener(%d)\n", nrpn);
    const int i = (nrpn-NRPN_PADC0_PIN)/(NRPN_PADC1_PIN-NRPN_PADC0_PIN);
    if (padc_adc[i] != NULL) {
      delete padc_adc[i];
      padc_adc[i] = NULL;
    }
    padc_adc[i] = new ResponsiveAnalogRead(pin, true, 0.001f);
    padc_adc[i]->setActivityThreshold(8.0f);
    padc_adc[i]->enableEdgeSnap();
    padc_adc[i]->update();
  }
}

static void padc_setup(void) {
  for (int i = 0; i < KYR_N_PADC; i += 1)
    nrpn_listen(NRPN_PADC0_PIN+i*(NRPN_PADC1_PIN-NRPN_PADC0_PIN), padc_pin_listener);
  nrpn_listen(NRPN_MILLI, padc_milli);
}
