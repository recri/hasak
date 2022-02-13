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
** The morse table has moved into the NRPN array, so it can be augmented as
** required.  It covers the 64 characters from ! to `, though many are undefined
** by default.
*/

#ifndef input_text_h_
#define input_text_h_
#include "../../config.h"
#include "Arduino.h"
#include "../../linkage.h"
#include "AudioStream.h"
#include "ring_buffer.h"

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
  }

private:
  // element buffer
  // send abs(value) samples of (value < 0) ? 0 : 1
  // return 0 if the value was successfully queued
  // return -1 if there is no room
  int send(int32_t value) {
    if (elements.can_put()) {
      elements.put(value);
      return 0;
    }
    return -1;
  }
  int recv(void) { return can_recv() ? elements.get() :  0; }
  int can_recv(void) { return elements.can_get() || get_more_elements(); }

public:
  // text buffer
  // valid vox returns true if we are allowed to send
  int valid_vox(void) {
    uint8_t active = get_active_vox();
    return active == KYRF_NONE || active == vox;
  }
  // valid vox or abort
  int valid_vox_or_abort(void) {
    if ( ! valid_vox()) {
      abort();
      return 0;
    }
    return 1;
  }
  // valid_text identifies acceptable characters
  int valid_text(uint8_t value) {
    // convert to upper case
    return (value >='a' && value <= 'z') ||
      value == ' ' || value == '\t' || value == '\n' || value == '\e' || value == '|' ||
      (value-33 >= 0 && value-33 < 64 && get_nrpn(KYRP_MORSE+value-33) != 1);
  }
  // send_text queues a character for sending
  // return 0 if the value was successfully queued
  // return -1 if there is no room or the character
  // was rejected by valid_text
  int send_text(uint8_t value) {
    // only if we're allowed
    if ( ! valid_vox_or_abort()) return -1;
    // reject false characters
    if ( ! valid_text(value)) return -1;
    // convert to upper case
    if (value >='a' && value <= 'z') value -= 32;
    // map spaces to space
    if (value == ' ' || value == '\t' || value == '\n') value = ' ';
    if (chars.can_put()) {
      chars.put(value);
      return 0;
    }
    return -1;
  }
  // return true if there is room to queue a character
  int can_send_text() { return valid_vox() && chars.can_put(); }
  void unsend_text(void) {
    noInterrupts();
    if (chars.can_unput()) chars.unput();
    interrupts();
  }
private:
  int recv_text(void) { return valid_vox_or_abort() && chars.can_get() ? chars.get() : -1; }
  int peek_text(void) { return valid_vox_or_abort() && chars.can_get() ? chars.peek() : -1; }
  int can_recv_text(void) { return chars.can_get(); }

  // pull a character from the text buffer
  // translate into morse elements
  // and push them into the element buffer
  // return 1 if elements were generated
  // return 0 if no luck
  int get_more_elements() {
    if ( ! valid_vox_or_abort()) return 0;
    if ( ! can_recv_text()) return 0;
    uint8_t value = recv_text();
    if (value == ' ') {
      // send word space
      send(-get_vox_xnrpn(vox, KYRP_XPER_IWS));
      return 1;
    }
    if (value == '|') {
      // send half dit space
      send(-get_vox_xnrpn(vox, KYRP_XPER_DIT)/2);
      return 1;
    }
    int sent = 0;
    uint8_t prosign = 0;
    if (value == '\e') {
      // prosign together the next two characters
      prosign = 1;
      // we've run dry
      if ( ! can_recv_text()) return 0;
      // get the next char
      value = recv_text();
    }
    uint8_t code = get_nrpn(KYRP_MORSE+value-33);
    while (code > 1) {
      send(get_vox_xnrpn(vox, code & 1 ? KYRP_XPER_DAH : KYRP_XPER_DIT));
      send(-get_vox_xnrpn(vox, KYRP_XPER_IES));
      code >>= 1;
      sent += 2;
    }
    if (code == 1 && ! prosign) {
      send(-get_vox_xnrpn(vox, KYRP_XPER_ILS)-get_vox_xnrpn(vox, KYRP_XPER_IES));
      sent += 1;
    }
    return sent > 0;
  }
public:
  void abort() {
    chars.reset();		// clear text buffer
    elements.reset();		// clear element buffer
    element = 0;		// clear the element
  }
  virtual void update(void);
private:
  const int vox;
  int element;
  audio_block_t *block;
  int16_t *wptr, n;
  RingBuffer<int32_t, 32> elements;
  RingBuffer<uint8_t, 64> chars;
};

#endif
