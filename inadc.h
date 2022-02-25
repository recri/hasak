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

static void inadc_update(int i, int pin, int nrpn, int min, int max) {
  static ResponsiveAnalogRead *inadc_adc[KYR_N_INADC];
  static int16_t inadc_value[KYR_N_INADC];
  if ( ! pin_valid(pin)) return;		/* can't read a bad pin */
  if (nrpn == KYRP_NOTHING) return;		/* can't store to nowhere */
  if (inadc_adc[i] == NULL) {			/* make sure our reader is setup */
    inadc_adc[i] = new ResponsiveAnalogRead(pin, true, 0.001f);
    inadc_adc[i]->setActivityThreshold(8.0f);
    inadc_adc[i]->enableEdgeSnap();
  }
  inadc_adc[i]->update();			/* update the reader */
  int16_t raw_value = inadc_adc[i]->getValue();	/* get the updated value */
  if (raw_value == inadc_value[i]) return;	/* the raw value has not changed */
  inadc_value[i] = raw_value;			/* save the new value */
  int16_t cooked_value = ((1024-raw_value)*min  /* interpolate new cooked value */
			  + raw_value*max)>>10; 
  int16_t current_value = nrpn_get(nrpn);	/* old cooked value */
  if (cooked_value == current_value) return;	/* the cooked value hasn't changed */
  nrpn_set(nrpn, cooked_value);			/* set the new value */
}

static void inadc_milli(void) {
  static uint8_t count = 0;
  if ( ! nrpn_get(KYRP_ADC_ENABLE)) return;
  if ((count++ % 4) != 0) return;
  /* reading every 4 milliseconds, so all 8 pins are read every 32 milliseconds */
  switch ((count >> 2) & 7) {
  case 0: inadc_update(0, nrpn_get(KYRP_INADC0_PIN), nrpn_get(KYRP_INADC0_NRPN), nrpn_get(KYRP_INADC0_MIN), nrpn_get(KYRP_INADC0_MAX)); break;
  case 1: inadc_update(1, nrpn_get(KYRP_INADC1_PIN), nrpn_get(KYRP_INADC1_NRPN), nrpn_get(KYRP_INADC1_MIN), nrpn_get(KYRP_INADC1_MAX)); break;
  case 2: inadc_update(2, nrpn_get(KYRP_INADC2_PIN), nrpn_get(KYRP_INADC2_NRPN), nrpn_get(KYRP_INADC2_MIN), nrpn_get(KYRP_INADC2_MAX)); break;
  case 3: inadc_update(3, nrpn_get(KYRP_INADC3_PIN), nrpn_get(KYRP_INADC3_NRPN), nrpn_get(KYRP_INADC3_MIN), nrpn_get(KYRP_INADC3_MAX)); break;
  case 4: inadc_update(4, nrpn_get(KYRP_INADC4_PIN), nrpn_get(KYRP_INADC4_NRPN), nrpn_get(KYRP_INADC4_MIN), nrpn_get(KYRP_INADC4_MAX)); break;
  case 5: inadc_update(5, nrpn_get(KYRP_INADC5_PIN), nrpn_get(KYRP_INADC5_NRPN), nrpn_get(KYRP_INADC5_MIN), nrpn_get(KYRP_INADC5_MAX)); break;
  case 6: inadc_update(6, nrpn_get(KYRP_INADC6_PIN), nrpn_get(KYRP_INADC6_NRPN), nrpn_get(KYRP_INADC6_MIN), nrpn_get(KYRP_INADC6_MAX)); break;
  case 7: inadc_update(7, nrpn_get(KYRP_INADC7_PIN), nrpn_get(KYRP_INADC7_NRPN), nrpn_get(KYRP_INADC7_MIN), nrpn_get(KYRP_INADC7_MAX)); break;
  default: break;
  }
}

static void inadc_setup(void) {
  every_milli(inadc_milli);
}

static void inadc_loop(void) {
}
