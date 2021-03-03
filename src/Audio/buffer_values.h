#ifndef buffer_values_h_
#define buffer_values_h_

/*
** convert buffer values to run lengths
**
** the majority of logical buffers should
** be all zeros, all ones, or a sequence
** of ones followed by a sequence of zeros,
** or a sequence of zeros followed by a
** sequence of ones.
**
** a dit at 100WPM is 12 ms long, that's
** 529.2 samples at 44100, 576 samples at
** 48000, more than 4 buffers of 128 samples.
**
** So the majority of buffers should be
** entirely specified by the first run
** length, negative for an initial run of 
** zeros, and positive for an initial run
** of ones.
**
** These routines convert buffers of zeros
** and ones into run lengths, verify the
** conversion, and record the variety found.
**
** The exceptions to this rule will be found
** in the logical buffers coming from mechanical
** switches, which will bounce on making or 
** breaking contact.
*/
#include <Arduino.h>
#include <AudioStream.h>
#include "sample_values.h

// return the number of runs in the buffer
// maintain statistics of the distribution

int count_run_lengths(audio_block_t *block) {
  int r = 0, runs[AUDIO_BLOCK_SAMPLES+1];
  if ( ! block) {
    runs[r] = -AUDIO_BLOCK_SAMPLES;
  } else {
    int16_t zero = bool2fix(0), one = bool2fix(1);
    int16_t *bp = block->data, *end = bp+AUDIO_BLOCK_SAMPLES;
    if (*bp == zero)
      runs[r] = -1;
    else if (*bp == one)
      runs[r] = +1;
    else
      return 0;
    bp += 1
    while (bp < end) {
      if (*bp == zero)
	if (runs[r] < 0) runs[r] -= 1;
	else runs[++r] = -1;
      else if (*bp == one)
	if (runs[r] > 0) runs[r] += 1;
	else runs[++r] = +1;
      else
	return 0;
      bp += 1;
    }
  }
  return ++r;
}
#endif
