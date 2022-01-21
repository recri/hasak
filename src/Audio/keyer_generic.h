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
#ifndef keyer_generic_h_
#define keyer_generic_h_

class KeyerGeneric {
 public:
 KeyerGeneric(const int vox) : vox(vox) { }
  void init(void) { }
  virtual int clock(int dit, int dah, int ticks) { return 0; }
  int pad_mode(void) { return get_nrpn(KYRP_PAD_MODE); }
  int dit(void) { return ms_to_samples(get_vox_nrpn(vox, KYRP_PER_DIT)); }
  int dah(void) { return ms_to_samples(get_vox_nrpn(vox, KYRP_PER_DAH)); }
  int ies(void) { return ms_to_samples(get_vox_nrpn(vox, KYRP_PER_IES)); }
  int ils(void) { return ms_to_samples(get_vox_nrpn(vox, KYRP_PER_ILS)); }
  int iws(void) { return ms_to_samples(get_vox_nrpn(vox, KYRP_PER_IWS)); }
  int auto_ils(void) { return get_nrpn(KYRP_AUTO_ILS); }
  int auto_iws(void) { return get_nrpn(KYRP_AUTO_IWS); }
  
 private:
  const int vox;
};

#endif
