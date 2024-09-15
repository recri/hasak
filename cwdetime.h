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
** listen to a key line, detime the on/off into elements
** listen to the elements, decode the sequences into ascii
** transcoded into C from the javascript of https://keyer.elf.org
*/

/*
** The basic problem is to infer the dit clock rate from observations of dits,
** dahs, inter-element spaces, inter-letter spaces, and maybe inter-word spaces.
**
** Assume that each element observed is either a dit or a dah and record its
** contribution to the estimated dot clock as if it were both T and 3*T in length.
** Similarly, take each space observed as potentially T, 3*T, and 7*T in length.
**
** But weight the T, 3*T, and 7*T observations by the inverse of their squared
** distance from the current estimate, and weight the T, 3*T, and 7*T observations
**
** by their observed frequency in morse code.
**
** Until detime has seen both dits and dahs, it may be a little confused.
*/
static struct cwdetime_data {
  float wpm = 0; /* float words per minute estimate */
  float estimate = 0; /* float estimated dot clock period */
  uint32_t frame = 0; /* float time of last event */
  int nDit = 1; /* unsigned number of dits estimated */
  int nDah = 1; /* unsigned number of dahs estimated */
  int nIes = 1; /* unsigned number of inter-element spaces estimated */
  int nIls = 1; /* unsigned number of inter-letter spaces estimated */
  int nIws = 1; /* unsigned number of inter-word spaces estimated */
} cwdetime_data = { 0, 0, 0, 1, 1, 1, 1, 1 };

// as it turns out, AUDIO_SAMPLE_RATE * 60 / 50 is an exact integer
// and the same formula works in both directions 
// to convert word/minute into samples/dit,
// and to convert samples/dit into word/minute.
// is that right?
static float cwdetime_dit_to_wpm(float dit) {
  const int f = (AUDIO_SAMPLE_RATE * 60.0 / 50.0);
  return  f / dit;
}

static float cwdetime_wpm_to_dit(float wpm) {
  return cwdetime_dit_to_wpm(wpm);
}

static void cwdetime_listen(int _, int __, int velocity) {
  uint32_t this_frame = timing_samples();
  int observation = this_frame-cwdetime_data.frame;
  cwdetime_data.frame = this_frame;
  
  if (velocity) { 	/* start dit or dah, end of space */
    const int oIes = observation;	// the observed length as a one dit space
    const int oIls = observation / 3; // the observed length as a three dit space
    const float dIes = oIes - cwdetime_data.estimate; // how far off the 1 dit space assumption is
    const float dIls = oIls - cwdetime_data.estimate; // haw far off the 3 dit space assumption is
    int guess = (100 * observation) / cwdetime_data.estimate; // what we'd guess from our current
    if (dIes == 0 || dIls == 0) {
      /* 
      ** if one of the observations is spot on,
      ** then 1/(d*d) will be infinite
      ** and the estimate is unchanged 
      */
    } else if (guess > 500) {
      /* if it looks like a word space, it could be any length, don't worry about how long it is */
    } else {
      const float wIes = (1.0 * cwdetime_data.nIes) / (dIes * dIes);
      const float wIls = (1.0 * cwdetime_data.nIls) / (dIls * dIls);
      const float wt = wIes + wIls;
      const float update = (oIes * wIes + oIls * wIls) / wt;
      // console.log("o_ies="+o_ies+", w_ies="+w_ies+", o_ils="+o_ils+", w_ils="+w_ils+", wt="+wt);
      // console.log("update="+update+", estimate="+cwdetime_data.estimate);
      cwdetime_data.estimate += update;
      cwdetime_data.estimate /= 2;
      cwdetime_data.wpm = cwdetime_dit_to_wpm(cwdetime_data.estimate);
      guess = (100 * observation) / cwdetime_data.estimate;
    }
    if (guess < 200) {
      cwdetime_data.nIes += 1;
      return;
    }
    if (guess < 500) {
      cwdetime_data.nIls += 1;
      note_set(NOTE_ELT_DEC, ' ');
      return;
    }
    cwdetime_data.nIws += 1;
    note_set(NOTE_ELT_DEC, '\t');
    return;
  } else {			/* end dit or dah, start of space  */
    /* the end of a dit or a dah */
    const int oDit = observation; /* if it's a dit, then the length is the dit clock observation */
    const int oDah = observation / 3; /* float if it's a dah, then the length/3 is the dit clock observation */
    const float dDit = oDit - cwdetime_data.estimate; /* float the dit distance from the current estimate */
    const float dDah = oDah - cwdetime_data.estimate; /* float the dah distance from the current estimate */
    if (dDit == 0 || dDah == 0) {
      /* one of the observations is spot on, so 1/(d*d) will be infinite and the estimate is unchanged */
    } else {
      /* the weight of an observation is the observed frequency of the element scaled by inverse of
       * distance from our current estimate normalized to one over the observations made.
       */
      const float wDit = (1.0 * cwdetime_data.nDit) / (dDit * dDit); /* raw weight of dit observation */
      const float wDah = (1.0 * cwdetime_data.nDah) / (dDah * dDah); /* raw weight of dah observation */
      const float wt = wDit + wDah; /* weight normalization */
      const float update = (oDit * wDit + oDah * wDah) / wt;
      // console.log("o_dit="+o_dit+", w_dit="+w_dit+", o_dah="+o_dah+", w_dah="+w_dah+", wt="+wt);
      // console.log("update="+update+", estimate="+cwdetime_data.estimate);
      cwdetime_data.estimate += update;
      cwdetime_data.estimate /= 2;
      cwdetime_data.wpm = cwdetime_dit_to_wpm(cwdetime_data.estimate);
    }
    const int guess = (100 * observation) / cwdetime_data.estimate; /* make a guess */
    if (guess < 200) {
      cwdetime_data.nDit += 1;
      note_set(NOTE_ELT_DEC, '.');
      return;
    }
    cwdetime_data.nDah += 1;
    note_set(NOTE_ELT_DEC, '-');
    return;
  }
}

static void cwdetime_setup(void) {
  cwdetime_data.wpm = 15;
  cwdetime_data.estimate = cwdetime_wpm_to_dit(cwdetime_data.wpm);
  note_listen(NOTE_KEY_ST, cwdetime_listen);
}
