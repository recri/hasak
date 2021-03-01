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

#ifndef input_text_h_
#define input_text_h_
#include "Arduino.h"
#include "AudioStream.h"

/*
** Teensy Audio Library component for morse code text input
*/
class AudioInputText : public AudioStream
{
public:
  AudioInputText(int vox) : AudioStream(0, NULL), vox(vox) {
    block = allocate();
    if (block) {
      wptr = &block->data[0];
      n = AUDIO_BLOCK_SAMPLES;
    }
    element = 0;
    ewptr = erptr = 0;
  }

  // element buffer
  // send abs(value) samples of (value < 0) ? 0 : 1
  // return 0 if the value was successfully queued
  // return -1 if there is no room
  int send(int16_t value) {
    if (can_send()) {
      elements[ewptr] = value;
      ewptr = (ewptr+1)%ELEMENT_SIZE;
      return 0;
    }
    return -1;
  }
  int can_send() { return (((ewptr+1) % ELEMENT_SIZE) != erptr); }
  int recv(void) {
    if (can_recv()) {
      int16_t value = elements[erptr];
      erptr = (erptr+1)%ELEMENT_SIZE;
      return value;
    }
    return 0;
  }
  int can_recv(void) { return erptr != ewptr || get_elements(); }

  // text buffer
  // send the character value
  // return 0 if the value was successfully queued
  // return -1 if there is no room
  int send_text(uint8_t value) {
    if (can_send_text()) {
      buffer[bwptr++] = value;
      bwptr %= BUFFER_SIZE;
      return 0;
    }
    return -1;
  }
  int can_send_text() { return (((bwptr+1) % BUFFER_SIZE) != brptr); }
  int recv_text(void) {
    if (can_recv_text()) {
      uint8_t value = buffer[brptr++];
      erptr %= BUFFER_SIZE;
      return value;
    }
    return -1;
  }
  int peek_text(void) {
    if (can_recv_text()) {
      uint8_t value = buffer[brptr++];
      erptr %= BUFFER_SIZE;
      return buffer[brptr];
    }
    return -1;
  }
  int can_recv_text(void) { return brptr != bwptr; }

  // pull characters from the text buffer
  // translate into morse elements
  // and push them into the element buffer
  // return 1 if a character was translated
  // return 0 if no character was available
  int get_elements() {
    if (can_recv_text()) {
      uint8_t value = peek_text();
      uint8_t prosign = 0;
      // fetch the timing parameters
      // ...
      if (value == ' ') {
	// send word space
	// ...
	// clear the space from the input queue
	(void)recv_text();
	// 
	return 1;
      }
      if (value == '\e' || value == '\\') {
	// prosign together the next two characters
	prosign += 1;
	// clear the escape from the input queue
	(void)recv_text();
	// peek out the next char
	value = peek_text();
      }
      if (value >='a' && value <= 'z') {
	// convert to upper case
	value -= 32;
      }
      if (value > 32 && value < 'Z') {
	uint8_t code = morse[value-33];
	while (code != 1) {
	  if (code & 1) {
	    // send dah
	    // ...
	  } else {
	    // send dit
	    // ...
	  }
	  // send element space
	  // ...
	  // shift to the next element
	  code >>= 1;
	}
	if (prosign) {
	  // skip letter space
	  // reduce level of prosign
	  prosign -= 1;
	} else {
	  // send letter space
	}
      } else {
	// someone sent us a bad letter
      }
    }
  }
  void abort() {
    ewptr = erptr = bwptr = brptr = 0;
  }
  virtual void update(void);
private:
  static const int ELEMENT_SIZE = 32;
  static const int BUFFER_SIZE = 64;
  static const unsigned char morse[58];
  const int vox;
  int element;
  audio_block_t *block;
  int16_t *wptr, n, last;
  int16_t elements[ELEMENT_SIZE];
  uint8_t ewptr, erptr;
  uint8_t buffer[BUFFER_SIZE];
  uint8_t bwptr, brptr;
};

#endif
