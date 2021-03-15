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

#include "ring_buffer.h"

RingBuffer r;

void setup(void) {
  Serial.begin(115200);
  while ( ! Serial);
  Serial.printf("Testing ring buffer.\n");

  // ring is empty
  if (r.items() != 0) Serial.printf("! items() != 0\n");
  // ring can be put to
  if ( ! r.can_put()) Serial.printf("! cannot can_put() into empty\n");
  // ring cannot be get from
  if (r.can_get()) Serial.printf("! can_get() from empty\n");

  // fill with positive
  Serial.printf("Fill with positive.\n");
  for (int i = 1; r.can_put(); i += 1) {
    r.put(i);
    if (r.items() != i) Serial.printf("! items() != %d\n", i);
  }
  for (int i = 1; r.can_get(); i += 1) {
    int x = r.get();
    if (x != i) Serial.printf("! r.get() == %d != %d\n", x, i);
  }
  // ring is empty
  if (r.items() != 0) Serial.printf("! items() != 0\n");
  // ring can be put to
  if ( ! r.can_put()) Serial.printf("! cannot can_put() into empty\n");
  // ring cannot be get from
  if (r.can_get()) Serial.printf("! can_get() from empty\n");

  // Fill with negative
  Serial.printf("Fill with negative.\n");
  for (int i = -1; r.can_put(); i -= 1) {
    r.put(i);
    if (r.items() != -i) Serial.printf("! items() != -%d\n", i);
  }
  for (int i = -1; r.can_get(); i -= 1) {
    int x = r.get();
    if (x != i) Serial.printf("! r.get() == %d != %d\n", x, i);
  }
  // ring is empty
  if (r.items() != 0) Serial.printf("! items() != 0\n");
  // ring can be put to
  if ( ! r.can_put()) Serial.printf("! cannot can_put() into empty\n");
  // ring cannot be get from
  if (r.can_get()) Serial.printf("! can_get() from empty\n");

  Serial.printf("Testing complete.\n");
}

void loop(void) {
}
