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
#ifndef _timers_h
#define _timers_h

#include "src/elapsed_counter.h"

extern unsigned long timing_loops();
extern unsigned long timing_samples();
extern unsigned long timing_updates();

/*
** These types, like the Teensy supplied elapsedMicros and elapsedMillis, 
** allow you to easily count elapsed time periods.
** First off, llocate the instance in storage that will persist, so external
** or static.
** Then set the instance to zero, and then it will always read the number of
** units that have elapsed since you set it to zero.
** You can also initialize to a negative number and read it with an (int) cast,
** it will count up from the negative value to zero and then continue counting.
*/

typedef elapsed<micros> myElaspedMicros; // microseconds, for comparison to Teensy elapsedMicros
typedef elapsed<millis> myElapsedMillis; // milliseconds, for comparison to Teensy elapsedMillis
typedef elapsed<timing_samples> elapsedSamples; // sample times
typedef elapsed<timing_loops> elapsedLoops; // executions of arduino loop()
typedef elapsed<timing_updates> elapsedUpdates; // audio library buffer cycles

#endif
