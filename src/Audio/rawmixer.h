/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
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

#ifndef rawmixer_h_
#define rawmixer_h_

#include "Arduino.h"
#include "AudioStream.h"

// this is a raw mixer that joins four sample streams by addition
// the streams should be preattenuated accordingly
class RawAudioMixer4 : public AudioStream
{
public:
	RawAudioMixer4(void) : AudioStream(4, inputQueueArray) {
	}
	virtual void update(void);
private:
	audio_block_t *inputQueueArray[4];

};

class RawAudioAmplifier : public AudioStream
{
public:
	RawAudioAmplifier(void) : AudioStream(2, inputQueueArray) {
	}
	virtual void update(void);
        int32_t get_multiplier(void) { return multiplier; }
private:
	int32_t multiplier;
	audio_block_t *inputQueueArray[2];
};

#endif
