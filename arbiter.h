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
 * they interrupt the current fist.
 * straight key > paddle key > tune key > ascii keyer 
 */

static int arbiter_active = KYRN_ST_NONE;
static int arbiter_change_over = 0;
static elapsedSamples arbiter_counter = 0;

static void arbiter_any_st(const int note) {
  /* case 0 - change over to new active note in progress */
  if (arbiter_change_over)
    return;
  /* case 1 - continuation of active note */
  if (note == arbiter_active) {
    if (note_get(note) != note_get(KYRN_KEY_ST))
      note_toggle(KYRN_KEY_ST);
    return;
  }
  /* case 2 - no active note, become active note */
  if (arbiter_active == KYRN_ST_NONE) {
    arbiter_active = note;
    if (note_get(note) != note_get(KYRN_KEY_ST))
      note_toggle(KYRN_KEY_ST);
    return;
  }
  /* case 3 - lower priority than active note */
  if (note > arbiter_active)
    return;
  /* case 4 - preempt active note which is sounding */
  if (note_get(arbiter_active)) {
    note_off(KYRN_KEY_ST);
    arbiter_active = note;
    arbiter_change_over = 1;
    arbiter_counter = 0;
    return;
  }
  /* case 5 - preempt active note which is silent */
  arbiter_active = note;
  if (note_get(note) != note_get(KYRN_KEY_ST))
    note_toggle(KYRN_KEY_ST);
  return;
}

static void arbiter_release(const int note) {
  arbiter_active = KYRN_ST_NONE;
}

static void arbiter_setup(void) {
  note_listen(KYRN_ST_S_KEY, arbiter_any_st);
  note_listen(KYRN_ST_PAD, arbiter_any_st);
  note_listen(KYRN_ST_TEXT, arbiter_any_st);
  note_listen(KYRN_ST_TEXT2, arbiter_any_st);
  note_listen(KYRN_ST_BUT, arbiter_any_st);
  note_listen(KYRN_ST_TUNE, arbiter_any_st);
  note_listen(KYRN_PTT_OUT, arbiter_release);
}

/* FIX.ME - move to every_sample() */
static void arbiter_loop(void) {
  /* if we preempted a sounding note we have to allow one sample of sidetone off
   * to trigger the fall ramp in the audio graph */
  if (arbiter_change_over && arbiter_counter) {
    arbiter_change_over = 0;
    if (note_get(arbiter_active) != note_get(KYRN_KEY_ST))
      note_toggle(KYRN_KEY_ST);
  }
}

/* this should go into a nrpn */
int get_active_st(void) { return  arbiter_active; }
