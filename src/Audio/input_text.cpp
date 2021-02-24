
#include <Arduino.h>
#include "input_text.h"
#include "sample_value.h"

void AudioInputText::update(void)
{
  if (block) {
    if (element == 0)
      element = recv();
    while (n > 0 && element != 0) {
      while (n > 0 && element < 0) {
	*wptr++ = bool2fix(0);
	n -= 1;
	element += 1;
      }
      while (n > 0 && element > 0) {
	*wptr++ = bool2fix(1);
	n -= 1;
	element -= 1;
      }
      if (element == 0)
	element = recv();
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
    if (element == 0) 
      element = recv();
    while (n > 0 && element != 0) {
      while (n > 0 && element < 0) {
	*wptr++ = bool2fix(0);
	n -= 1;
	element += 1;
      }
      while (n > 0 && element > 0) {
	*wptr++ = bool2fix(1);
	n -= 1;
	element -= 1;
      }
      if (element == 0)
	element = recv();
    }
  }
}
