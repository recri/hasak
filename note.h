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

struct note_after {
  elapsedSamples timer;
  int8_t note, value;
  struct note_after *next;
};
typedef struct note_after note_after_t;

static note_listener_t *note_lhead[KYR_N_NOTE];
static note_listener_t *note_lfree;
static note_listener_t note_lnode[N_NOTE_LISTENERS];

static note_after_t *note_ahead;
static note_after_t *note_afree;
static note_after_t note_anode[N_NOTE_AFTERS];

static int note_listener_used = 0;
static int note_after_used = 0;

static void note_setup(void) {
  /* set the listener list heads to empty */
  for (int i = 0; i < KYR_N_NOTE; i += 1)
    note_lhead[i] = NULL;
  /* set the listener free list to empty */
  note_lfree = NULL;
  /* add the listener nodes to the free list */
  for (int i = 0; i < N_NOTE_LISTENERS; i += 1) {
    note_lnode[i].next = note_lfree;
    note_lfree = &note_lnode[i];
  }
  /* set the after list head to empty */
  note_ahead = NULL;
  /* set the after free list to empty */
  note_afree = NULL;
  /* add the after nodes to the free list */
  for (int i = 0; i < N_NOTE_AFTERS; i += 1) {
    note_anode[i].next = note_afree;
    note_afree = &note_anode[i];
  }
}

static void note_invalid(const int16_t note, const char *who) {
  Serial.printf("invalid note %d in %s\n", note, who);
}

static int note_valid(const int16_t note) { return note >= 0 && note < KYR_N_NOTE; }

/* add a listener to note to the note listener list */
static void note_listen(int note, void (*listener)(int note)) {
  if ( ! note_valid(note)) {
    note_invalid(note, "note_listen");
    return;
  }
  if (note_lfree == NULL) {
    Serial.printf("note_listen(%d, ...) no free lnode\n", note);
    return;
  }
  note_listener_t *lp = note_lfree;
  note_lfree = lp->next;
  lp->listener = listener;
  lp->next = note_lhead[note];
  note_lhead[note] = lp;
  note_listener_used += 1;
}

/* add an after node to the after list */
static void note_after(uint32_t samples, int8_t note, int8_t value) {
  if (samples == 0) {		/* has already expired */
    note_set(note, value);
    return;
  }
  if (note_afree == NULL) {	/* free list is empty */
    Serial.printf("note_after(%d, ...) no free anode\n", note);
    return;
  }
  note_after_t *ap = note_afree; /* allocate a node */
  note_afree = ap->next;	 /* update the free list */
  ap->timer = -samples;		 /* initialize the timer */
  ap->note = note;
  ap->value = value;
  ap->next = note_ahead;		/* link the node into the after list */
  note_ahead = ap;
}
      
static void note_loop(void) {
  /* scan the after list */
  for (note_after_t **app = &note_ahead; *app != NULL; ) {
    note_after_t *ap = *app;
    if ((int)(ap->timer) < 0) {	/* timer not expired */
      app = &ap->next;		/* step to next in list */
    } else {			/* timer expired */
      note_set(ap->note, ap->value); /* perform the note */
      *app = ap->next;		     /* update the after list */
      ap->next = note_afree;	     /* free the expired node */
      note_afree = ap;
      note_after_used -= 1;	/* keep the count */
    }
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
    for (note_listener_t *lp = note_lhead[note]; lp != NULL; lp = lp->next) lp->listener(note);
  } else {
    note_invalid(note, "note_set");
  }
}

static void note_toggle(const int16_t note) { note_set(note, hasak.notes[note] ^ 1); } 

static void note_on(const int16_t note) { note_set(note, 1); } 

static void note_off(const int16_t note) { note_set(note, 0); } 
