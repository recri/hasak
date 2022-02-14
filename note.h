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

/* forward ref */
static void note_set(const int16_t note, const int8_t value);

#define N_NOTE_LISTENERS 512
#define N_NOTE_AFTERS 512

struct note_listener {
  void (*listener)(int note);
  struct note_listener *next;
};
typedef struct note_listener note_listener_t;

typedef struct {
  elapsedSamples start;
  uint32_t finish;
  int8_t note, value;
} note_after_t;

static note_listener_t *note_head[KYR_N_NOTE];
static note_listener_t *note_free;
static note_listener_t note_node[N_NOTE_LISTENERS];

static note_after_t note_afters[N_NOTE_AFTERS];

static int note_listener_used = 0;
static int note_after_used = 0;

static void note_setup(void) {
  for (int i = 0; i < KYR_N_NOTE; i += 1)
    note_head[i] = NULL;
  note_free = NULL;
  for (int i = 0; i < N_NOTE_LISTENERS; i += 1) {
    note_node[i].next = note_free;
    note_free = &note_node[i];
  }
  for (int i = 0; i < N_NOTE_AFTERS; i += 1) note_afters[i].finish = 0;
}

static void note_invalid(const int16_t note, const char *who) {
  Serial.printf("invalid note %d in %s\n", note, who);
}

static int note_valid(const int16_t note) { return note >= 0 && note < KYR_N_NOTE; }

static void note_listen(int note, void (*listener)(int note)) {
  if ( ! note_valid(note)) {
    note_invalid(note, "note_listen");
    return;
  }
  if (note_free == NULL) {
    Serial.printf("note_listen(%d, ...) no free listeners\n", note);
    return;
  }
  note_listener_t *lp = note_free;
  note_free = lp->next;
  lp->listener = listener;
  lp->next = note_head[note];
  note_head[note] = lp;
  note_listener_used += 1;
}

static void note_after(uint32_t samples, int8_t note, int8_t value) {
  if (samples == 0) {
    note_set(note, value);
    return;
  }
  for (int i = 0; i < N_NOTE_AFTERS; i += 1)
    if (note_afters[i].finish == 0) {
      note_afters[i].start = 0;
      note_afters[i].finish = samples;
      note_afters[i].note = note;
      note_afters[i].value = value;
      note_after_used += 1;
      return;
    }
  Serial.printf("note_after(%d, %d, %d) no free afters\n", samples, note, value);
}
      
static void note_loop(void) {
  for (int i = 0; i < N_NOTE_AFTERS; i += 1)
    if (note_afters[i].finish != 0 && note_afters[i].start >= note_afters[i].finish) {
      note_afters[i].finish = 0;
      note_after_used -= 1;
      note_set(note_afters[i].note, note_afters[i].value);
    }
}

static int note_get(const int16_t note) { 
  if ( note_valid(note)) return hasak.notes[note];
  note_invalid(note, "note_get");
  return -1;
}

static void note_set(const int16_t note, const int8_t value) {
  if ( note_valid(note) ) {
    hasak.notes[note] = value;
    Serial.printf("note_set(%d, %d)\n", note, value);
    for (note_listener_t *lp = note_head[note]; lp != NULL; lp = lp->next) lp->listener(note);
  } else {
    note_invalid(note, "note_set");
  }
}

static void note_toggle(const int16_t note) { note_set(note, hasak.notes[note] ^ 1); } 

static void note_on(const int16_t note) { note_set(note, 1); } 

static void note_off(const int16_t note) { note_set(note, 0); } 
