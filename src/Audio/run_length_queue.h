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
#ifndef run_length_queue_h_
#define run_length_queue_h_

#include <Arduino.h>
#include "ring_buffer.h"

/* this should be reused in input_text */
/*
** circular buffer for run lengths of key and ptt events
** positive runs indicate sequences of 1's
** negative runs indicate sequences of 0's
** a zero run indicates empty
*/
class RunLengthQueue : public RingBuffer {
public:
  RunLengthQueue(void) : RingBuffer() {
    in = out = 0;
    overrun = underrun = maxusage = 0;
  }

  /* test the waters */
  int can_put_run(void) { return items()<SIZE; }
    
  /* append the run onto the queue */
  int put_run(int run) { 
    if (in == 0) {
      in = run;
    } else if ((in < 0) == (run < 0)) {
      in += run;
    } else {
      while (can_put())
	if (in > 0x7fff) {
	  put(0x7fff);
	  in -= 0x7fff;
	} else if (in < -0x7fff) {
	  put(-0x7fff);
	  in += 0x7fff;
	} else {
	  put(in);
	  in -= in;
	  break;
	}
      if (in != 0) {
	overrun += 1;
	return -1;
      }
      in = run;
    }
    maxusage = max(maxusage, items());
    return 0;
  }

  /* return the next bit in the run, -1 if none */
  int get_bit(void) {
    if (out == 0) {
      if (can_get())
	out = get();
      else if (in != 0) {
	out = in;
	in = 0;
      } else {
	underrun += 1;
	return -1;
      }
    }
    if (out < 0) { 
      out += 1; 
      return 0;
    }
    out -= 1;
    return 1;
  }

  bool emptyp(void) { 
    return ! (out != 0 || can_get());
  }
  
  void reset(void) {
    RingBuffer::reset();
    in = out = 0;
  }

  int overrun, underrun, maxusage;

private:
  int in, out;
};
#endif
