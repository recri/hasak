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
#ifndef _keyer_dl1ycf_h_
#define _keyer_dl1ycf_h_

#include "keyer_generic.h"

//
// a version of Christoph's iambic keyer running inside
// the hasak keyer.  Make Christoph's code work without
// modification?  Let's try.
//
class KeyerDl1ycf : public KeyerGeneric {
 public:
 KeyerDl1ycf() : KeyerGeneric() { }
  //
  // keyer state machine: the states
  //
  enum KSTAT {
    CHECK=0,        // idle
    SENDDOT,        // sending a dot
    SENDDASH,       // sending a dash
    DOTDELAY,       // pause following a dot
    DASHDELAY,      // pause following a dash
    STARTDOT,       // aquire PTT and start dot
    STARTDASH,      // aquire PTT and start dash
    STARTSTRAIGHT,  // aquire PTT and send "key-down" message
    SENDSTRAIGHT,   // wait for releasing the straight key and send "key-up" message
    SNDCHAR_PTT,    // aquire PTT, key-down
    SNDCHAR_ELE,    // wait until end of element (dot or dash), key-up
    SNDCHAR_DELAY   // wait until end of delay (inter-element or inter-word)
  } keyer_state=CHECK;

  ///////////////////////////////////////
  //
  // This is the Keyer state machine
  //
  ///////////////////////////////////////

  void keyer_state_machine() {
    int i;                        // general counter variable
    uint8_t byte;                 // general one-byte variable
    int dotlen;                   // length of dot (msec)
    int dashlen;                  // length of dash (msec)
    int plen;                     // length of delay between dits/dahs
    int clen;                     // inter-character delay in addition to inter-element delay
    int wlen;                     // inter-word delay in addition to inter-character delay
    int hang;                     // PTT tail hang time

    static unsigned long straight_pressed;  // for timing straight key signals

    //
    // It is a little overkill to determine all these values at each heart beat,
    // but the uC should have enough horse-power to handle this
    //


    //
    // Standard Morse code timing
    //
    dotlen =1200 / myspeed;          // dot length in milli-seconds
    dashlen=(3*Ratio*dotlen)/50;     // dash length
    plen   =dotlen;                  // inter-element pause
    clen   =2*dotlen;                // inter-character pause = 3 dotlen, one already done (inter-element)
    wlen   =4*dotlen;                // inter-word pause = 7 dotlen, 3 already done (inter-character)

    //
    // Farnsworth timing: strecht inter-character and inter-word pauses
    //
    if (Farnsworth > 10 && Farnsworth < myspeed) {
      i=3158/Farnsworth -(31*dotlen)/19;
      clen=3*i - dotlen;                    // stretched inter-character pause
      wlen=7*i - clen;                      // stretched inter-word pause
    }

    //
    // If both Weighting and Compensation is used, the
    // dit/dah length depends on the order of the corrections
    // applied. Here first Weighting and then Compensation
    // is done, but note one should not use these options
    // at the same time anyway.
    //
    if (Weight != 50) {
      i = ((Weight-50)*dotlen)/50;
      dotlen += i;
      dashlen +=i;
      plen -= i;
    }
    if (Compensation != 0) {
      dotlen  += Compensation;
      dashlen += Compensation;
      plen    -= Compensation;
    }

    //
    // PTT hang time from PTTtail.
    // If it is zero, or PTT not enabled, use hang bits
    // Note: WK2.3 changed hang times
    //
    if (Tail != 0 && PTT_ENABLED) {
      hang=10*Tail;
    } else {
      switch (HANGBITS) {
      case 0: hang =  8*dotlen; break;     // word space + 1 dot
      case 1: hang =  9*dotlen; break;     // word space + 2 dots
      case 2: hang = 11*dotlen; break;     // word space + 4 dots
      case 3: hang = 15*dotlen; break;     // word space + 8 dots
      }
    }

    //
    // Abort sending buffered characters (and clear the buffer)
    // if a paddle is hit. Set "breakin" flag
    //
    if ((eff_kdash || eff_kdot || straight) && (keyer_state >= SNDCHAR_PTT)) {
      breakin=1;
      clearbuf();
      keyer_state=CHECK;
      wait=actual+hang;
    }
    switch (keyer_state) {
    case CHECK:
      // reset number of elements sent
      num_elements=0;
      // wait = time when PTT is switched off
      if (actual >= wait) ptt_off();
      if (collpos > 0 && actual > last + 2 * dotlen) {
        // a morse code pattern has been entered and the character is complete
        // echo it in ASCII on the display and on the serial line
        collecting |= 1 << collpos;
        for (i=33; i<= 90; i++) {
          if (collecting == morse[i-33]) {
	    if (PADDLE_ECHO && hostmode) ToHost(i);
	    break;
          }
        }
        collecting=0;
        collpos=0;
      }
      //
      // if there is a long pause, send one space (no matter how long the pause is)
      //
      if (collpos == 0 && sentspace == 0 && actual > last + 6*dotlen) {
	if (PADDLE_ECHO && hostmode) ToHost(32);
	sentspace=1;
      }
      //
      // The dash, dot and straight key contact may be closed at
      // the same time. Priority here is straight > dot > dash
      //
      if (eff_kdash) {
        wait=actual;
        sentspace=0;
        keyer_state=STARTDASH;
        collecting |= (1 << collpos++);
        if (!ptt_stat && PTT_ENABLED) {
          ptt_on();
          wait=actual+LeadIn*10;
        }
      }
      if (eff_kdot) {

        keyer_state=STARTDOT;
        collpos++;
        wait=actual;
        sentspace=0;
        if (!ptt_stat && PTT_ENABLED) {
          ptt_on();
          wait=actual+LeadIn*10;
        }
      }
      if (straight) {
        sentspace=0;
        keyer_state=STARTSTRAIGHT;
        wait=actual;
        if (!ptt_stat && PTT_ENABLED) {
          ptt_on();
          wait=actual+LeadIn*10;
        }
      }
      break;
    case STARTDOT:
      // wait = end of PTT lead-in time
      if (actual >= wait) {
        keyer_state=SENDDOT;
        memdash=0;
        dash_held=eff_kdash;
        wait=actual+dotlen;
        keydown();
        num_elements++;
      }
      break;
    case STARTDASH:
      // wait = end of PTT lead-in time
      if (actual >= wait) {
        keyer_state=SENDDASH;
        memdot=0;
        dot_held=eff_kdot;
        wait=actual+dashlen;
        keydown();
        num_elements++;
      }
      break;
    case STARTSTRAIGHT:
      // wait = end of PTT lead-in time
      memdot=memdash=dot_held=dash_held=0;
      if (actual >= wait) {
        if (straight) {
          keyer_state=SENDSTRAIGHT;
          keydown();
          straight_pressed=actual;
        } else {
          // key-up during PTT lead-in time: do not send key-down but
          // init hang time
          wait=actual+hang;
          keyer_state=CHECK;
        }
        break;
      }
    case SENDDOT:
      // wait = end of the dot
      if (actual >= wait) {
        last=actual;
        keyup();
        wait=wait+plen;
        keyer_state=DOTDELAY;
      }
      break;
    case SENDSTRAIGHT:
      //
      // do nothing until contact opens
      if (!straight) {
        last=actual;
        keyup();
        // determine length of elements and treat it as a dash if long enough
        if (actual  > straight_pressed + 2*plen) {
          collecting |= (1 << collpos++);
        } else {
          collpos++;
        }
        wait=actual+hang;
        keyer_state=CHECK;
      }
      break;
    case DOTDELAY:
      // wait = end of the pause following a dot
      if (actual >= wait) {
        if (!eff_kdot && !eff_kdash && IAMBIC_A) dash_held=0;
        if (memdash || eff_kdash || dash_held) {
          collecting |= (1 << collpos++);
          keyer_state=STARTDASH;
        } else if (eff_kdot) {
          collpos++;
          keyer_state=STARTDOT;
        } else {
          keyer_state=CHECK;
          wait=actual+hang-plen;
        }
      }
      break;
    case SENDDASH:
      // wait = end of the dash
      if (actual >= wait) {
        last=actual;
        keyup();
        wait=wait+plen;
        keyer_state=DASHDELAY;
      }
      break;
    case DASHDELAY:
      // wait = end of the pause following the dash
      if (actual > wait) {
        if (!eff_kdot && !eff_kdash && IAMBIC_A) dot_held=0;
        if (memdot || eff_kdot || dot_held) {
          collpos++;
          keyer_state=STARTDOT;
        } else if (eff_kdash) {
          collecting |= (1 << collpos++);
          keyer_state=STARTDASH;
        } else {
          keyer_state=CHECK;
          wait=actual+hang-plen;
        }
      }
      break;
    case SNDCHAR_PTT:
      // wait = end of PTT lead-in wait
      if (actual > wait) {
        keyer_state=SNDCHAR_ELE;
        keydown();
        wait=actual + ((sending & 0x01) ? dashlen : dotlen);
        sending = (sending >> 1) & 0x7F;
      }
      break;
    case SNDCHAR_ELE:
      // wait = end of the current element (dot or dash)
      if (actual >= wait) {
        keyup();
        wait=actual+plen;
        if (sending == 1) {
          if (!prosign) wait += clen;
          prosign=0;
        }
        keyer_state=SNDCHAR_DELAY;
      }
      break;
    case SNDCHAR_DELAY:
      // wait = end  of pause (inter-element or inter-word)
      if (actual >= wait) {
        if (sending == 1) {
          // character sent completely
          keyer_state=CHECK;
          if (bufcnt > 0) wait=actual+3*dotlen; // when buffer empty, go RX immediately
        } else {
          keydown();
          keyer_state=SNDCHAR_ELE;
          wait=actual + ((sending & 0x01) ? dashlen : dotlen);
          sending = (sending >> 1) & 0x7F;
        }
      }
      break;
    }
    //
    // If keyer is idle and buffered characters available, transfer next one to "sending"
    //
    if (bufcnt > 0 && keyer_state==CHECK && !pausing) {
      //
      // transfer next character to "sending"
      //
      byte=FromBuffer();
      switch (byte) {
      case PROSIGN:
        prosign=1;
        break;
      case BUFNOP:
        break;
      case 32:  // space
        // send inter-word distance
        if (SERIAL_ECHO) ToHost(32);  // echo
        sending=1;
        wait=actual + wlen;
        keyer_state=SNDCHAR_DELAY;
        break;
      default:
        if (byte >='a' && byte <= 'z') byte -= 32;  // convert to lower case
        if (byte > 32 && byte < 'Z') {
          if (SERIAL_ECHO) ToHost(byte);  // echo
          sending=morse[byte-33];
          if (sending != 1) {
            wait=actual;  // no lead-in wait by default
            if (!ptt_stat && PTT_ENABLED) {
              ptt_on();
              wait=actual+LeadIn*10;
            }
            keyer_state=SNDCHAR_PTT;
          }
        }
        break;
      }
    }
  }
  
  //
  // the contents of the EEPROM
  //
  uint8_t ModeRegister;	/*  = EEPROM.read( 1) */
  uint8_t Speed;	/* = EEPROM.read( 2) */
  uint8_t Sidetone;	/* = EEPROM.read( 3) */
  uint8_t Weight;	/* = EEPROM.read( 4);*/
  uint8_t LeadIn;	/* = EEPROM.read( 5); // ptt parameter */
  uint8_t Tail;		/* = EEPROM.read( 6); // ptt parameter*/
  uint8_t MinWPM;	/* = EEPROM.read( 7); // pot read parameter */
  uint8_t WPMrange;	/* = EEPROM.read( 8); // pot read parameter */
  uint8_t Extension;	/* = EEPROM.read( 9); // ? */
  uint8_t Compensation; /* = EEPROM.read(10); */
  uint8_t Farnsworth;	/* = EEPROM.read(11); */
  uint8_t PaddlePoint;	/* = EEPROM.read(12); // debounce period */
  uint8_t Ratio;	/* = EEPROM.read(13); */
  uint8_t PinConfig;	/* = EEPROM.read(14); */

  //
  // emulation
  //
  const int bufcnt = 0;
  int FromBuffer() { return bufcnt; }
  //
  // Macros to read the ModeRegister
  //
  int PADDLE_SWAP() { return (ModeRegister & 0x08); }
  int PADDLE_ECHO() { return (ModeRegister & 0x40); }
  int SERIAL_ECHO() { return (ModeRegister & 0x04); }

  int IAMBIC_A() { return ((ModeRegister & 0x30) == 0x10); }
  int IAMBIC_B() { return ((ModeRegister & 0x30) == 0x00); }
  int BUGMODE() { return ((ModeRegister & 0x30) == 0x30); }
  int ULTIMATIC() { return ((ModeRegister & 0x30) == 0x20); }

  int clock(int left, int right, int ticks) {
  }
};
#endif
