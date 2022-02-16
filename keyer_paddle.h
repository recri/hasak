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

#include "keyer_generic.h"
#include "keyer_ad5dz.h"
#include "keyer_k1el.h"
#include "keyer_nd7pa.h"
#include "keyer_vk6ph.h"

/*
** paddle keyer is a choice of adapter and choice of iambic keyer
**
** adapter table
*/

/*
** code for adapter retrieved from 
** https://www.amateurradio.com/single-lever-and-ultimatic-adapter/
** November 13, 2020
** Posted 17 January 2014 | by Sverre LA3ZA
** Rewritten to encode state|left|right into a 3 bit integer
** and which indexes a table of outputs to be decoded.
** original code contains this attribution:
       Direct implementation of table 3 in "K Schmidt (W9CF)
       "An ultimatic adapter for iambic keyers"
       http://fermi.la.asu.edu/w9cf/articles/ultimatic/ultimatic.html
       with the addition of the Single-paddle emulation mode
*/ 
/*
** so, if we leave out all the packing and unpacking of bits
** this is simply a multidimensional array, 144 bytes
** where the last 3 bytes specify the result left, right, and state
** the first index specifies {not-swap, swap} as {0,1},
** the second index specifies {normal, ultimatic, single-lever} as {0,1,2}
** the third index is the left paddle { low, high } as {0,1}
** the fourth index is the right paddle { low, high } as {0,1}
** the fifth index is the state variable { low, high } as {0,1}
** and the last index picks {left', right', state'} as {0,1}
** x=0 -> paddles not swapped
** x=1 -> paddles swapped
** a=0 -> adapter normal
** a=1 -> adapter ultimatic
** a=2 -> adapter single lever
** l=0, r=0, s=0 -> left paddle, right paddle, state variable false
** l=1, r=1, s=1 -> left paddle, right paddle, state variable true
** the three numbers on each row are the output l, r, and s.
*/
/*                                            a  x  s  l  r -> [s' l' r'] */
static const uint8_t keyer_paddle_adapt[3][2][2][2][2][3] = {
  { /* a=0 */ { /* x=0 */ { /* s=0 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 },	/* normal { 0, R, L, LR } */
					          { /* r=1 */ 0, 0, 1 } },
			              { /* l=1 */ { /* r=0 */ 0, 1, 0 }, 
				                  { /* r=1 */ 0, 1, 1 } } },
		          { /* s=1 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 }, 
					          { /* r=1 */ 0, 0, 1 } },
			              { /* l=1 */ { /* r=0 */ 0, 1, 0 }, 
					          { /* r=1 */ 0, 1, 1 } } } },
              { /* x=1 */ { /* s=0 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 },	/* normal swapped { 0, L, R, LR } */
					          { /* r=1 */ 0, 1, 0 } },
			              { /* l=1 */ { /* r=0 */ 0, 0, 1 }, 
					          { /* r=1 */ 0, 1, 1 } } },
			  { /* s=1 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 }, 
					          { /* r=1 */ 0, 1, 0 } },
			              { /* l=1 */ { /* r=0 */ 0, 0, 1 }, 
					          { /* r=1 */ 0, 1, 1 } } } } },
  { /* a=1 */ { /* x=0 */ { /* s=0 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 },	/* ultimatic { 0, R, SL, L, 0, R, SL, SR } */
					          { /* r=1 */ 0, 0, 1 } },
			              { /* l=1 */ { /* r=0 */ 1, 1, 0 }, 
					          { /* r=1 */ 0, 1, 0 } } },
		          { /* s=1 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 }, 
					          { /* r=1 */ 0, 0, 1 } },
			              { /* l=1 */ { /* r=0 */ 1, 1, 0 }, 
					          { /* r=1 */ 1, 0, 1 } } } },
              { /* x=1 */ { /* s=0 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 },	/* ultimatic swapped { 0, L, SR, R, 0, L, SR, SL }*/
					          { /* r=1 */ 0, 1, 0 } },
			              { /* l=1 */ { /* r=0 */ 1, 0, 1 }, 
					          { /* r=1 */ 0, 0, 1 } } },
		          { /* s=1 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 }, 
					          { /* r=1 */ 0, 1, 0 } },
			              { /* l=1 */ { /* r=0 */ 1, 0, 1 }, 
					          { /* r=1 */ 1, 1, 0 } } } } },
  { /* a=2 */ { /* x=0 */ { /* s=0 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 },	/* single lever { 0, R, SL, R, 0, R, SL, SL } */
					          { /* r=1 */ 0, 0, 1 } },
			              { /* l=1 */ { /* r=0 */ 1, 1, 0 }, 
					          { /* r=1 */ 0, 0, 1 } } },
		          { /* s=1 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 }, 
					          { /* r=1 */ 0, 0, 1 } },
			              { /* l=1 */ { /* r=0 */ 1, 1, 0 }, 
					          { /* r=1 */ 1, 1, 0 } } } },
              { /* x=1 */ { /* s=0 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 },	/* single lever swapped { 0, L, SR, L, 0, L, SR, SR } */
					          { /* r=1 */ 0, 1, 0 } },
			              { /* l=1 */ { /* r=0 */ 1, 0, 1 }, 
					          { /* r=1 */ 0, 1, 0 } } },
		          { /* s=1 */ { /* l=0 */ { /* r=0 */ 0, 0, 0 }, 
					          { /* r=1 */ 0, 1, 0 } },
			              { /* l=1 */ { /* r=0 */ 1, 0, 1 }, 
					          { /* r=1 */ 1, 0, 1 } } } } } };

/*
** paddle keyer class which combines adapter and iambic keyer
*/

class KeyerPaddle {
 public:
  KeyerPaddle() : state(0)
  {
  }
  int clock(const int left, const int right, const int ticks) { 
    const int adapter = get_nrpn(KYRP_PAD_ADAPT);
    const int swapped = get_nrpn(KYRP_PAD_SWAP);
    const uint8_t *map = keyer_paddle_adapt[adapter][swapped][state][left][right];
    state = map[0];
    const int dit = map[1];
    const int dah = map[2];
    switch (get_nrpn(KYRP_PAD_KEYER)) {
    case KYRV_KEYER_AD5DZ: return ad5dz.clock(dit, dah, ticks);
    case KYRV_KEYER_K1EL: return k1el.clock(dit, dah, ticks);
    case KYRV_KEYER_ND7PA: return nd7pa.clock(dit, dah, ticks);
    case KYRV_KEYER_VK6PH: return vk6ph.clock(dit, dah, ticks);
    default: return 0;
    }
  }
private:
  uint8_t state;
  KeyerAd5dz ad5dz;
  KeyerK1el k1el;
  KeyerNd7pa nd7pa;
  KeyerVk6ph vk6ph;
};

static KeyerPaddle keyer_paddle;

static void keyer_paddle_listener(int note) {
  const int key = keyer_paddle.clock(hasak.notes[KYRN_L_PAD], hasak.notes[KYRN_R_PAD], 0);
  if (key != note_get(KYRN_PAD_ST))
    note_toggle(KYRN_PAD_ST);
  //Serial.printf("keyer_pad(%d) -> key %d\n", note, key);
}

static void keyer_paddle_setup(void) {
  note_listen(KYRN_L_PAD, keyer_paddle_listener);
  note_listen(KYRN_R_PAD, keyer_paddle_listener);
}

static void keyer_paddle_loop(void) {
  static elapsedSamples ticks;
  if (ticks) {
    const int key = keyer_paddle.clock(hasak.notes[KYRN_L_PAD], hasak.notes[KYRN_R_PAD], ticks);
    if (key != note_get(KYRN_PAD_ST))
      note_toggle(KYRN_PAD_ST);
    ticks = 0;
  }
}


