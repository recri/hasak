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
#ifndef _timing_h
#define _timing_h

#include "src/elapsed_counter.h"

/*
** Timing - keep track of 
**    the number of trips through the loop
**    the number of times AudioStream::update runs
**    the number of cpu cycles consumed by
**        the AudioStream::update
**        the hasak sample interrupt()
**        the loop()
**    so we can present percent cpu for all,
**    and normalize the cpu cycles per audio element, too.
**
** keep track of cycles during each sample buffer time
** used to normalize the cycle usage from the audio library
** also track the cycle usage in the pollatch interrupt routine.
**
** also provide various elapsedCounter classes
** in addition to the Teensy elapsedMillis and elapsedMicros
*/
static uint32_t timing_loopCounter = 0; /* count trips through the loop() function */
static uint32_t timing_audio_stream_cycles_total = 0; /* copy of AudioStream::cpu_cycles_total */
static uint32_t timing_updateCounter = 0;	      /* count AudioStream::update's */

static uint32_t timing_cpuCyclesRaw;
static uint32_t timing_cpuCyclesPerAudioBuffer;
static uint32_t timing_cpuCyclesPerAudioBufferMax;

static uint32_t timing_sampleCounter = 0;	      /* updated by sample interrupt */
static uint32_t timing_isrCpuCyclesRaw;
static uint32_t timing_isrCyclesPerAudioBuffer;
static uint32_t timing_isrCyclesPerAudioBufferMax;

static void timing_loop(void) {
  timing_loopCounter += 1;		/* one trip through the loop */
  
  /* 
  ** detect the bump in AudioStream::cpu_cycles_total
  ** that happens when AudioStream::update runs
  ** that bump never happens reliably.
  */
  if (au_st_key.update_counter-timing_updateCounter != 0) {
    timing_updateCounter = au_st_key.update_counter;		/* one buffer update completed */
    timing_audio_stream_cycles_total = AudioStream::cpu_cycles_total; /* reset to detect next update */

    /* cpu cycle count executed in loop() since last update */
    timing_cpuCyclesPerAudioBuffer = (ARM_DWT_CYCCNT - timing_cpuCyclesRaw) >> 6;
    timing_cpuCyclesRaw = ARM_DWT_CYCCNT; /* reset for next updatea */

    /* cpu cycle count in sample interrupt since last update */
    timing_isrCyclesPerAudioBuffer = timing_isrCpuCyclesRaw >> 6;
    timing_isrCpuCyclesRaw = 0;	/* reset for next update */

    /* track the high water marks */
    timing_cpuCyclesPerAudioBufferMax = max(timing_cpuCyclesPerAudioBufferMax, timing_cpuCyclesPerAudioBuffer);
    timing_isrCyclesPerAudioBufferMax = max(timing_isrCyclesPerAudioBuffer, timing_isrCyclesPerAudioBufferMax);

    /* this discards an early transient observed when the audio library initializes */
    if (timing_cpuCyclesPerAudioBufferMax > 100000) timing_cpuCyclesPerAudioBufferMax = timing_cpuCyclesPerAudioBuffer;
  }
}

static void timing_setup(void) {
}

static float timing_percent(uint32_t cpuCycles) {
  return 100*(float)cpuCycles/(float)timing_cpuCyclesPerAudioBuffer;
}

static float timing_percent_max(uint32_t cpuCyclesMax) {
  return 100*(float)cpuCyclesMax/(float)timing_cpuCyclesPerAudioBufferMax;
}

static unsigned long timing_loops() { return timing_loopCounter; }
static unsigned long timing_samples() { return timing_sampleCounter; }
static unsigned long timing_updates() { return timing_updateCounter; }

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