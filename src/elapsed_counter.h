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

#ifndef _elapsed_counter_h_
#define _elapsed_counter_h_

/* Elapsed time types - for easy-to-use measurements of elapsed time
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
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
** a template class to generalize elapsedMillis and elapsedMicros to other 
** monotonically increasing counters.
** shamelessly ripped off from Paul Stoffregen's elapsedMillis and elapsedMicros classes
*/

template <unsigned long counter()>
class elapsed {
private:
  unsigned long t;
public:
  elapsed<counter>(void) { t = counter(); }
  elapsed<counter>(unsigned long val) { t = counter() - val; }
  elapsed<counter>(const elapsed<counter> &orig) { t = orig.t; }
  operator unsigned long () const { return counter() - t; }
  elapsed<counter> & operator = (const elapsed<counter> &rhs) { t = rhs.t; return *this; }
  elapsed<counter> & operator = (unsigned long val) { t = counter() - val; return *this; }
  elapsed<counter> & operator -= (unsigned long val)      { t += val ; return *this; }
  elapsed<counter> & operator += (unsigned long val)      { t -= val ; return *this; }
  elapsed<counter> operator - (int val) const           { elapsed<counter> r(*this); r.t += val; return r; }
  elapsed<counter> operator - (unsigned int val) const  { elapsed<counter> r(*this); r.t += val; return r; }
  elapsed<counter> operator - (long val) const          { elapsed<counter> r(*this); r.t += val; return r; }
  elapsed<counter> operator - (unsigned long val) const { elapsed<counter> r(*this); r.t += val; return r; }
  elapsed<counter> operator + (int val) const           { elapsed<counter> r(*this); r.t -= val; return r; }
  elapsed<counter> operator + (unsigned int val) const  { elapsed<counter> r(*this); r.t -= val; return r; }
  elapsed<counter> operator + (long val) const          { elapsed<counter> r(*this); r.t -= val; return r; }
  elapsed<counter> operator + (unsigned long val) const { elapsed<counter> r(*this); r.t -= val; return r; }
};

#endif
