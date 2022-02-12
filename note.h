static long note_changed = 0;

static void note_setup(void) {
}

static void note_loop(void) {
  if (note_changed) {
    for (int note = 0; note < KYRC_NNOTE; note += 1) {
      if (note_changed & (1L<<note)) {
	note_changed ^= (1L<<note);
      }
    }
  }
}

static void note_toggle(const int16_t note) {
  hasak.notes[note] ^= 1;
  note_changed |= (1L<<note);
}

