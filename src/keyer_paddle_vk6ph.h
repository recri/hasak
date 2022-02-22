/* -*- mode: c++; tab-width: 8 -*- */
/*
  Copyright (C) 2018 by Roger E Critchlow Jr, Charlestown, MA, USA.

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

#ifndef keyer_paddle_vk6ph_h_
#define keyer_paddle_vk6ph_h_

/*

    10/12/2016, Rick Koch / N1GP, I adapted Phil's verilog code from
                the openHPSDR Hermes iambic.v implementation to build
                and run on a raspberry PI 3.

    1/7/2017,   N1GP, adapted to work with Jack Audio, much better timing.

    8/3/2018,   Roger Critchlow / AD5DZ/1, I adapted Rick's adaptation to
		run when clocked at specified microseconds per tick, as
		necessary when running inside a jack frame processor callback.
		Rick's code from https://github.com/n1gp/iambic-keyer
--------------------------------------------------------------------------------
This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.
This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
Boston, MA  02110-1301, USA.
--------------------------------------------------------------------------------


---------------------------------------------------------------------------------
        Copywrite (C) Phil Harman VK6PH May 2014
---------------------------------------------------------------------------------

        The code implements an Iambic CW keyer.  The following features are supported:

                * Variable speed control from 1 to 60 WPM
                * Dot and Dash memory
                * Straight, Bug, Iambic Mode A or B Modes
                * Variable character weighting
                * Automatic Letter spacing
                * Paddle swap

        Dot and Dash memory works by registering an alternative paddle closure whilst a paddle is pressed.
        The alternate paddle closure can occur at any time during a paddle closure and is not limited to being
        half way through the current dot or dash. This feature could be added if required.

        In Straight mode, closing the DASH paddle will result in the output following the input state.  This enables a
        straight morse key or external Iambic keyer to be connected.

        In Bug mode closing the dot paddle will send repeated dots.

        The difference between Iambic Mode A and B lies in what the keyer does when both paddles are released. In Mode A the
        keyer completes the element being sent when both paddles are released. In Mode B the keyer sends an additional
        element opposite to the one being sent when the paddles are released.

        This only effects letters and characters like C, period or AR.

        Automatic Letter Space works as follows: When enabled, if you pause for more than one dot time between a dot or dash
        the keyer will interpret this as a letter-space and will not send the next dot or dash until the letter-space time has been met.
        The normal letter-space is 3 dot periods. The keyer has a paddle event memory so that you can enter dots or dashes during the
        inter-letter space and the keyer will send them as they were entered.

        Speed calculation -  Using standard PARIS timing, dot_period(mS) = 1200/WPM
*/
#include "keyer_paddle_generic.h"

class KeyerVk6ph : public KeyerPaddleGeneric {

private:
  enum {
    CHECK = 0,
    PREDOT,
    PREDASH,
    SENDDOT,
    SENDDASH,
    DOTDELAY,
    DASHDELAY,
    DOTHELD,
    DASHHELD,
    LETTERSPACE,
    EXITLOOP
  };
  enum {
    KEYER_MODE_A = KYRV_MODE_A,
    KEYER_MODE_B = KYRV_MODE_B,
    KEYER_STRAIGHT = KYRV_MODE_S
  };
  bool dot_memory = 0;
  bool dash_memory = 0;
  int key_state = CHECK;
  int32_t kdelay = 0;

public:
 KeyerVk6ph() : KeyerPaddleGeneric() { }

  virtual int clock(int dit_on, int dah_on, int ticks) {
    int keyer_out = 0;
    switch(key_state) {
    case CHECK:		// check for key press
      if (pad_mode() == KEYER_STRAIGHT) { // Straight/External key or bug
	if (dah_on) {	// send manual dashes
	  keyer_out = 1;
	  key_state = CHECK;
	} else if (dit_on)	// and automatic dots
	  key_state = PREDOT;
	else {
	  keyer_out = 0;
	  key_state = CHECK;
	}
      } else {
	if (dit_on)
	  key_state = PREDOT;
	else if (dah_on)
	  key_state = PREDASH;
	else {
	  keyer_out = 0;
	  key_state = CHECK;
	}
      }
      break;
    case PREDOT:	   // need to clear any pending dots or dashes
      key_state = SENDDOT;
      dot_memory = dash_memory = 0;
      break;
    case PREDASH:
      key_state = SENDDASH;
      dot_memory = dash_memory = 0;
      break;
      // dot paddle  pressed so set keyer_out high for time dependant on speed
      // also check if dash paddle is pressed during this time
    case SENDDOT:
      keyer_out = 1;
      if (kdelay >= dit()) {
	kdelay = 0;
	keyer_out = 0;
	key_state = DOTDELAY; // add inter-character spacing of one dot length
      } else
	kdelay += ticks;
      // if Mode A and both paddels are relesed then clear dash memory
      if (!dit_on & !dah_on) {
	if (pad_mode() == KEYER_MODE_A)
	  dash_memory = 0;
      } else 
	dash_memory |= dah_on;	// set dash memory
      break;
      // dash paddle pressed so set keyer_out high for time dependant on 3 x dot delay and weight
      // also check if dot paddle is pressed during this time
    case SENDDASH:
      keyer_out = 1;
      if (kdelay >= dah()) {
	kdelay = 0;
	keyer_out = 0;
	key_state = DASHDELAY; // add inter-character spacing of one dot length
      } else 
	kdelay += ticks;
      // if Mode A and both padles are relesed then clear dot memory
      if (!dit_on & !dah_on) {
	if (pad_mode() == KEYER_MODE_A)
	  dot_memory = 0;
      } else
	dot_memory |= dit_on;	// set dot memory
      break;
      // add dot delay at end of the dot and check for dash memory, then check if paddle still held
    case DOTDELAY:
      if (kdelay >= ies()) {
	kdelay = 0;
	if(!dit_on && pad_mode() == KEYER_STRAIGHT)   // just return if in bug mode
	  key_state = CHECK;
	else if (dash_memory) // dash has been set during the dot so service
	  key_state = PREDASH;
	else 
	  key_state = DOTHELD; // dot is still active so service
      } else
	kdelay += ticks;
      dash_memory = dah_on;	// set dash memory
      break;
    case DASHDELAY: // add dot delay at end of the dash and check for dot memory, then check if paddle still held
      if (kdelay >= ies()) {
	kdelay = 0;
	if (dot_memory) // dot has been set during the dash so service
	  key_state = PREDOT;
	else 
	  key_state = DASHHELD; // dash is still active so service
      } else
	kdelay += ticks;
      dot_memory = dit_on;	// set dot memory 
      break;
    case DOTHELD: // check if dot paddle is still held, if so repeat the dot. Else check if Letter space is required
      if (dit_on)	// dot has been set during the dash so service
	key_state = PREDOT;
      else if (dah_on)	// has dash paddle been pressed
	key_state = PREDASH;
      else if (auto_ils()) { // Letter space enabled so clear any pending dots or dashes
	dot_memory = dash_memory = 0;
	key_state = LETTERSPACE;
      } else
	key_state = CHECK;
      break;
    case DASHHELD: // check if dash paddle is still held, if so repeat the dash. Else check if Letter space is required
      if (dah_on)   // dash has been set during the dot so service
	key_state = PREDASH;
      else if (dit_on)		// has dot paddle been pressed
	key_state = PREDOT;
      else if (auto_ils()) { // Letter space enabled so clear any pending dots or dashes
	dot_memory = dash_memory = 0;
	key_state = LETTERSPACE;
      } else
	key_state = CHECK;
      break;
    case LETTERSPACE: // Add remainder of letter space (3 x dot delay) to end of character and check if a paddle is pressed during this time.
      if (kdelay >= ils()-ies()) {
	kdelay = 0;
	if (dot_memory) // check if a dot or dash paddle was pressed during the delay.
	  key_state = PREDOT;
	else if (dash_memory)
	  key_state = PREDASH;
	else
	  key_state = CHECK; // no memories set so restart
      } else
	kdelay += ticks;
      // save any key presses during the letter space delay
      dot_memory |= dit_on;
      dash_memory |= dah_on;
      break;
    default:
      key_state = CHECK;
      break;
    }
    
    return keyer_out;
  }
};

#endif
