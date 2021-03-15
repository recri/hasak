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
  int pad_mode(void) { return get_vox_pad_mode(vox); }
  int dit(void) { return get_vox_dit(vox); }
  int dah(void) { return get_vox_dah(vox); }
  int ies(void) { return get_vox_ies(vox); }
  int ils(void) { return get_vox_ils(vox); }
  int iws(void) { return get_vox_iws(vox); }
  int auto_ils(void) { return get_vox_auto_ils(vox); }
  int auto_iws(void) { return get_vox_auto_iws(vox); }
  
 private:
  const int vox;
};

#endif
