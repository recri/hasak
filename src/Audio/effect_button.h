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
/*
** Translate adc readings from the headset button
** into button presses.
*/
#ifndef effect_button_h_
#define effect_button_h_

#include "../../config.h"
#include <Arduino.h>
#include <AudioStream.h>
#include "sample_value.h"
#include "../../linkage.h"

class AudioEffectButton : public AudioStream
{
public:
  AudioEffectButton() : AudioStream(1, inputQueueArray) {
    initial_skip = 24000/AUDIO_BLOCK_SAMPLES;
    last_best[0] = last_best[1] = 0;
  }
  virtual void update(void);
  static const int N_BUTTONS = 5;
  int initial_skip;
  uint8_t last_best[2];
private:
  audio_block_t *inputQueueArray[1];
};

#endif
