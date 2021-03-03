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
** hasak text keyer component.
**
** I've adopted the compact code representation and the winkey convention
** of a prefix operator to form prosigns by concatenation, but I'm feeling
** a little queasy about corner cases.
**
** The morse table could be expanded to include 128 or 256 slots with 14 bits
** of encoding set via MIDI.
*/

#ifndef input_text_h_
#define input_text_h_
#include "../../config.h"
#include "Arduino.h"
#include "../../linkage.h"
#include "AudioStream.h"

extern "C" {
  extern unsigned char morse[58];
}

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
    prosign = 0;
    code = 0;
    ewptr = erptr = 0;
  }

private:
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

public:
  // text buffer
  // valid_text identifies acceptable characters
  int valid_text(uint8_t value) {
    // convert to upper case
    return (value >='a' && value <= 'z') ||
      value == ' ' || value == '\t' || value == '\n' ||
      value == '\e' ||
      (value-33 >= 0 && value-33 < 58 && morse[value-33] != 1);
  }
  // send_text queues a character for sending
  // return 0 if the value was successfully queued
  // return -1 if there is no room or the character
  // was rejected by valid_text
  int send_text(uint8_t value) {
    // reject false characters
    if ( ! valid_text(value)) return -1;
    // convert to upper case
    if (value >='a' && value <= 'z') value -= 32;
    // map spaces to space
    if (value == ' ' || value == '\t' || value == '\n') value = ' ';
    if (can_send_text()) {
      buffer[bwptr++] = value;
      bwptr %= BUFFER_SIZE;
      return 0;
    }
    return -1;
  }
  // return true if there is room to queue a character
  int can_send_text() { return (((bwptr+1) % BUFFER_SIZE) != brptr); }
private:
  int recv_text(void) {
    if (can_recv_text()) {
      uint8_t value = buffer[brptr++];
      erptr %= BUFFER_SIZE;
      return value;
    }
    return -1;
  }
  int peek_text(void) {
    if (can_recv_text())
      return buffer[brptr];
    return -1;
  }
  int can_recv_text(void) { return brptr != bwptr; }

  // pull characters from the text buffer
  // translate into morse elements
  // and push them into the element buffer
  // return 1 if a character was translated
  // return 0 if no character was available
  int get_elements() {
    if (code > 1) {
      send(code & 1 ? get_vox_dah(vox) : get_vox_dit(vox));
      send(get_vox_ies(vox));
      code >>= 1;
      return 1;
    }
    if (code == 1) {
      // end of code decoding
      if ( ! prosign) {
	send(get_vox_ils(vox)-get_vox_ies(vox));
	code = 0;
	return 1;
      }
      prosign = 0;
      code = 0;
    }
    if ( ! can_recv_text())
      return 0;
    uint8_t value = recv_text();
    if (value == ' ') {
      // send word space
      send(get_vox_iws(vox));
      // clear the space from the input queue
      return 1;
    }
    if (value == '\e') {
      // prosign together the next two characters
      prosign = 1;
      // we've run dry
      if ( ! can_recv_text()) return 0;
      // get the next char
      value = recv_text();
    }
    code = morse[value-33];
    send(code & 1 ? get_vox_dah(vox) : get_vox_dit(vox));
    send(get_vox_ies(vox));
    code >>= 1;
    return 1;
  }
public:
  void abort() {
    bwptr = brptr = 0;		// clear text buffer
    code = 0;			// clear the code
    ewptr = erptr = 0;		// clear element buffer
    element = 0;		// clear the element
  }
  virtual void update(void);
private:
  static const int ELEMENT_SIZE = 8;
  static const int BUFFER_SIZE = 64;
  const int vox;
  int element;
  uint8_t prosign, code;
  audio_block_t *block;
  int16_t *wptr, n;
  int16_t elements[ELEMENT_SIZE];
  uint8_t ewptr, erptr;
  uint8_t buffer[BUFFER_SIZE];
  uint8_t bwptr, brptr;
};

#endif
