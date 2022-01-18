/* Audio Library for Teensy 3.X
 * Copyright (c) 2014, Paul Stoffregen, paul@pjrc.com
 *
 * Development of this audio library was funded by PJRC.COM, LLC by sales of
 * Teensy and Audio Adaptor boards.  Please support PJRC's efforts to develop
 * open source software by purchasing Teensy or other PJRC products.
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

#include "effect_paddle.h"
#include "sample_value.h"

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
const uint8_t AudioEffectPaddle::adapt[3][2][2][2][2][3] = {
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
					          { /* r=1 */ 1, 0, 1 } } } } } } 
  ;

void AudioEffectPaddle::update(void)
{
  audio_block_t *blocka, *blockb, *blockc;
  int16_t *pa, *pb, *pc, *end;

  switch (get_vox_pad_keyer(vox)) {
  case KYRV_KEYER_AD5DZ: keyer = &ad5dz; break;
  case KYRV_KEYER_K1EL: keyer = &k1el; break;
  case KYRV_KEYER_ND7PA: keyer = &nd7pa; break;
  case KYRV_KEYER_VK6PH: keyer = &vk6ph; break;
  }
  blocka = receiveReadOnly(0);
  pa = blocka ? blocka->data : (int16_t *)zeros;
  blockb = receiveReadOnly(1);
  pb = blockb ? blockb->data : (int16_t *)zeros;
  blockc = allocate();
  int adapter = get_vox_pad_adapt(vox);
  int swapped = get_vox_pad_swap(vox);
  if (blockc) {
    int sum = 0;
    pc = blockc->data;
    end = pc + AUDIO_BLOCK_SAMPLES;
    while (pc < end) {
      uint8_t left = fix2bool(*pa++), right = fix2bool(*pb++);
      const uint8_t *map = adapt[adapter][swapped][state][left][right];
      state = map[0];
      left = map[1];
      right = map[2];
      sum += *pc++ = bool2fix(keyer->clock(left, right, 1));
    }
    if (sum != 0)
      transmit(blockc);
    release(blockc);
  }
  if (blocka) release(blocka);
  if (blockb) release(blockb);
}

