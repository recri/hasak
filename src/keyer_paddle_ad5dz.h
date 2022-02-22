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
#ifndef _keyer_paddle_ad5dz_h_
#define _keyer_paddle_ad5dz_h_

#include "keyer_paddle_generic.h"

class KeyerAd5dz : public KeyerPaddleGeneric {
 public:
 KeyerAd5dz() : KeyerPaddleGeneric() { }
  /* 
     keyer, coded in line because it works
     int clock(dit, dah, ticks) is called to advance the state machine
  */
  typedef unsigned char byte;
  typedef enum {
    KEY_OFF, KEY_DAH, KEY_DIT, KEY_DIDAH
  } key_t;
  typedef enum {
    KEYER_OFF, KEYER_DIT, KEYER_DIT_SPACE, KEYER_DAH, KEYER_DAH_SPACE, KEYER_SYMBOL_SPACE, KEYER_WORD_SPACE,
  } keyer_t;

  byte keyOut = 0;		// output key state
  key_t key = KEY_OFF;		// input key didah state
  key_t memKey = KEY_OFF;	// memory of states seen since element began
  keyer_t keyerState;		// current keyer state
  int32_t keyerDuration;	// ticks to next keyer state transition

  // mask the key memory to the appropriate bits
  void memToDit(void) { memKey = (key_t)(memKey & KEY_DIT); }
  void memToDah(void) { memKey = (key_t)(memKey & KEY_DAH); }
  void memToOff(void) { memKey = KEY_OFF; }

  // transition to the specified state, with the specified duration, and set the key out state
  bool transitionTo(keyer_t newState, int32_t newDuration) {
    switch (newState) {
    case KEYER_OFF: break;
    case KEYER_DIT: keyOut = 1; memToOff(); break;
    case KEYER_DAH: keyOut = 1; memToOff(); break;
    case KEYER_DIT_SPACE: // fall through
    case KEYER_DAH_SPACE: keyOut = 0; if (key == KEY_OFF && pad_mode() == KYRV_MODE_A) memToOff(); break;
    case KEYER_SYMBOL_SPACE: memToOff(); break;
    case KEYER_WORD_SPACE: break;
      // default: fprintf(stderr, "uncaught case %d in transitionTo()", newState);
    }
    keyerState = newState; keyerDuration += newDuration; return true;
  }
  
  // each of these functions returns true if it performs the stated action
  bool startDit(void) { return ((key|memKey)&KEY_DIT) != 0 && transitionTo(KEYER_DIT, dit()); }
  bool startDah(void) { return ((key|memKey)&KEY_DAH) != 0 && transitionTo(KEYER_DAH, dah()); }
  bool startSpace(keyer_t newState) { return  transitionTo(newState, ies()); }
  bool continueSpace(keyer_t newState, int32_t newDuration) { return transitionTo(newState, newDuration); }
  bool symbolSpace(void) { return (auto_ils() && continueSpace(KEYER_SYMBOL_SPACE, ils()-ies())) || finish(); }
  bool wordSpace(void) { return (auto_iws() && continueSpace(KEYER_WORD_SPACE, iws()-ils())) || finish(); }
  bool finish(void) { return transitionTo(KEYER_OFF, 0); }

  // build key state for swapped and unswapped states
  static key_t key_in(int dit, int dah) { return (key_t)((dit<<1)|dah); }

  // clock ticks
  virtual int clock(int raw_dit_on, int raw_dah_on, int ticks) {

    key = key_in(raw_dit_on, raw_dah_on);

    memKey = (key_t)(memKey | key);

    if ((keyerDuration -= ticks) > 0) return keyOut;

    switch (keyerState) {
    case KEYER_OFF: keyerDuration = 0; memToOff(); startDit() || startDah(); break;
    case KEYER_DIT: startSpace(KEYER_DIT_SPACE); break;
    case KEYER_DAH: startSpace(KEYER_DAH_SPACE); break;
    case KEYER_DIT_SPACE: memToDah(); startDah() || startDit() || symbolSpace(); break;
    case KEYER_DAH_SPACE: memToDit(); startDit() || startDah() || symbolSpace(); break;
    case KEYER_SYMBOL_SPACE: startDit() || startDah() || wordSpace(); break;
    case KEYER_WORD_SPACE: startDit() || startDah() || finish(); break;
    }
    return keyOut;
  }
  
};
#endif
