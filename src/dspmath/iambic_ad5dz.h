/* -*- mode: c++; tab-width: 8 -*- */
/*
  Copyright (C) 2011, 2012, 2020 by Roger E Critchlow Jr, Charlestown, MA, USA.

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

#include "morse_keyer.h"

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
** // the timing is specified with
** keyer.setTimes(sample_rate, words_per_minute, weight, ratio, compensation, farnsworth)
**
** // or alacarte, as detailed in iambic_keyer.h
** 
** // the mode is specified with
** k.setModes(keyer_mode, // 'A' or 'B' or 'S' or 'G' or 'U'
**            swap_paddles,
**            automatic_inter_letter_space,
**            automatic_inter_word_space
**        )
*/

/*
** May need to parameterize the ticks type
** in case the int is too small on an microprocessor
*/

// #include <stdio.h>
#include "iambic.h"
#include "morse_keyer.h"

class iambic_ad5dz : public morse_keyer
{
public:
  typedef unsigned char byte;
  typedef enum {
    KEY_OFF, KEY_DAH, KEY_DIT, KEY_DIDAH
  } key_t;
  typedef enum {
    KEYER_OFF, KEYER_DIT, KEYER_DIT_SPACE, KEYER_DAH, KEYER_DAH_SPACE, KEYER_SYMBOL_SPACE, KEYER_WORD_SPACE,
  } keyer_t;

  char _mode;			// mode B or mode A or ...
  bool _autoIls;		// automatically time space between letters
  bool _autoIws;		// automatically time space between words

  byte _keyOut;			// output key state
  key_t _key;			// input key didah state, swapped
  key_t _memKey;		// memory of states seen since element began
  keyer_t _keyerState;		// current keyer state
  int _keyerDuration;		// ticks to next keyer state transition

  // initialize a keyer
  iambic_ad5dz() : morse_keyer() {
    _keyOut = IAMBIC_OFF;
    _key = KEY_OFF;
    _memKey = KEY_OFF;
    setModes('B', false, false, false);
  }

  // mask the key memory to the appropriate bits
  void _memToDit() { _memKey = (key_t)(_memKey & KEY_DIT); }
  void _memToDah() { _memKey = (key_t)(_memKey & KEY_DAH); }
  void _memToOff() { _memKey = KEY_OFF; }

  // transition to the specified state, with the specified duration, and set the key out state
  bool _transitionTo(keyer_t newState, int newDuration) {
    switch (newState) {
    case KEYER_OFF: break;
    case KEYER_DIT: _keyOut = IAMBIC_DIT; _memToOff(); break;
    case KEYER_DAH: _keyOut = IAMBIC_DAH; _memToOff(); break;
    case KEYER_DIT_SPACE: // fall through
    case KEYER_DAH_SPACE: _keyOut = IAMBIC_OFF; if (_key == KEY_OFF && _mode == 'A') _memToOff(); break;
    case KEYER_SYMBOL_SPACE: _memToOff(); break;
    case KEYER_WORD_SPACE: break;
      // default: fprintf(stderr, "uncaught case %d in transitionTo()", newState);
    }
    _keyerState = newState; _keyerDuration += newDuration; return true;
  }
  
  // each of these functions returns true if it performs the stated action
  bool _startDit() { return ((_key|_memKey)&KEY_DIT) != 0 && _transitionTo(KEYER_DIT, _ticksPerDit); }
  bool _startDah() { return ((_key|_memKey)&KEY_DAH) != 0 && _transitionTo(KEYER_DAH, _ticksPerDah); }
  bool _startSpace(keyer_t newState) { return  _transitionTo(newState, _ticksPerIes); }
  bool _continueSpace(keyer_t newState, int newDuration) { return _transitionTo(newState, newDuration); }
  bool _symbolSpace() { return (_autoIls && _continueSpace(KEYER_SYMBOL_SPACE, _ticksPerIls-_ticksPerIes)) || _finish(); }
  bool _wordSpace() { return (_autoIws && _continueSpace(KEYER_WORD_SPACE, _ticksPerIws-_ticksPerIls)) || _finish(); }
  bool _finish() { return _transitionTo(KEYER_OFF, 0); }

  // build key state for swapped and unswapped states
  static key_t _key_in(int dit, int dah) { return (key_t)((dit<<1)|dah); }
  static key_t _is_swapped(int raw_dit_on, int raw_dah_on) { return _key_in(raw_dah_on, raw_dit_on); }
  static key_t _is_not_swapped(int raw_dit_on, int raw_dah_on) { return _key_in(raw_dit_on, raw_dah_on); }
  key_t (*_fix_swapped)(int raw_dit_on, int raw_dit_off) = &_is_not_swapped;

  // clock ticks
  int clock(int raw_dit_on, int raw_dah_on) {

    _key = _fix_swapped(raw_dit_on, raw_dah_on);

    _memKey = (key_t)(_memKey | _key);

    if ((_keyerDuration -= 1) > 0) return _keyOut;

    switch (_keyerState) {
    case KEYER_OFF: _keyerDuration = 0; _memToOff(); _startDit() || _startDah(); break;
    case KEYER_DIT: _startSpace(KEYER_DIT_SPACE); break;
    case KEYER_DAH: _startSpace(KEYER_DAH_SPACE); break;
    case KEYER_DIT_SPACE: _memToDah(); _startDah() || _startDit() || _symbolSpace(); break;
    case KEYER_DAH_SPACE: _memToDit(); _startDit() || _startDah() || _symbolSpace(); break;
    case KEYER_SYMBOL_SPACE: _startDit() || _startDah() || _wordSpace(); break;
    case KEYER_WORD_SPACE: _startDit() || _startDah() || _finish(); break;
    }
    return _keyOut;
  }

  // set the modes
  void setModes(char mode, bool swapped, bool autoIls, bool autoIws) {
    _mode = mode;		// set the paddle key mode
    _autoIls = autoIls;		// set the auto inter-letter spacing
    _autoIws = autoIws;		// set the auto inter-word spacing
    _fix_swapped = swapped ? &_is_swapped : &_is_not_swapped;
  }
};

extern "C" {
  typedef struct {
    iambic_ad5dz k;
  } iambic_ad5dz_t;
  typedef struct {
  } iambic_ad5dz_options_t;
}

#endif
