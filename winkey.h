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
#if ! defined(KYRC_ENABLE_WINKEY)
void winkey_setup(void) { }
void winkey_loop(void) { }
#else
////////////////////////////////////////////////////////////////////////////////////////
//
// Teensy keyer with audio (WinKey emulator)
//
// (C) Christoph van Wüllen, DL1YCF, 2020/2021
//
// Designed for the Teensy 4, using the "Serial+MIDI+Audio" programming model
//
//
//
// WinKey state machine: the states
// The numerical (enum) values ADMIN ... BUFNOP must agree with the
// winkey command bytes.
//
#define WKVERSION 21  // This version is returned to the host


enum WKSTAT {
  ADMIN       =0x00,
  SIDETONE    =0x01,
  WKSPEED     =0x02,
  WEIGHT      =0x03,
  PTT         =0x04,
  POTSET      =0x05,
  PAUSE       =0x06,
  GETPOT      =0x07,
  BACKSPACE   =0x08,
  PINCONFIG   =0x09,
  CLEAR       =0x0a,
  TUNE        =0x0b,
  HSCW        =0x0c,
  FARNS       =0x0d,
  WK2MODE     =0x0e,
  LOADDEF     =0x0f,
  EXTENSION   =0x10,
  KEYCOMP     =0x11,
  PADSW       =0x12,
  NULLCMD     =0x13,
  SOFTPAD     =0x14,
  WKSTAT      =0x15,
  POINTER     =0x16,
  RATIO       =0x17,
  SETPTT      =0x18,
  KEYBUF      =0x19,
  WAIT        =0x1a,
  PROSIGN     =0x1b,
  BUFSPD      =0x1c,
  HSCWSPD     =0x1d,
  CANCELSPD   =0x1e,
  BUFNOP      =0x1f,
  FREE,
  SWALLOW,
  XECHO,
  WRPROM,
  RDPROM,
  MESSAGE,
  POINTER_1,
  POINTER_2,
  POINTER_3
} winkey_state=FREE;

//
// Winkeyer properties all reduced to this enumeration
// 
typedef enum {
  WK_MODE_REGISTER = 0,
  WK_SPEED,
  WK_SIDETONE,
  WK_WEIGHT,
  WK_PTT_LEAD_IN,
  WK_PTT_TAIL,
  WK_MIN_WPM,
  WK_WPM_RANGE,
  WK_EXTENSION,
  WK_COMPENSATION,
  WK_FARNSWORTH,
  WK_PADDLE_POINT,
  WK_RATIO,
  WK_PIN_CONFIG
} winkey_property_t;


//
// Winkey variable abstraction, 14 bytes of state translated to/from nrpn equivalents
//
//
// Macros to read the ModeRegister
//
#define PADDLE_SWAP(ModeRegister)   ((ModeRegister & 0x08) != 0)
#define PADDLE_ECHO(ModeRegister)   ((ModeRegister & 0x40) != 0)
#define SERIAL_ECHO(ModeRegister)   ((ModeRegister & 0x04) != 0)
#define PADDLE_MODE(ModeRegister)   ((ModeRegister & 0x30)>>4)

#define IAMBIC_A(ModeRegister)      ((ModeRegister & 0x30) == 0x10)
#define IAMBIC_B(ModeRegister)      ((ModeRegister & 0x30) == 0x00)
#define BUGMODE(ModeRegister)       ((ModeRegister & 0x30) == 0x30)
#define ULTIMATIC(ModeRegister)     ((ModeRegister & 0x30) == 0x20)

//
// Macros to read PinConfig
//
#define SIDETONE_ENABLED(PinConfig) (PinConfig & 0x02)
#define PTT_ENABLED(PinConfig)      (PinConfig & 0x01)
#define HANGBITS(PinConfig)         ((PinConfig & 0x30) >> 4)

static void winkey_set(winkey_property_t p, int v) {
  switch (p) {
  case WK_MODE_REGISTER:
    //static uint8_t ModeRegister=0x10;       // no echos, no swap, Iambic-A by default
    // mode register: serial echo / paddle echo / swap / paddle mode
    // bits used in ModeRegiser:
    //   b6:    echo characters entered by paddle or straight key (to serial line)
    //   b4/5:  Paddle mode (00 = Iambic-B, 01 = Iambic-A, 10 = Ultimatic, 11 = Bugmode)
    //   b3:    swap paddles
    //   b2:    echo characters received from the serial line as they are transmitted
    //
    if (get_nrpn(KYRP_PAD_SWAP) != PADDLE_SWAP(v))
      nrpn_set(KYRP_PAD_SWAP, PADDLE_SWAP(v) != 0);
    if (get_nrpn(KYRP_PAD_KEYER) != KYRV_KEYER_VK6PH)
      nrpn_set(KYRP_PAD_KEYER, KYRV_KEYER_VK6PH);
    if (ULTIMATIC(v)) {
      if (get_nrpn(KYRP_PAD_ADAPT) != KYRV_ADAPT_ULTIMATIC)
	nrpn_set(KYRP_PAD_ADAPT, KYRV_ADAPT_ULTIMATIC);
      if (get_nrpn(KYRP_PAD_MODE) != KYRV_MODE_A)
	nrpn_set(KYRP_PAD_MODE, KYRV_MODE_A);
    } else {
      if (get_nrpn(KYRP_PAD_ADAPT) != KYRV_ADAPT_NORMAL)
	nrpn_set(KYRP_PAD_ADAPT, KYRV_ADAPT_NORMAL);
      switch (PADDLE_MODE(v)) {
      case 0:
	if (get_nrpn(KYRP_PAD_MODE) != KYRV_MODE_B)
	  nrpn_set(KYRP_PAD_MODE, KYRV_MODE_B);
	break;
      case 1: 
	if (get_nrpn(KYRP_PAD_MODE) != KYRV_MODE_A)
	  nrpn_set(KYRP_PAD_MODE, KYRV_MODE_A);
	break;
      case 3:
	if (get_nrpn(KYRP_PAD_MODE) != KYRV_MODE_S)
	  nrpn_set(KYRP_PAD_MODE, KYRV_MODE_S);
	break;
      }
    }
    if (PADDLE_ECHO(v)) // do something FIX.ME
      ;
    if (SERIAL_ECHO(v)) // do something FIX.ME
      ;
    break;
  case WK_SPEED:
    //static uint8_t Speed=0;                 // CW speed (zero means: use speed pot)
    // speed: 0 means use speed pot
    // Speed=byte;
    if (get_nrpn(KYRP_SPEED) != v)
      nrpn_set(KYRP_SPEED, v);	// FIX.ME, what if no speed pot and v == 0
    break;
  case WK_SIDETONE:
    //static uint8_t Sidetone=5;              // 800 Hz
    // 4000/(byte&15) hz, default 5 => 800Hz  // Sidetone=byte;
    // myfreq=4000/(Sidetone & 0x0F);
    // sidetonefrequency(myfreq);     
    v = 4000/(v & 0x0F);
    if (get_nrpn(KYRP_TONE) != v)
      nrpn_set(KYRP_TONE, v);
    break;
  case WK_WEIGHT:
    //static uint8_t Weight=50;               // used to modify dit/dah length
    // max(10, min(90, byte)), default 50
    // if (byte < 10) byte=10;
    // if (byte > 90) byte=90;
    // Weight=byte;
    v = max(10, min(90, v));
    if (get_nrpn(KYRP_WEIGHT) != v)
      nrpn_set(KYRP_WEIGHT, v);			// FIX.ME, this is a wider range of values than ctrlr implements
    break;
  case WK_PTT_LEAD_IN:
    //static uint8_t LeadIn=15;               // PTT Lead-in time (in units of 10 ms)
    // unit == 10ms
    v = 10*v;
    if (get_nrpn(KYRP_HEAD_TIME) != v)
      nrpn_set(KYRP_HEAD_TIME, v);
    break;
  case WK_PTT_TAIL:
    //static uint8_t Tail=0;                  // PTT tail (in 10 ms), zero means "use hang bits"
    // unit == 10ms
    v = 10*v;
    if (get_nrpn(KYRP_TAIL_TIME) != v)
      nrpn_set(KYRP_TAIL_TIME, v);
    break;
  case WK_MIN_WPM:
    //static uint8_t MinWPM=5;                // CW speed when speed pot maximally CCW
    // FIX.ME - how to implement with no speed pot
    break;
  case WK_WPM_RANGE:
    //static uint8_t WPMrange=25;             // CW speed range for SpeedPot
    // range of speed pot
    // range = min(31, range);
    // FIX.ME - how to implement with no speed pot
    break;
  case WK_EXTENSION:
    //static uint8_t Extension=0;             // ignored
    // FIX.ME - stretch the first element of a transmission sequence to compensate
    // for lousy transmitter ptt timing
    break;
  case WK_COMPENSATION:
    //static uint8_t Compensation=0;          // Used to modify dit/dah lengths
    v = 10*v;
    if (get_nrpn(KYRP_COMP) != v)
      nrpn_set(KYRP_COMP, v);
    break;
  case WK_FARNSWORTH:
    //static uint8_t Farnsworth=10;           // Farnsworth speed (10 means: no Farnsworth)
    // if (byte < 10) byte=10;
    // if (byte > 99) byte=99;
    // Farnsworth=byte;
    v = max(10, min(99, v));
    if (get_nrpn(KYRP_FARNS) != v)
      nrpn_set(KYRP_FARNS, v);
    break;
  case WK_PADDLE_POINT:
    //static uint8_t PaddlePoint;             // debounce refractory period
    v = 10*v;				      // convert ms to tenth ms
    if (get_nrpn(KYRP_DEBOUNCE) != v)
      nrpn_set(KYRP_DEBOUNCE, v);
    break;
  case WK_RATIO:
    //static uint8_t Ratio=50;                // dah/dit ratio = (3*ratio)/50
    // if (byte < 33) byte=33;
    // if (byte > 66) byte=66;
    v = max(33, min(66, v));
    if (get_nrpn(KYRP_RATIO) != v)
      nrpn_set(KYRP_RATIO, v);
    break;
  case WK_PIN_CONFIG:
    //static uint8_t PinConfig=0x23;          // PTT and side tone enabled, 1.67 word space hang time
    // bits used in PinConfig:
    //   b0:    PTT enable/disable bit, if PTT disabled, LeadIn times will have no effect
    //   b1:    Sidetone enable/disable bit
    //   b4/b5: PTT tail = 2 wordspaces (11), 1.67 wordspaces, 1.33 wordspaces, 1.00 wordspaces dit(00)

  default:
    break;
  }
}

static int winkey_get(winkey_property_t p) {
  switch (p) {
  case WK_MODE_REGISTER: {
    int v = 0;
    if (get_nrpn(KYRP_PAD_SWAP)) v |= 0x08;
    if (get_nrpn(KYRP_PAD_ADAPT) == KYRV_ADAPT_ULTIMATIC)
      v |= 0x20;
    else 
      switch (get_nrpn(KYRP_PAD_MODE)) {
      case KYRV_MODE_A: v |= 0x10; break;
      case KYRV_MODE_B: v |= 0x00; break;
      case KYRV_MODE_S: v |= 0x30; break;
      }
    // PADDLE_ECHO FIX.ME
    // SERIAL_ECHO FIX.ME
    return v;
    }
  case WK_SPEED:	return get_nrpn(KYRP_SPEED);
  case WK_SIDETONE:	return (int)(get_nrpn(KYRP_TONE)/4000.0);
  case WK_WEIGHT:	return get_nrpn(KYRP_WEIGHT);
  case WK_PTT_LEAD_IN:	return get_nrpn(KYRP_HEAD_TIME)/10;
  case WK_PTT_TAIL:	return get_nrpn(KYRP_TAIL_TIME)/10;
  case WK_MIN_WPM:	return 0; // FIX.ME
  case WK_WPM_RANGE:	return 0; // FIX.ME
  case WK_EXTENSION:    return 0;
  case WK_COMPENSATION: return get_nrpn(KYRP_COMP)/10;
  case WK_FARNSWORTH:	return get_nrpn(KYRP_FARNS);
  case WK_PADDLE_POINT: return get_nrpn(KYRP_DEBOUNCE)/10;
  case WK_RATIO:	return get_nrpn(KYRP_RATIO);
  case WK_PIN_CONFIG: {
    int v = 0;
    return v;
  }
  default: 
    return 0;
  }
}

static uint8_t WKstatus=0xC0;           // reported to host when it changes
static int inum;                        // counter for number of bytes received in a given state

// static uint8_t pausing=0;               // "pause" state
static uint8_t breakin=1;               // breakin state
//static uint8_t straight=0;              // state of the straight key (1 = pressed, 0 = released)
//static uint8_t tuning=0;                // "Tune" mode active, deactivate paddle
static uint8_t hostmode  = 0;           // host mode
static uint8_t SpeedPot =  0;           // Speed value from the Potentiometer
//static int     myfreq=800;              // current side tone frequency
//static uint8_t myspeed;                 // current CW speed
//static uint8_t prosign=0;               // set if we are in the middle of a prosign

///////////////////////////////////////
//
// Winkeyer functions
//
///////////////////////////////////////

// check for available byte from host
static byte winkey_available(void) { return Serial2.available(); }

// Read one byte from host
static int winkey_from_host() { return Serial2.read(); }

// Write one byte to the host
static void winkey_to_host(int c) { Serial2.write(c); }

// remove last queued character
static void winkey_backspace(void) { wink.unsend_text(); }

// clear queued characters
static void winkey_clearbuf(void) { wink.abort(); }

// queue one, two, or three characters
static void winkey_queue(uint8_t c) { wink.send_text(c); }

static void winkey_setbufpos(uint8_t byte) {
  // not clear what to do
  // if (byte > 0) byte--;
}

static void winkey_bufzero(uint8_t byte) {
  // not clear what to do
}


// restore winkey state from eeprom
static void winkey_read_from_eeprom(void) { }

// restore winkey byte from eeprom
static byte winkey_read_eeprom(int inum) { return 0; }

// write winkey byte to eeprom
static void winkey_update_eeprom(byte inum, byte data) { }

// set the pausing
static void winkey_set_pausing(uint8_t byte) { }

// see if the keyer is idle
static int winkey_keyer_idle(void) { return get_active_st() > KYRN_WINK_ST; }

// tune on/off
static void winkey_tune(int byte) {
  // hmm, these delays are going to screw everything up
  // Do not bother about lead-in and tail times, but DO switch PTT
  // if (byte) {
  //   winkey_clearbuf();
  //   tuning=1;
  //   if (PTT_ENABLED) {
  //     ptt_on();
  //     delay(50);
  //   }
  //   keydown();
  // } else {
  //   keyup();
  //   if (PTT_ENABLED) {
  //     delay(10);
  //     ptt_off();
  //   }
  //   tuning=0;
  // }
}

static void winkey_rdprom(void) {
  // dump EEPROM command
  // only dump bytes 0 through 15,
  // report the others being zero
  // Nothing must interrupt us, therefore no state machine
  // delays are a no no
  for (inum=0; inum<16; inum++) {
    winkey_to_host(winkey_read_eeprom(inum));
    // delay(20);
  }
  for (inum=16; inum < 256; inum++) {
    winkey_to_host(0);
    // delay(20);
  }
}

static void winkey_wrprom(void) {
  //
  // Load EEPROM command
  // nothing must interrupt us, hence no state machine
  //
  for (int inum = 0; inum < 256; inum += 1) {
    while ( ! winkey_available());
    if (inum < 16)
      winkey_update_eeprom(inum, winkey_from_host());
    else
      (void)winkey_from_host();
  }
}

static void winkey_dump_default(void) {
  // never used so I refrain from making an own "state" for this.
  winkey_to_host(winkey_get(WK_MODE_REGISTER)); // 0
  winkey_to_host(winkey_get(WK_SPEED));		// 1
  winkey_to_host(winkey_get(WK_SIDETONE));      // 2
  winkey_to_host(winkey_get(WK_WEIGHT));	// 3
  winkey_to_host(winkey_get(WK_PTT_LEAD_IN));   // 4
  winkey_to_host(winkey_get(WK_PTT_TAIL));      // 5
  winkey_to_host(winkey_get(WK_MIN_WPM));	// 6
  winkey_to_host(winkey_get(WK_WPM_RANGE));     // 7
  winkey_to_host(winkey_get(WK_EXTENSION));     // 8
  winkey_to_host(winkey_get(WK_COMPENSATION));  // 9
  winkey_to_host(winkey_get(WK_FARNSWORTH));    // 10
  winkey_to_host(winkey_get(WK_PADDLE_POINT));  // 11
  winkey_to_host(winkey_get(WK_RATIO));		// 12
  winkey_to_host(winkey_get(WK_PIN_CONFIG));    // 13
  winkey_to_host(0);				// 14
}  



static byte winkey_get_speed_pot(void) {
  return 0;
}

///////////////////////////////////////
//
// This is the WinKey state machine
//
///////////////////////////////////////
void winkey_state_machine() {
  uint8_t byte;
  static int OldWKstatus=-1;           // this is to detect status changes
  static int OldSpeedPot=-1;           // this is to detect Speed pot changes

  //
  // Now comes the WinKey state machine
  // First, handle commands that need no further bytes
  //
  switch (winkey_state) {
  case GETPOT:		winkey_to_host(128+winkey_get_speed_pot()); winkey_state=FREE; break;      // send immediately
  case BACKSPACE:	winkey_backspace(); winkey_state=FREE; break;
  case CLEAR:		winkey_clearbuf(); winkey_state=FREE; break;
  case WKSTAT:		winkey_to_host(WKstatus); winkey_state=FREE; break;
  case NULLCMD:		winkey_state=FREE; break;
  case CANCELSPD:	winkey_state=FREE; break;      // cancel buffered speed, ignored
  case BUFNOP:		winkey_state=FREE; break;      // buffered no-op; ignored
  case RDPROM:		winkey_rdprom(); winkey_state=FREE; break;
  case WRPROM:		winkey_wrprom(); winkey_state=FREE; break;
  default: break;     // This is a multi-byte command handled below
  }
  //
  // Check serial line, if in hostmode or ADMIN command enter "WinKey" state machine
  // NOTE: process *all* ADMIN command even if hostmode is closed. For example,
  // fldigi sends "echo" first and then "open".
  //
  if (winkey_available()) {
    byte=winkey_from_host();

    if (hostmode == 0 && winkey_state == FREE && byte != ADMIN) return;
    //
    // This switch statement builds the "WinKey" state machine
    //
    switch (winkey_state) {
    case FREE:
      //
      // In the idle state, the incoming byte is either a letter to
      // be transmitted or the start of the WinKey command.
      //
      if (byte >= 0x20) {
			winkey_queue(byte); winkey_state=FREE;
      } else {
			winkey_state=(enum WKSTAT) byte; inum=0;
      }
      break;
    case SWALLOW:	winkey_state=FREE; break;
    case XECHO:		winkey_to_host(byte); winkey_state=FREE; break;
    case MESSAGE:	winkey_state=FREE; break;        // output stored message as CW
    case ADMIN:
      switch (byte) {
      case 0:		winkey_state=SWALLOW; break; // Admin Calibrate
      case 1:		winkey_state=FREE; winkey_read_from_eeprom(); hostmode=0; break; // Admin Reset
      case 2:		hostmode = 1; winkey_to_host(WKVERSION); winkey_state=FREE; break; // Admin Open
      case 3:		hostmode = 0; winkey_read_from_eeprom(); winkey_state=FREE; break; // Admin Close
      case 4:		winkey_state=XECHO; break; // Admin Echo
      case 5:			     // Admin 
      case 6:
      case 8:
      case 9:		winkey_to_host(0); break;            // only for backwards compatibility
      case 7:		winkey_dump_default(); winkey_state=FREE; break;    // Admin DumpDefault
      case 10:		winkey_state=FREE; break;      // Admin set WK1 mode
      case 11:		winkey_state=FREE; break;      // Admin set WK2 mode
      case 12:		winkey_state=RDPROM; break;
      case 13:		winkey_state=WRPROM; break;
      case 14:		winkey_state=MESSAGE; break;
      }
      break;
    case WK2MODE:	winkey_set(WK_MODE_REGISTER, byte); winkey_state=FREE; break;
    case WKSPEED:	winkey_set(WK_SPEED, byte); winkey_state=FREE; break;
    case SIDETONE:	winkey_set(WK_SIDETONE, byte); winkey_state=FREE; break;
    case WEIGHT:	winkey_set(WK_WEIGHT, byte); winkey_state=FREE; break;
    case PTT: 
      if (inum==0) {	winkey_set(WK_PTT_LEAD_IN, byte); inum += 1; break; }
      if (inum==1) {	winkey_set(WK_PTT_TAIL, byte); winkey_state=FREE; break; }
			winkey_state=FREE; break;
    case POTSET:
      if (inum==0) {	winkey_set(WK_MIN_WPM, byte); inum += 1; break; }
      if (inum==1) {	winkey_set(WK_WPM_RANGE, byte); winkey_state=FREE; break; }
			winkey_state=FREE; break;
    case FARNS:		winkey_set(WK_FARNSWORTH, byte); winkey_state=FREE; break;
    case EXTENSION:	winkey_set(WK_EXTENSION, byte); winkey_state=FREE; break;
    case KEYCOMP:	winkey_set(WK_COMPENSATION, byte); winkey_state=FREE; break;
    case RATIO:		winkey_set(WK_RATIO, byte); winkey_state=FREE; break;
    case PINCONFIG:	winkey_set(WK_PIN_CONFIG, byte); winkey_state=FREE; break;
    case PAUSE:		winkey_set_pausing(byte); winkey_state=FREE; break;
    case TUNE:		winkey_tune(byte); winkey_state=FREE; break;
    case HSCW:		winkey_state=FREE; break; // ignored
    case LOADDEF:
      if (inum>=WK_MODE_REGISTER && 
	  inum<=WK_PIN_CONFIG) { 
			winkey_set((winkey_property_t)inum, byte); inum += 1; break;
      }
			winkey_state=FREE; break;
    case PADSW:		winkey_state=FREE; break;      // paddle switch-point ignored
    case SOFTPAD:	winkey_state=FREE; break;     // software paddle ignored
    case POINTER_1:
    case POINTER_2:	winkey_setbufpos(byte); winkey_state=FREE; break; // unclear
    case POINTER_3:	winkey_bufzero(byte); winkey_state=FREE; break; // unclear
    case POINTER:
      switch (byte) {
      case 0:		winkey_clearbuf(); winkey_state=FREE; break;
      case 1:		winkey_state=POINTER_1; break;
      case 2:		winkey_state=POINTER_2; break;
      case 3:		winkey_state=POINTER_3; break;
      default:		winkey_state=FREE;      break;
      }
      break;
    case SETPTT:	winkey_state=FREE; break;     // buffered PTT ignored
    case KEYBUF:	winkey_state=FREE; break;     //buffered key-down ignored
    case WAIT:		winkey_state=FREE; break;      // buffered wait ignored
    case PROSIGN: 
      if (inum==0) {	winkey_queue(0x1b); winkey_queue(byte); inum += 1; break; }
      if (inum==1) {	winkey_queue(byte); winkey_state=FREE; break; }
			winkey_state=FREE; break;
    case BUFSPD:	winkey_state=FREE; break;      // buffered speed change ignored
    case HSCWSPD:	break;	// HSCW speed change ignored
    default:		winkey_state=FREE; break;
    }
  }

  //
  // loop exit code: update WK status and send if changed
  //
  if (breakin) {
    WKstatus |= 0x02;
    breakin=0;
  } else {
    WKstatus &= 0xFD;
    if (winkey_keyer_idle()) {
      WKstatus &= 0xFB;
    } else {
      WKstatus |= 0x04;
    }
  }  
  
  if ((WKstatus != OldWKstatus) && hostmode) {
    winkey_to_host(WKstatus);
    OldWKstatus=WKstatus;
  }
  if ((SpeedPot != OldSpeedPot) && hostmode) {
    winkey_to_host(128+SpeedPot);
    OldSpeedPot=SpeedPot;  
  }
}

/***************************************************************
 ** Winkey interface
 ***************************************************************/
static void winkey_setup(void) {
}

static void winkey_loop(void) {
  winkey_state_machine();
}

#endif
