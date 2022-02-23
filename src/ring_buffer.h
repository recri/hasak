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
#ifndef ring_buffer_h_
#define ring_buffer_h_

#include <Arduino.h>

#ifndef RING_BUFFER_SIZE
#define RING_BUFFER_SIZE 128
#endif

/* circular buffer of SIZE elements of type T */
template <class T, unsigned SIZE>
class RingBuffer {
public:
  RingBuffer() { }
  void reset(void) { wptr = rptr = 0; }
  int items(void) { return wptr-rptr; }
  bool can_get(void) { return rptr!=wptr; }
  T peek(void) { return buff[rptr%SIZE]; }
  T get(void) { return buff[rptr++%SIZE]; }
  bool can_put(void) { return (wptr+1) != rptr; }
  void put(T val) { buff[wptr++%SIZE] = val; }
  bool can_unput(void) { return can_get(); }
  void unput(void) { wptr -= 1; }
private:
  unsigned wptr = 0, rptr = 0;
  T buff[SIZE];
};
#endif
