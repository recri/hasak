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

#ifndef effect_paddle_h_
#define effect_paddle_h_
#include "../../config.h"
#include "Arduino.h"
#include "../../linkage.h"
#include "AudioStream.h"

#include "keyer_ad5dz.h"
#include "keyer_k1el.h"
#include "keyer_nd7pa.h"
#include "keyer_vk6ph.h"

class AudioEffectPaddle : public AudioStream
{
public:
  AudioEffectPaddle(const int vox) : AudioStream(2, inputQueueArray), vox(vox),
			       ad5dz(vox), k1el(vox), nd7pa(vox), vk6ph(vox) {
    state = 0;
    keyer = &ad5dz;
  }
  virtual void update(void);
private:
  const int vox;
  uint8_t state;
  static const uint8_t adapt[3][2][2][2][2][3];
  audio_block_t *inputQueueArray[2];
  KeyerGeneric *keyer;
  KeyerAd5dz ad5dz;
  KeyerK1el k1el;
  KeyerNd7pa nd7pa;
  KeyerVk6ph vk6ph;
};

#endif
