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
#ifndef keyer_paddle_generic_h_
#define keyer_paddle_generic_h_

#include "Arduino.h"
#include "../config.h"
#include "../linkage.h"

#include "keyer_timing_generic.h"

class KeyerPaddleGeneric : public KeyerTimingGeneric {
 public:
  KeyerPaddleGeneric() : KeyerTimingGeneric() { }
  // void init(void) { }
  virtual int clock(int dit, int dah, int ticks) { return 0; }
  int swap(void) { return nrpn_get(NRPN_PAD_SWAP); }
  int pad_mode(void) { return nrpn_get(NRPN_PAD_MODE); }
  int adapt(void) { return nrpn_get(NRPN_PAD_ADAPT); }
  int auto_ils(void) { return nrpn_get(NRPN_AUTO_ILS); }
  int auto_iws(void) { return nrpn_get(NRPN_AUTO_IWS); }
};

#endif
