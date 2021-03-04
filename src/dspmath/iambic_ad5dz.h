/* -*- mode: c++; tab-width: 8 -*- */
/*
  Copyright (C) 2011, 2012, 2020, 2021 by Roger E Critchlow Jr, Charlestown, MA, USA.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifndef IAMBIC_AD5DZ_H
#define IAMBIC_AD5DZ_H

/*
** A morse code keyer reduced to a simple logic class.
**
** // usage
**
** // to make a keyer
** iambic_ad5dz keyer;
**
** // to specify the current paddle state,
** // advance the keyer clock by one sample tick,
** // and receive the keyout state for the tick.
** keyout = keyer.clock(dit, dah);	
**
** the timing and modes are all specified via inline calls to get_vox_*(vox)
** which expands into an array index operation.
*/

#include "iambic.h"

class iambic_ad5dz
{
public:
  typedef unsigned char byte;
  typedef enum {
    KEY_OFF, KEY_DAH, KEY_DIT, KEY_DIDAH
  } key_t;
  typedef enum {
    KEYER_OFF, KEYER_DIT, KEYER_DIT_SPACE, KEYER_DAH, KEYER_DAH_SPACE, KEYER_SYMBOL_SPACE, KEYER_WORD_SPACE,
  } keyer_t;

  const byte vox;		// voice for fetcing parameters
  byte keyOut;			// output key state
  key_t key;			// input key didah state
  key_t memKey;			// memory of states seen since element began
  keyer_t keyerState;		// current keyer state
  int keyerDuration;		// ticks to next keyer state transition

  // initialize a keyer
  iambic_ad5dz(int vox) : vox(vox), keyOut(IAMBIC_OFF), key(KEY_OFF), memKey(KEY_OFF) { }

  // mask the key memory to the appropriate bits
  void memToDit() { memKey = (key_t)(memKey & KEY_DIT); }
  void memToDah() { memKey = (key_t)(memKey & KEY_DAH); }
  void memToOff() { memKey = KEY_OFF; }

  // transition to the specified state, with the specified duration, and set the key out state
  bool transitionTo(keyer_t newState, int newDuration) {
    switch (newState) {
    case KEYER_OFF: break;
    case KEYER_DIT: keyOut = IAMBIC_DIT; memToOff(); break;
    case KEYER_DAH: keyOut = IAMBIC_DAH; memToOff(); break;
    case KEYER_DIT_SPACE: // fall through
    case KEYER_DAH_SPACE: keyOut = IAMBIC_OFF; if (key == KEY_OFF && get_vox_pad_mode(vox) == KYRP_MODE_A) memToOff(); break;
    case KEYER_SYMBOL_SPACE: memToOff(); break;
    case KEYER_WORD_SPACE: break;
      // default: fprintf(stderr, "uncaught case %d in transitionTo()", newState);
    }
    keyerState = newState; keyerDuration += newDuration; return true;
  }
  
  // each of these functions returns true if it performs the stated action
  bool startDit() { return ((key|memKey)&KEY_DIT) != 0 && transitionTo(KEYER_DIT, get_vox_dit(vox)); }
  bool startDah() { return ((key|memKey)&KEY_DAH) != 0 && transitionTo(KEYER_DAH, get_vox_dah(vox)); }
  bool startSpace(keyer_t newState) { return  transitionTo(newState, get_vox_ies(vox)); }
  bool continueSpace(keyer_t newState, int newDuration) { return transitionTo(newState, newDuration); }
  bool symbolSpace() { return (get_vox_auto_ils(vox) && continueSpace(KEYER_SYMBOL_SPACE, get_vox_ils(vox)-get_vox_ies(vox))) || finish(); }
  bool wordSpace() { return (get_vox_auto_iws(vox) && continueSpace(KEYER_WORD_SPACE, get_vox_iws(vox)-get_vox_ils(vox))) || finish(); }
  bool finish() { return transitionTo(KEYER_OFF, 0); }

  // build key state for swapped and unswapped states
  static key_t key_in(int dit, int dah) { return (key_t)((dit<<1)|dah); }

  // clock ticks
  int clock(int raw_dit_on, int raw_dah_on, int ticks) {

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
