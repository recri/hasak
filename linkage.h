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
#ifndef linkage_h_
#define linkage_h_

int16_t get_active_vox(void);

typedef struct {
  /* block stored to eeprom */
  /* three word header */
  uint16_t header[3];
  /* nrpn values */
  int16_t nrpn[KYRP_LAST-KYRP_FIRST];
  /* saved messages */
  int8_t msgs[EEPROM_BYTES-(3+(KYRP_LAST-KYRP_FIRST)+3)*sizeof(int16_t)];
  /* additional non-persistent storage */
  int32_t xnrpn[KYRP_XLAST-KYRP_XFIRST];
  /* index for reading and writing message bytes */
  uint16_t index;
}  hasak_t;

extern hasak_t hasak;

static int16_t invalid_get_nrpn(const int16_t nrpn) {
  Serial.printf("invalid get_nrpn(%d)\n", nrpn);
  return -1;
}
/* fetch a nrpn */
static inline int16_t get_nrpn(const int16_t nrpn) { 
  return (unsigned)nrpn < (KYRP_LAST-KYRP_FIRST) ? hasak.nrpn[nrpn] : invalid_get_nrpn(nrpn);
}

static int32_t invalid_get_xnrpn(const int16_t nrpn) {
  Serial.printf("invalid get_xnrpn(%d)\n", nrpn);
  return -1;
}
/* fetch an xnrpn */  
static inline int32_t get_xnrpn(const int16_t nrpn) {
  return (unsigned)(nrpn-KYRP_XFIRST) < (KYRP_XLAST-KYRP_XFIRST) ? hasak.xnrpn[nrpn-KYRP_XFIRST] : invalid_get_xnrpn(nrpn);
}

/* fetch a vox specialized nrpn */
static inline int16_t get_vox_nrpn(const int16_t vox, const int16_t nrpn) {
  const int16_t value = get_nrpn(KYRP_KEYER+vox*KYRP_VOX_OFFSET+(nrpn-KYRP_KEYER));
  return (value >= 0) ? value : get_nrpn(nrpn);
}

/* fetch a vox specialized xnrpn */
static inline int32_t get_vox_xnrpn(const int16_t vox, const int16_t nrpn) {
  const int32_t value = get_xnrpn(KYRP_XKEYER+vox*KYRP_XVOX_OFFSET+(nrpn-KYRP_XKEYER));
  return (value >= 0) ? value : get_xnrpn(nrpn);
}

/* unit conversions */
static inline int32_t ms_to_samples(const int16_t ms) { return ms * (AUDIO_SAMPLE_RATE*0.001); }
static inline float samples_to_ms(const int32_t samples) { return samples / (AUDIO_SAMPLE_RATE*0.001); }
static inline int tenth_ms_to_samples(const int16_t tenthms) { return tenthms * (AUDIO_SAMPLE_RATE*0.0001); }
static inline float samples_to_tenth_ms(const int32_t samples) { return samples / (AUDIO_SAMPLE_RATE*0.0001); }
static inline float tenth_ms_to_ms(const int16_t tenthms) { return tenthms*0.1f; }
static inline float int_to_127ths(const int16_t val) { return val*0.007874f; }
static inline int16_t signed_value(const int16_t val) { return (val<<2)>>2; }

#endif
