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

static int adc_get(int i, int pin) {
  static ResponsiveAnalogRead *adc[KYR_N_ADC];
  if (adc[i] == NULL) {			/* make sure our reader is setup */
    adc[i] = new ResponsiveAnalogRead(pin, true, 0.001f);
    adc[i]->setActivityThreshold(8.0f);
    adc[i]->enableEdgeSnap();
  }
  adc[i]->update();			/* update the reader */
  return adc[i]->getValue();	/* return the updated value */
}

static void adc_update(int i, int pin, int nrpn) {
  if ( ! pin_valid(pin)) return;		/* can't read a bad pin */
  int16_t raw_value = adc_get(i, pin);		/* get the updated value */
  if (raw_value == nrpn_get(nrpn)) return;	/* the raw value has not changed */
  nrpn_set(nrpn, raw_value);			/* save the new value */
}

static void adc_milli(void) {
  static uint16_t count = 0;
  if ( ! nrpn_get(KYRP_ADC_ENABLE)) return;
  count += 1;
  if ((count % nrpn_get(KYRP_ADC_RATE)) != 0) return; /* FIX.ME - is this what ADC_RATE means? */
  switch ((count/nrpn_get(KYRP_ADC_RATE)) % 8) {
  case 0: adc_update(0, nrpn_get(KYRP_ADC0_PIN), KYRP_ADC0_VAL); break;
  case 1: adc_update(1, nrpn_get(KYRP_ADC1_PIN), KYRP_ADC1_VAL); break;
  case 2: adc_update(2, nrpn_get(KYRP_ADC2_PIN), KYRP_ADC2_VAL); break;
  case 3: adc_update(3, nrpn_get(KYRP_ADC3_PIN), KYRP_ADC3_VAL); break;
  case 4: adc_update(4, nrpn_get(KYRP_ADC4_PIN), KYRP_ADC4_VAL); break;
  case 5: adc_update(5, nrpn_get(KYRP_ADC5_PIN), KYRP_ADC5_VAL); break;
  case 6: adc_update(6, nrpn_get(KYRP_ADC6_PIN), KYRP_ADC6_VAL); break;
  case 7: adc_update(7, nrpn_get(KYRP_ADC7_PIN), KYRP_ADC7_VAL); break;
  default: break;
  }
}

static void adc_setup(void) {
  every_milli(adc_milli);
}

static void adc_loop(void) {
}
