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

#ifndef keyer_nd7pa_h_
#define keyer_nd7pa_h_
/*
** This has been stripped down to the minimal iambic state machine
** from the AVR sources that accompany the article in QEX March/April
** 2012, and the length of the dah and inter-element-space has been
** made into configurable multiples of the dit clock.
*/
/*
* newkeyer.c  an electronic keyer with programmable outputs 
* Copyright (C) 2012 Roger L. Traylor   
* 
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
* 
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* 
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// newkeyer.c    
// R. Traylor
// 3.19.2012
// iambic keyer      
#include "keyer_generic.h"

class KeyerNd7pa : public KeyerGeneric {
 private:
  // keyer states
  static const int IDLE =     0;  // waiting for a paddle closure 
  static const int DIT =      1;  // making a dit 
  static const int DAH =      2;  // making a dah  
  static const int DIT_DLY =  3;  // intersymbol delay, one dot time
  static const int DAH_DLY =  4;  // intersymbol delay, one dot time

  // state variables
  int keyer_state;	// the keyer state 
  bool dit_pending;	// memory for dits  
  bool dah_pending;	// memory for dahs  
  int timer;		// ticks counting down

public:
 KeyerNd7pa(const int vox) : KeyerGeneric(vox), 
    keyer_state(IDLE), dit_pending(false), dah_pending(false) { }

  virtual int clock(int dit_on, int dah_on, int ticks) {

    int key_out = 0;

    // update timer
    timer -= ticks;
    bool timer_expired = timer <= 0;

    // keyer state machine   
    if (keyer_state == IDLE) {
      key_out = 0;
      if (dit_on) {
	timer = dit(); keyer_state = DIT;
      } else if (dah_on) {
	timer = dah(); keyer_state = DAH;
      }       
    } else if (keyer_state == DIT) {
      key_out = 1; 
      if ( timer_expired ) { timer = ies(); keyer_state = DIT_DLY; }  
    } else if (keyer_state == DAH) {
      key_out = 1; 
      if ( timer_expired ) { timer = ies(); keyer_state = DAH_DLY; }  
    } else if (keyer_state == DIT_DLY) {
      key_out = 0;  
      if ( timer_expired ) {
	if ( dah_pending ) { timer = dah(); keyer_state = DAH;
	} else { keyer_state = IDLE; }
      }
    } else if (keyer_state == DAH_DLY) {
      key_out = 0; 
      if ( timer_expired ) {
        if ( dit_pending ) {
	  timer = dit(); keyer_state = DIT;
	} else {
	  keyer_state = IDLE;
	}
      }
    }

    //*****************  dit pending state machine   *********************
    dit_pending = dit_pending ?
      keyer_state != DIT :
      (dit_on && ((keyer_state == DAH && timer < dah()/3) ||
		  (keyer_state == DAH_DLY && timer > ies()/2)));
         
    //******************  dah pending state machine   *********************
    dah_pending = dah_pending ?
      keyer_state != DAH :
      (dah_on && ((keyer_state == DIT && timer < dit()/2) ||
		  (keyer_state == DIT_DLY && timer > ies()/2)));

    return key_out;
  }
};

#endif
