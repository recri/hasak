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

#include "../ring_buffer.h"

class KeyerText {

public:

  enum { none, dit, dah, ies, ils, iws };

  KeyerText(int text_note, int st_note) : text_note(text_note), st_note(st_note) {
    code = prosign =  0;
    timing_element = none;
  }
  int text_note, st_note;
  RingBuffer<uint8_t, 128> chars;
  int code, prosign, timing_element;
  elapsedSamples timer;

  int abort() {
    chars.reset();		// clear text buffer
    code = 0;			// clear element buffer
    prosign = 0;		// no prosign
    timing_element = none;	// element timing
    if (note_get(st_note)) note_toggle(st_note);
    Serial.printf("keyer_text:abort() active %d st_note %d\n", get_active_st(), st_note);
    return 0;			// return false
  }

  int valid_vox(void) {
    return 1;
    return st_note < get_active_st();
  }

  // valid vox or abort
  int valid_vox_or_abort(void) {
    return valid_vox() ? 1 : abort();
  }

  bool valid_text(uint8_t value) {
    return (value-33 >= 0 && value-33 < 64 && get_nrpn(KYRP_MORSE+value-33) != 1) || 
      (value >='a' && value <= 'z') ||
      value == ' ' || value == '\t' || value == '\n' || 
      value == '\e' || value == '|';
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

  int recv_text(void) { return valid_vox_or_abort() && chars.can_get() ? chars.get() : -1; }

  // return true if there is room to queue a character
  int can_send_text() { return valid_vox() && chars.can_put(); }

  void unsend_text(void) { if (chars.can_unput()) chars.unput(); }

  void receive() { 
    Serial.printf("keyer_text:receive from %d %d\n", text_note, note_get(text_note));
    send_text(note_get(text_note));
  }

  void loop(void) {

  while (valid_vox_or_abort()) {
      switch (timing_element) {
      case none: {
	int value = recv_text();
	if (value < 0) {	   // nothing to send
	  return;
	}
	if (value == ' ') {	  // send word space
	  timing_element = iws;
	  timer = -get_xnrpn(KYRP_XPER_IWS);
	} else if (value == '|') { // send half dit space
	  timing_element = iws;
	  timer = -get_xnrpn(KYRP_XPER_DIT)/2;
	} else if (value == '\e') { // prosign together the next two characters
	  prosign += 1;
	} else {
	  // Serial.printf("keyer_text::loop() try to get_nrpn(%d)\n", KYRP_MORSE+value-33);
	  // return;
	  code = get_nrpn(KYRP_MORSE+value-33);
	  if (code > 1)
	    timing_element = ies;
	}
	continue;
      }
      case dit:
      case dah:
	if ((int)timer < 0) return; // element timer not expired
	note_toggle(st_note);	    // start interelement space
	timer = -get_xnrpn(KYRP_XPER_IES);
	timing_element = ies;
	continue;

      case ies:
	if ((int)timer < 0) return; // element timer not expired
	if (code != 1) {	    // another element in code
	  note_toggle(st_note);	    // start next element
	  if (code&1) {		    // next element is dah
	    timer = -get_xnrpn(KYRP_XPER_DAH);
	    timing_element = dah;
	  } else {		    // next element is dit
	    timer = -get_xnrpn(KYRP_XPER_DIT);
	    timing_element = dit;
	  }
	  code >>= 1;	            // clear element from code
	} else if (prosign) {       // code is finished, but we're in a prosign
	  prosign -= 1;
	  timing_element = none;
	} else {		    // code is finished, extend ies to ils
	  timer = -(get_xnrpn(KYRP_XPER_ILS)-get_xnrpn(KYRP_XPER_IES));
	  timing_element = ils;
	}
	continue;
	
      case ils:
	if ((int)timer < 0) return; // element timer not expired
	if (chars.peek() == ' ') {  // extend to iws
	  recv_text();		    // pull the space
	  timer = -(get_xnrpn(KYRP_XPER_IWS)-get_xnrpn(KYRP_XPER_ILS));
	  timing_element = iws;
	} else {		    // start the next code
	  timing_element = none;
	}
	continue;

      case iws:
	if ((int)timer < 0) return; // element timer not expired
	timing_element = none;
	continue;

      default:
	Serial.printf("invalid timing_element (%d) in keyer_text.loop()\n", timing_element);
      }
    }
  }
};

static KeyerText keyer_text_wink(KYRN_TXT_WINK, KYRN_WINK_ST);
static KeyerText keyer_text_kyr(KYRN_TXT_KYR, KYRN_KYR_ST);

static void keyer_text_wink_listen(int note) { keyer_text_wink.receive(); }

static void keyer_text_kyr_listen(int note) { keyer_text_kyr.receive(); }

static void keyer_text_setup(void) {
  note_listen(keyer_text_wink.text_note, keyer_text_wink_listen);
  note_listen(keyer_text_kyr.text_note, keyer_text_kyr_listen);
}

static void keyer_text_loop(void) {
  keyer_text_wink.loop();
  keyer_text_kyr.loop();
}
