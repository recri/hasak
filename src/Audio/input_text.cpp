
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
// Morse code for ASCII characters 33 - 90, from ITU-R M.1677-1
// read from bit0 to bit7
// 0 = dot, 1 = dash
// highest bit set indicated end-of-character (not to be sent as a dash!)
// borrowed from Christoph's keyer, along with the decoding code
// https://github.com/dl1ycf/TeensyWinkeyEmulator
static const unsigned char AudioInputText::morse[58] = {
   0x01,     //33 !    not in ITU-R M.1677-1
   0x52,     //34 "    .-..-.
   0x01,     //35 #    not in ITU-R M.1677-1
   0x01,     //36 $    not in ITU-R M.1677-1
   0x01,     //37 %    not in ITU-R M.1677-1
   0x01,     //38 &    not in ITU-R M.1677-1
   0x5e,     //39 '    .----.    
   0x36,     //40 (    -.--.
   0x6d,     //41 )    -.--.-
   0x01,     //42 *    not in ITU-R M.1677-1
   0x2a,     //43 +    .-.-.
   0x73,     //44 ,    --..--
   0x61,     //45 -    -....-
   0x6A,     //46 .    .-.-.-
   0x29,     //47 /    -..-.
   0x3f,     //48 0    -----
   0x3e,     //49 1    .----
   0x3c,     //50 2    ..---
   0x38,     //51 3    ...--
   0x30,     //52 4    ....-
   0x20,     //53 5    .....
   0x21,     //54 6    -....
   0x23,     //55 7    --...
   0x27,     //56 8    ---..
   0x2f,     //57 9    ----.
   0x78,     //58 :    ---...
   0x01,     //59 ;    not in ITU-R M.1677-1
   0x01,     //60 <    not in ITU-R M.1677-1
   0x31,     //61 =    -...-
   0x01,     //62 >    not in ITU-R M.1677-1
   0x4c,     //63 ?    ..--..
   0x56,     //64 @    .--.-.
   0x06,     //65 A    .-
   0x11,     //66 B    .---
   0x15,     //67 C    -.-.
   0x09,     //68 D    -..
   0x02,     //69 E    .
   0x14,     //70 F    ..-.
   0x0b,     //71 G    --.
   0x10,     //72 H    ....
   0x04,     //73 I    ..
   0x1e,     //74 J    .---
   0x0d,     //75 K    -.-
   0x12,     //76 L    .-..
   0x07,     //77 M    -- 
   0x05,     //78 N    -.
   0x0f,     //79 O    ---
   0x16,     //80 P    .--.
   0x1b,     //81 Q    --.-
   0x0a,     //82 R    .-.
   0x08,     //83 S    ...
   0x03,     //84 T    -
   0x0c,     //85 U    ..-
   0x18,     //86 V    ...-
   0x0e,     //87 W    .--
   0x19,     //88 X    -..-
   0x1d,     //89 Y    -.--
   0x13      //90 Z    --..
};
