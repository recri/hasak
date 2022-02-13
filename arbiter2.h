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
 * arbiter - the arbiter listens for sidetone key notes, grants the sidetone key
 * to the first fist which asks for it, but switches to higher priority fists when
 *  they interrupt the current fist.
 * straight key > paddle key > tune key > asciii keyer 
 *
 * The arbiter also manages swapping keyer parameters when switching active fist.
 * and updating computed per element timings when necessary.  Hmm, that won't
 * work perfectly, a keyer might start with the wrong timing, but it should catch
 * up soon enough.  
 *
 * This is much easier than the arbiter in the audio stream version, because the
 * ptt timing is separate.  Some keys won't generate ptt.
 */

class Arbiter2 {
 public:
  int active = -1;
  int changeover = 0;
  int timeon = 0;
  int timeoff = 0;
  elapsedMicros countup;
  struct {
    int note, vox;
  } line[KYR_N_VOX];
  void define_vox(int index, int note, int vox) {
    if (index >= 0 && index < KYR_N_VOX) {
      line[index].note = note;
      line[index].vox = vox;
    }
  }
  int get_active_vox(void) {
    return active < 0 ? KYR_VOX_NONE : line[active].vox;
  }
  void setup(void) {
    // define the voices the arbiter sees on input channels
    // changing to let their identity be their priority
    define_vox(0, KYR_NOTE_S_KEY_ST, KYR_VOX_S_KEY);
    define_vox(1, KYR_NOTE_PAD_ST, KYR_VOX_PAD);
    define_vox(2, KYR_NOTE_TUNE_ST, KYR_VOX_TUNE);
    define_vox(3, KYR_NOTE_WINK_ST, KYR_VOX_WINK);
    define_vox(4, KYR_NOTE_KEYER_ST, KYR_VOX_KYR);
    define_vox(5, KYR_NOTE_BUT_ST, KYR_VOX_BUT);
  }
  void loop(void) {
    int n_active = 0, best = -1, send = 0;

    /* are we changing active voices */
    if (changeover) {
      /* have we waited long enough for the changeover */
      if (countup < 30) return;
      countup = 0;
      changeover = 0;
    }
    
    /* find highest priority active stream */
    for (int i = 0; i < KYR_N_VOX; i += 1) {
      if (hasak.notes[line[i].note]) {
	n_active += 1;	// active if keyed
	/* remember the highest priority active stream */
	best = i;
	break;
      }
    }

    /* the result of the scan is combined with active_stream */
    if (active < 0) {
      /* there is no currently active stream */
      /* any lingering active_tail has been cleared */
      if (n_active == 0) {
	/* there is no new voice keying */
	/* send nothing */
	;
      } else {
	/* there are one or more new voice(s) */
	active = best;
	/* send the active stream */
	send = 1;
	timeon = timeoff = 0;
      }
    } else {
      /* there is a currently active stream */
      if (n_active == 0 || active <= best) {
	/* no contention */
	/* send the active stream */
	send = 1;
      } else {
	/* pre-empt active stream */
	if (hasak.notes[KYR_NOTE_KEY_ST]) note_toggle(KYR_NOTE_KEY_ST);
	/* set the new active stream */
	active = best;
	/* start the new active stream on the next loop */
	changeover = 1;
	countup = 0;
      }
    }
    if (send) {
      const uint8_t note = line[active].note;
      if (hasak.notes[KYR_NOTE_KEY_ST] != hasak.notes[note]) {
	if (hasak.notes[KYR_NOTE_KEY_ST])
	  timeon = countup;
	else
	  timeoff = countup;
	note_toggle(KYR_NOTE_KEY_ST);
	countup = 0;
      } else {
	if (hasak.notes[KYR_NOTE_KEY_ST]) {
	  /* stuck on? */
	  ;
	} else {
	  if (countup > 7*timeon)
	    active = -1;
	}
      }
    }
  }
};

static Arbiter2 arbiter2;
  
static void arbiter2_setup(void) { arbiter2.setup(); }

static void arbiter2_loop(void) { arbiter2.loop(); }
