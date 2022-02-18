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

#ifdef __cplusplus
extern "C" {
#endif

static int note_invalid(const int note, const char *who) {
  Serial.printf("invalid note %d in %s\n", note, who);
  return -1;
}

static int note_is_valid(int note) { return midi.note_is_valid(note); }
  
static void note_get(int note) { 
  return note_is_valid(note) ? midi.note_get(note) : note_invalid(note, "note_get");
}

static void note_set(int note, int value) { 
  if (note_is_valid(note))
    midi.note_set(note, value);
  else
    note_invalid(note, "note_set");
}

/* define a note */
static void note_define(const int note, const int value, 
			const int input_enable, const int output_enable, 
			const int echo_enable, const int read_only) {
  if (note_is_valid(note)) {
    midi.note_flags_set(note, midi_flags_encode(input_enable, output_enable, echo_enable, read_only));
    nprn_set(note, value);
  } else
    note_invalid(note, "nrpn_define");
}

static void note_toggle(const int note) { note_set(note, note_get(note) ^ 1); } 

static void note_on(const int note) { note_set(note, 1); } 

static void note_off(const int note) { note_set(note, 0); } 

static void note_setup(void) {
}
static void note_loop(void) {
}

#ifdef __cplusplus
}
#endif
