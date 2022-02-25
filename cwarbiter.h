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

static int cwarbiter_change_over = 0;

static void cwarbiter_any_st(const int note) {
  /* case 0 - change over to new active note in progress */
  if (cwarbiter_change_over)
    return;
  /* case 1 - continuation of active note */
  if (note == nrpn_get(KYRP_ACTIVE_ST)) {
    if (note_get(note) != note_get(KYRN_KEY_ST))
      note_toggle(KYRN_KEY_ST);
    return;
  }
  /* case 2 - no active note, become active note */
  if (nrpn_get(KYRP_ACTIVE_ST) == KYRN_ST_NONE) {
    nrpn_set(KYRP_ACTIVE_ST, note);
    if (note_get(note) != note_get(KYRN_KEY_ST))
      note_toggle(KYRN_KEY_ST);
    return;
  }
  /* case 3 - lower priority than active note */
  if (note > nrpn_get(KYRP_ACTIVE_ST))
    return;
  /* case 4 - preempt active note which is sounding */
  if (note_get(nrpn_get(KYRP_ACTIVE_ST))) {
    note_off(KYRN_KEY_ST);
    nrpn_set(KYRP_ACTIVE_ST, note);
    cwarbiter_change_over = 1;
    return;
  }
  /* case 5 - preempt active note which is silent */
  nrpn_set(KYRP_ACTIVE_ST, note);
  if (note_get(note) != note_get(KYRN_KEY_ST))
    note_toggle(KYRN_KEY_ST);
  return;
}

static void cwarbiter_release(const int note) {
  nrpn_set(KYRP_ACTIVE_ST, KYRN_ST_NONE);
}

static void cwarbiter_sample(void) {
  /* if we preempted a sounding note we have to allow one sample of sidetone off
   * to trigger the fall ramp in the audio graph */
  if (cwarbiter_change_over && ++cwarbiter_change_over > 32) {
    cwarbiter_change_over = 0;
    if (note_get(nrpn_get(KYRP_ACTIVE_ST)) != note_get(KYRN_KEY_ST))
      note_toggle(KYRN_KEY_ST);
  }
}

static void cwarbiter_setup(void) {
  note_listen(KYRN_ST_S_KEY, cwarbiter_any_st);
  note_listen(KYRN_ST_S_KEY2, cwarbiter_any_st);
  note_listen(KYRN_ST_S_KEY3, cwarbiter_any_st);
  note_listen(KYRN_ST_PAD, cwarbiter_any_st);
  note_listen(KYRN_ST_PAD2, cwarbiter_any_st);
  note_listen(KYRN_ST_PAD3, cwarbiter_any_st);
  note_listen(KYRN_ST_TEXT, cwarbiter_any_st);
  note_listen(KYRN_ST_TEXT2, cwarbiter_any_st);
  note_listen(KYRN_ST_TUNE, cwarbiter_any_st);
  note_listen(KYRN_PTT_ST, cwarbiter_release);
  every_sample(cwarbiter_sample);
  nrpn_set(KYRP_ACTIVE_ST, KYRN_ST_NONE);
}

static void cwarbiter_loop(void) {
}
