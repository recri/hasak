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

/*
** adcmap endpoints for pot mapping.
*/
#define KYR_PADC_VOLUME_MAX -320 /* -32dB */
#define KYR_PADC_VOLUME_MIN 0	/*  0dB */
#define KYR_PADC_TONE_MAX 2500	/*  250.0Hz */
#define KYR_PADC_TONE_MIN 12500	/* 1250.0Hz */
#define KYR_PADC_SPEED_MAX 5	/* 5 wpm */
#define KYR_PADC_SPEED_MIN 55	/* 55 wpm */
#define KYR_PADC_FRAC_MAX 0	/* 0/128th */
#define KYR_PADC_FRAC_MIN 127	/* 127/128th */
#define KYR_PADC_BAL_MAX -8192
#define KYR_PADC_BAL_MIN 8191
#define KYR_PADC_TIME_MAX 0
#define KYR_PADC_TIME_MIN 16383
#define KYR_PADC_RATIO_MIN 25
#define KYR_PADC_RATIO_MAX 75

static void padcmap_value(int nrpn) {
  int min, max;
  int nrpn2 = nrpn_get(nrpn+1);
  switch (nrpn2) {
  case KYRP_NOTHING: return;
  case KYRP_VOLUME:
  case KYRP_LEVEL:
  case KYRP_IQ_LEVEL:
  case KYRP_I2S_LEVEL:
  case KYRP_HDW_LEVEL:
  case KYRP_CODEC_VOLUME:
    min = KYR_PADC_VOLUME_MIN; max = KYR_PADC_VOLUME_MAX; break;
  case KYRP_TONE:
    min = KYR_PADC_TONE_MIN; max = KYR_PADC_TONE_MAX; break;
  case KYRP_SPEED:
  case KYRP_FARNS:
    min = KYR_PADC_SPEED_MIN; max = KYR_PADC_SPEED_MAX; break;
  case KYRP_SPEED_FRAC:
    min = KYR_PADC_FRAC_MIN; max = KYR_PADC_FRAC_MAX; break;
  case KYRP_ST_BALANCE:
  case KYRP_IQ_BALANCE:
  case KYRP_COMP:
    min = KYR_PADC_BAL_MIN; max = KYR_PADC_BAL_MAX; break;
  case KYRP_HEAD_TIME:
  case KYRP_TAIL_TIME:
  case KYRP_RISE_TIME:
  case KYRP_FALL_TIME:
  case KYRP_MIXER_SLEW_TIME:
  case KYRP_FREQ_SLEW_TIME:
    min = KYR_PADC_TIME_MIN; max = KYR_PADC_TIME_MAX; break;
  case KYRP_WEIGHT:
  case KYRP_RATIO:
    min = KYR_PADC_RATIO_MIN; max = KYR_PADC_RATIO_MAX; break;
  default: return;
  }
  int p = nrpn_get(nrpn);		/* fetch adc reading */
  int n = ((1024-p)*min + p*max)>>10;	/* compute new value for nrpn2 */
  if (n == nrpn_get(nrpn2)) return;	/* value is unchanged */
  // Serial.printf("adc %d map %d -> %d\n", p, n, nrpn2);
  nrpn_set(nrpn2, n);			/* set the new value */
}
  
static void padcmap_setup(void) {
  for (int i = 0; i < KYR_N_PADC; i += 1)
    nrpn_listen(KYRP_PADC0_VAL+i*(KYRP_PADC1_VAL-KYRP_PADC0_VAL), padcmap_value);
}

// static void adcmap_loop(void) {}
