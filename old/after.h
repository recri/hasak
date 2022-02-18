struct note_after {
  elapsedSamples timer;
  int8_t note, value;
  struct note_after *next;
};
typedef struct note_after note_after_t;

static note_after_t *note_ahead;
static note_after_t *note_afree;
static note_after_t note_anode[N_NOTE_AFTERS];

static int note_after_used = 0;

static void after_setup(void) {
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

