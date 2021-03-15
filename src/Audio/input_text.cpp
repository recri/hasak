
#include <Arduino.h>
#include "input_text.h"
#include "sample_value.h"

void AudioInputText::update(void)
{
  if (block) {
    while (n > 0) {
      if (element < 0) {
	*wptr++ = bool2fix(0);
	n -= 1;
	element += 1;
      } else if (element > 0) {
	*wptr++ = bool2fix(1);
	n -= 1;
	element -= 1;
      } else {
	element = recv();
	if (element == 0)
	  break;
      }
    }
    if (wptr == &block->data[0]) {
      // empty block, nothing to put into it
      return;
    }
    while (n > 0) {
      *wptr++ = bool2fix(0);
      n -= 1;
    }
    transmit(block);
    release(block);
  }
  block = allocate();
  if (block) {
    wptr = &block->data[0];
    n = AUDIO_BLOCK_SAMPLES;
    while (n > 0) {
      if (element < 0) {
	*wptr++ = bool2fix(0);
	n -= 1;
	element += 1;
      } else if (element > 0) {
	*wptr++ = bool2fix(1);
	n -= 1;
	element -= 1;
      } else {
	element = recv();
	if (element == 0)
	  break;
      }
    }
  }
}
