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
#ifndef output_byte_h_
#define output_byte_h_
#include "Arduino.h"
#include "AudioStream.h"
#include "sample_value.h"

/*
** Teensy Audio Library component for accessing output samples
*/

class AudioOutputByte : public AudioStream
{
public:
  AudioOutputByte() : AudioStream(1, inputQueueArray) { 
    rptr = wptr = 0;
  }
  uint8_t recv(void) {
    uint8_t value = buffer[rptr++];
    rptr %= 2*AUDIO_BLOCK_SAMPLES;
    return value;
  }
  virtual void update(void);
private:
  uint16_t wptr, rptr;
  uint8_t buffer[2*AUDIO_BLOCK_SAMPLES];
  audio_block_t *inputQueueArray[1];
};

#endif
