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

#ifndef _keyer_text_h
#define _keyer_text_h

#include "Arduino.h"
#include "../config.h"
#include "../linkage.h"

#include "keyer_timing_generic.h"
#include "ring_buffer.h"

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

class KeyerText : public KeyerTimingGeneric {

public:

  const int text_note, st_note;
  RingBuffer<uint8_t, 128> chars;
  int code, prosign;
  unsigned timer;
  enum { key_none, key_dit, key_dah, key_ies, key_ils, key_iws } timing_element;

  KeyerText(int text_note, int st_note) : KeyerTimingGeneric(), text_note(text_note), st_note(st_note) {
    code = prosign =  0;
    timing_element = key_none;
  }

  bool valid_text(uint8_t value) {
    return (value-33 >= 0 && value-33 < 64 && nrpn_get(KYRP_MORSE+value-33) != 1) || 
      (value >='a' && value <= 'z') ||
      value == ' ' || value == '\t' || value == '\n' || 
      value == '\e' || value == '|';
  }

  // send_text queues a character for sending
  // return 0 if the value was successfully queued
  // return -1 if there is no room or the character
  // was rejected by valid_text
  int send_text(uint8_t value) {
    // reject false characters
    if ( ! valid_text(value)) return -1;
    // don't overfill the buffer
    if ( ! chars.can_put()) return -1;
    // convert to upper case
    if (value >='a' && value <= 'z') value -= 32;
    // map spaces to space
    if (value == ' ' || value == '\t' || value == '\n') value = ' ';
    // Serial.printf("queued %d\n", value);
    chars.put(value);
    // return success
    return 0;
  }

  int recv_text(void) { return chars.can_get() ? chars.get() : -1; }

  // return true if there is room to queue a character
  int can_send_text() { return chars.can_put(); }

  void unsend_text(void) { if (chars.can_unput()) chars.unput(); }

  void receive() { 
    // Serial.printf("keyer_text:receive from %d %d\n", text_note, note_get(text_note));
    send_text(note_get(text_note));
  }

  int valid_vox(void) {
    return st_note <= nrpn_get(KYRP_ACTIVE_ST);
  }
  
  void clock(int ticks) {
    while (valid_vox()) {
      if (timing_element != key_none) {
	timer += ticks;
	if ((int)timer < 0) 
	  break; // element timer not expired
      }
      switch (timing_element) {
      case key_none: {
	int value = recv_text();
	if (value < 0) {	   // nothing to send
	  break;
	}
	// Serial.printf("recv_text() = %d\n", value);
	if (value == ' ') {	  // send word space
	  timing_element = key_iws;
	  timer = -iws();
	} else if (value == '|') { // send half dit space
	  timing_element = key_iws;
	  timer = -dit()/2;
	} else if (value == '\e') { // prosign together the next two characters
	  prosign += 1;
	} else {
	  code = nrpn_get(KYRP_MORSE+value-33);
	  // Serial.printf("nrpn_get(KYRP_MORSE+...) = %d\n", code);
	  if (code > 1)
	    timing_element = key_ies;
	}
	continue;
      }
      case key_dit:
      case key_dah:
	note_toggle(st_note);	    // start interelement space
	timing_element = key_ies;
	timer = -ies();
	continue;

      case key_ies:
	if (code != 1) {	    // another element in code
	  note_toggle(st_note);	    // start next element
	  if (code&1) {		    // next element is dah
	    timing_element = key_dah;
	    timer = -dah();
	  } else {		    // next element is dit
	    timing_element = key_dit;
	    timer = -dit();
	  }
	  code >>= 1;	            // clear element from code
	} else if (prosign) {       // code is finished, but we're in a prosign
	  prosign -= 1;
	  timing_element = key_none;
	} else {		    // code is finished, extend ies to ils
	  timing_element = key_ils;
	  timer = -(ils()-ies());
	}
	continue;
	
      case key_ils:
	if (chars.can_get() && chars.peek() == ' ') {  // extend to iws
	  recv_text();		    // pull the space
	  timing_element = key_iws;
	  timer = -(iws()-ils());
	} else {		    // start the next code
	  timing_element = key_none;
	}
	continue;

      case key_iws:
	timing_element = key_none;
	continue;

      default:
	Serial.printf("invalid timing_element (%d) in keyer_text.loop()\n", timing_element);
      }
      break;
    }
    if ( ! valid_vox() && // lost the key
	 (chars.can_get() || // some chars to send
	 code > 1 ||	     // a char being sent
	 prosign != 0 ||     // a prosign open
	 timing_element != key_none || // timing an element
	 note_get(st_note)) ) {	       // note sounding
      chars.reset();		       // clear text buffer
      code = 0;			       // clear element buffer
      prosign = 0;		       // clear prosign
      timing_element = key_none;       // clear element timing
      if (note_get(st_note)) note_toggle(st_note); // clear sounding note
      // Serial.printf("keyer_text:abort() active %d st_note %d\n", nrpn_get(KYRP_ACTIVE_ST), st_note);
    }
  }
};


#endif