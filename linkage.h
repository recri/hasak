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

#ifdef __cplusplus
extern "C" {
#endif

  /* forward references */
int get_note(int note);		// externally available note_get()
int get_ctrl(int ctrl);		// externally available ctrl_get()
int get_nrpn(int nrpn);		// externally available nrpn_get()
int get_xnrpn(int xnrpn);	// externally available xnrpn_get()

/* unit conversions */
static inline int32_t ms_to_samples(const int16_t ms) { return ms * (AUDIO_SAMPLE_RATE*0.001); }
static inline float samples_to_ms(const int32_t samples) { return samples / (AUDIO_SAMPLE_RATE*0.001); }
static inline int tenth_ms_to_samples(const int16_t tenthms) { return tenthms * (AUDIO_SAMPLE_RATE*0.0001); }
static inline float samples_to_tenth_ms(const int32_t samples) { return samples / (AUDIO_SAMPLE_RATE*0.0001); }
static inline float tenth_ms_to_ms(const int16_t tenthms) { return tenthms*0.1f; }
static inline float int_to_127ths(const int16_t val) { return val*0.007874f; }
static inline int16_t signed_value(const int16_t val) { return ((int16_t)(val<<2))>>2; }
static inline int32_t signed_xvalue(const int32_t val) { return ((int32_t)(val<<4))>>4; }
static inline float tenthdbtolinear(const int16_t val) { return min(2,powf(10.0f, val/(10.0*20.0))); } // tenthdb == dB/10
static inline float qtrdbtolinear(const int16_t val) { return min(2,powf(10.0f, val/(4.0*20.0))); } // qtrdb == db/4

/* */
static const uint8_t qtrdbtolinearasbyte[] = {
  127, 123, 119, 116, 113, 109, 106, 103, 100, 98, 95, 92, 89, 87, 84, 82, 80, 77, 75, 73, 71, 69, 67, 65, 63, 61, 60, 58, 56, 55, 53, 52, 50, 49,
  47, 46, 45, 43, 42, 41, 40, 39, 37, 36, 35, 34, 33, 32, 31, 30, 30, 29, 28, 27, 26, 26, 25, 24, 23, 23, 22, 21, 21, 20, 20, 19, 19, 18, 17, 17,
  16, 16, 15, 15, 15, 14, 14, 13, 13, 13, 12, 12, 11, 11, 11, 10, 10, 10, 10, 9, 9, 9, 8, 8, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5,
  5, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

static inline uint8_t qtrdbtolinear127(const int16_t val) {
  if (val >= 0) return 127;
  if (-val >= (int16_t)sizeof(qtrdbtolinearasbyte)) return 0;
  return qtrdbtolinearasbyte[-val];
}

/* */
static const uint8_t tenthdbtolinearasbyte[] = {
  127, 125, 124, 122, 121, 119, 118, 117, 115, 114, 113, 111, 110, 109, 108, 106, 105, 104, 103, 102, 100, 99, 98, 97, 96, 95, 94, 93, 92, 90, 89, 
  88, 87, 86, 85, 84, 83, 82, 81, 81, 80, 79, 78, 77, 76, 75, 74, 73, 73, 72, 71, 70, 69, 68, 68, 67, 66, 65, 65, 64, 63, 62, 62, 61, 60, 60, 59, 
  58, 58, 57, 56, 56, 55, 54, 54, 53, 52, 52, 51, 51, 50, 49, 49, 48, 48, 47, 47, 46, 46, 45, 45, 44, 44, 43, 43, 42, 42, 41, 41, 40, 40, 39, 39, 
  38, 38, 37, 37, 37, 36, 36, 35, 35, 34, 34, 34, 33, 33, 33, 32, 32, 31, 31, 31, 30, 30, 30, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 26, 25, 
  25, 25, 25, 24, 24, 24, 23, 23, 23, 23, 22, 22, 22, 22, 21, 21, 21, 21, 20, 20, 20, 20, 19, 19, 19, 19, 19, 18, 18, 18, 18, 17, 17, 17, 17, 17, 
  16, 16, 16, 16, 16, 15, 15, 15, 15, 15, 15, 14, 14, 14, 14, 14, 14, 13, 13, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 12, 11, 11, 11, 11, 11, 11,
  11, 11, 10, 10, 10, 10, 10, 10, 10, 10, 9, 9, 9, 9, 9, 9, 9, 9, 9, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 6, 6, 6, 6,
  6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0
};

static inline uint8_t tenthdbtolinear127(const int16_t val) {
  if (val >= 0) return 127;
  if (-val >= (int16_t)sizeof(tenthdbtolinearasbyte)) return 0;
  return tenthdbtolinearasbyte[-val];
}

static inline float nrpn_to_db(int16_t v) { return signed_value(v)/4.0; }

static inline float nrpn_to_hertz(int16_t v) { return v/10.0; }

#ifdef __cplusplus
}
#endif

#endif
