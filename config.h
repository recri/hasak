/* -*- mode: c++; tab-width: 8 -*- */
/*
 * hasak (ham and swiss army knife) keyer for Teensy 4.X, 3.X
 * Copyright (c) 2021,2022 by Roger Critchlow, Charlestown, MA, USA
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
#ifndef config_h_
#define config_h_

/* 
** naming conventions for config.h defines
**
** KYR_* are configuration definitions
** KYRA_* are audio related definitions, A is for Audio
** NOTE_* are MIDI note numbers or note number relocations, N is for Note.
** CTRL_* are MIDI control change numbers, or control number relocations, C is for Control Change
** NRPN_* are MIDI NRPNs, nonregistered parameter numbers, or nrpn relocations, P is for Parameter
** VAL_* are parameter values, V is for Value
*/

/* Identifier of hasak */
#define KYR_IDENT   0x0ad5 /* {type def title {identifier of the hasak keyer}} */

/* Version of hasak */
#define KYR_VERSION 110	/* {type def title {version number of the NRPN set implemented}} */

/* Magic number to identify valid eeprom contents */
#define KYR_MAGIC   0xad5d /* {type def title {magic number to identify valid EEPROM contents}} */

/* start hot */
#define KYR_ENABLE_TX	1 /* {type def title {Should we start with TX_ENABLE true}} */

/* and whistling */
#define KYR_ENABLE_ST	1 /* {type def title {Should we start with ST_ENABLE true}} */

/* potentiometer input */
#define KYR_ENABLE_POTS 1 /* {type def title {Should we start with potentiometers for volume, pitch, and speed enabled}} */

/* enable hardware output - FIX.ME hdw_out should be pointer */
//#define KYR_ENABLE_HDW_OUT 1 /* {type def title {Should we enable the hardware audio out channel}} */

/* enable hardware input - FIX.ME adc_in should be pointer */
//#define KYR_ENABLE_HDW_IN 1 /* {type def title {Should we enable a hardware input input channel}} */

/* enable winkey emulator, but don't, it doesn't work */
#define KYR_ENABLE_WINKEY 1  /* {type def title {Should we enable the winkey emulator}} */

/* numbers of configurable modules that are implemented */
/* *** don't change these unless you're prepared to modify all the code that's wired to their values *** */
#define KYR_N_PIN	8 /* {type def title {number of digital input pins that can be configured}} */
#define KYR_N_PADC	8 /* {type def title {number of analog input pins that can be configured}} */
#define KYR_N_POUT	8 /* {type def title {number of digital output pins that can be configured}} */
#define KYR_N_SKEY	3 /* {type def title {number of straight key inputs that can be configured}} */
#define KYR_N_PADDLE	3 /* {type def title {number of iambic keyer inputs that can be configured}} */
#define KYR_N_TEXT	2 /* {type def title {number of text keyer outputs that can be configured}} */

/*
** decide which Teensy we're running on
** this works great, but it confuses nrpn.tcl
** so all of the pin values it sees are the last values set
*/
#if ! defined(TEENSYDUINO)

#error "You're not building for a Teensy?"

#elif defined(ARDUINO_TEENSY40)

#define TEENSY4 1
#define TEENSY40 1
#define EEPROM_BYTES 1080
#define TEENSY_CPU_ID 40
static int pin_valid(int pin) { return ((unsigned)(pin) <= 33); }
static int pin_analog(int pin) { return ((pin) >= 14 && (pin) <= 27); }

#elif defined(ARDUINO_TEENSY41)

#define TEENSY4 1
#define TEENSY41 1
#define EEPROM_BYTES 4284
#define TEENSY_CPU_ID 41
static int pin_valid(int pin) { return ((unsigned)(pin) <= 41); }
static int pin_analog(int pin) { return (((pin) >= 14 && (pin) <= 27) || ((pin) >= 38 && (pin) <= 41)); }

#elif defined(ARDUINO_TEENSY30)

#define TEENSY3 1
#define TEENSY30 1
#define EEPROM_BYTES 2048
#define TEENSY_CPU_ID 30
static int pin_valid(int pin) { return ((unsigned)(pin) <= 33); }
static int pin_analog(int pin) { return (((pin) >= 14 && (pin) <= 23) || ((pin) >= 26 && (pin) <= 31)); }

#elif defined(ARDUINO_TEENSY31)

#define TEENSY3 1
#define TEENSY31 1
#define EEPROM_BYTES 2048
#define TEENSY_CPU_ID 31
static int pin_valid(int pin) { return ((unsigned)(pin) <= 33); }
static int pin_analog(int pin) { return (((pin) >= 14 && (pin) <= 23) || ((pin) >= 26 && (pin) <= 31)); }

#elif defined(ARDUINO_TEENSY32)

#define TEENSY3 1
#define TEENSY32 1
#define EEPROM_BYTES 2048
#define TEENSY_CPU_ID 32
static int pin_valid(int pin) { return ((unsigned)(pin) <= 33); }
static int pin_analog(pin) { return (((pin) >= 14 && (pin) <= 23) || ((pin) >= 26 && (pin) <= 31)); }

#elif defined(ARDUINO_TEENSY35)

#define TEENSY3 1
#define TEENSY35 1
#define EEPROM_BYTES 4096
#define TEENSY_CPU_ID 35
static int pin_valid(int pin) { return ((unsigned)(pin) < 58); }
static int pin_analog(int pin) { return (((pin) >= 14 && (pin) <= 27) || ((pin) >= 31 && (pin) <= 39)); }

#elif defined(ARDUINO_TEENSY36)

#define TEENSY3 1
#define TEENSY36 1
#define EEPROM_BYTES 4096
#define TEENSY_CPU_ID 36
static int pin_valid(int pin) { return ((unsigned)(pin) < 58); }
static int pin_analog(int pin) { return (((pin) >= 14 && (pin) <= 27) || ((pin) >= 31 && (pin) <= 39)); }

#else

#error "You're not building for a Teensy 4.1, 4.0, 3.6, 3.5, 3.2, 3.1 or 3.0?"

#endif

/*
** Pin budgeting.
** Pins assigned for audio card interface,
** for I2C communication,
** for hardware audio output,
** and for microphone input or headset switch detection.
** https://www.pjrc.com/teensy/pinout.html
*/
#if defined(TEENSY4)

/* I2S 7, 8, 20, 21, 23 */
#define KYR_DIN		7	/* {type pin title {teensy 4.x i2s DIN pin}} */
#define KYR_DOUT	8	/* {type pin title {teensy 4.x i2s DOUT pin}} */
#define KYR_MCLK	23	/* {type pin title {teensy 4.x i2s MCLK pin}} */
#define KYR_BCLK	21	/* {type pin title {teensy 4.x i2s BCLK pin}} */
#define KYR_LRCLK	20	/* {type pin title {teensy 4.x i2s LRCLK pin}} */
/* I2C 18, 19 */
#define KYR_SCL		19	/* {type pin title {teensy 4.x i2c clock pin}} */
#define KYR_SDA		18	/* {type pin title {teensy 4.x i2c data pin}} */
/* Teensy 4.0, 4.1 MQS 10, 12 */
#define KYR_MQSR	10	/* {type pin title {teensy 4.x medium quality sound pin}} */
#define KYR_MQSL	12	/* {type pin title {teensy 4.x medium quality sound pin}} */
/* Teensy 4.0, 4.1 analog A0, A1, A2, A3, A8 - unless you use bottom pads */
#define KYR_ADC0	14	/* {type pin title {teensy 4.x A0} analog A0} */
#define KYR_ADC1	15	/* {type pin title {volume pot input pin} analog A1} */
#define KYR_ADC2	16	/* {type pin title {sidetone volume pot input pin} analog A2} */
#define KYR_ADC3	17	/* {type pin title {sidetone frequency pot input pin} analog A3} */
#define KYR_ADC4	22	/* {type pin title {keyer speed pot input pin} analog A8} */

#elif defined(TEENSY3)

/* I2S 9, 11, 13, 22, 23 */
#define KYR_DIN		22	/* {type pin title {teensy 3.x i2s DIN pin}} */
#define KYR_DOUT	13	/* {type pin title {teensy 3.x i2s DOUT pin}} */
#define KYR_MCLK	11	/* {type pin title {teensy 3.x i2s MCLK pin}} */
#define KYR_BCLK	9	/* {type pin title {teensy 3.x i2s BCLK pin}} */
#define KYR_LRCLK	23	/* {type pin title {teensy 3.x i2s LRCLK pin}} */
/* I2C 18, 19 */
#define KYR_SCL		19	/* {type pin title {teensy 3.x i2c clock pin}} */
#define KYR_SDA		18	/* {type pin title {teensy 3.x i2c data pin}} */
/* Teensy 3.2 analog A0, A1, A2, A3, A6, A7 - unless you use bottom pads */
#define KYR_ADC0	14	/* {type pin title {teensy 3.x A0} analog A0} */
#define KYR_ADC1	15	/* {type pin title {teensy 3.x A1} analog A1} */
#define KYR_ADC2	16	/* {type pin title {teensy 3.x A2} analog A2} */
#define KYR_ADC3	17	/* {type pin title {teensy 3.x A3} analog A3} */
#define KYR_ADC4	20	/* {type pin title {teensy 3.x A6} analog A6} */

/* DAC */

#if defined(TEENSY30) || defined(TEENSY31) || defined(TEENSY32)
/* Teensy 3.1, 3.2  DAC A14 */
#define KYR_DAC		40	/* {type pin analog A14 title {teensy 3.[0-2] hardware dac pin} ignore 1} */
#endif

#if defined(TEENSY35) || defined(TEENSY36)
/* Teensy 3.5, 3.6 DAC A21, A22 */
#define KYR_DAC0	66	/* {type pin analog A21 title {teens 3.[56] hardware dac pin} ignore 1} */
#define KYR_DAC1	67	/* {type pin analog A22 title {teens 3.[56] hardware dac pin} ignore 1} */
#endif

/* Teensy LC DAC 26/A12 */

#endif
static int pin_i2s(int p) { return ((p)==KYR_DIN||(p)==KYR_DOUT||(p)==KYR_MCLK||(p)==KYR_BCLK||(p)==KYR_LRCLK); }
static int pin_i2c(int p) { return ((p)==KYR_SCL||(p)==KYR_SDA); }
/* 
** pin allocations for standard input switches and output latches
*/
#define KYR_R_PAD_PIN	0	/* {type pin title {right paddle input pin}} */
#define KYR_L_PAD_PIN	1	/* {type pin title {left paddle input pin}} */
#define KYR_S_KEY_PIN	2	/* {type pin title {straight key input pin}} */
#define KYR_EXT_PTT_PIN	3	/* {type pin title {external PTT switch input pin}} */
#define KYR_KEY_OUT_PIN	4	/* {type pin title {KEY output pin}} */
#define KYR_PTT_OUT_PIN	5	/* {type pin title {PTT output pin}} */

/*
** pin allocations for CWKeyer shield input pots
*/
#define KYR_VOLUME_POT	15	/* {type pin title {volume pot input pin} analog A1} */
#define KYR_ST_VOL_POT	16	/* {type pin title {sidetone volume pot input pin} analog A2} */
#define KYR_ST_FREQ_POT	17	/* {type pin title {sidetone frequency pot input pin} analog A3} */
#define KYR_SPEED_POT	22	/* {type pin title {keyer speed pot input pin} analog A8} */

/* 
** MIDI usage
*/
/* midi channel usage */
#define KYR_CHANNEL	10	/* {type def title {default MIDI channel for all keyer MIDI}} */

/*
** midi note usage
** Notes are named with NOTE_ prefix and the following addtional conventions
** NOTE_MIDI_* are notes that enter or exit via the MIDI interface.
** NOTE_HW_* are notes derived from or delivered to hardware switches or latches.
** NOTE_AU_* are notes that go into or come out of the audio library graph.
** NOTE_ST_* are notes which serve as sidetone sources for keying.
** NOTE_*_ST are the keyed sidetone and sidetone ptt.
** NOTE_*_OUT are the transmitted key and transmitted ptt.
** NOTE_TXT_* are notes with a velocity value that is a 7 bit ascii character.
** NOTE_ELT_* are notes with velocity values chosen from ".- \t".
**
** all the notes which simply signal on/off have velocity 1 or 0 internally.
** externally they use velocity 127 and 0 for on and off, VAL_EXT_NOTE_*
**
** there's a group of 4 NOTE_MIDI_* notes which communicate volume, frequency, and speed
** with legacy SDR apps.
**
** there's a group of NOTE_TXT_* and NOTE_ELT_* which pass 7 bit ascii velocities.
**
** The order of the NOTE_ST_* note numbers is significant, lower numbered sidetones
** have higher priority, and higher priority sidetones may take the key away from lower
** priority sidetones.
*/

/* imported/exported signals from/to MIDI */
#define NOTE_MIDI_OUT_KEY	0 /* {type note title {tx key out midi signal}} */
#define NOTE_MIDI_OUT_PTT	1 /* {type note title {tx ptt out midi signal}} */
#define NOTE_MIDI_IN_TUNE	2 /* {type note title {tune key in midi signal}} */
#define NOTE_MIDI_OUT_ST	3 /* {type note title {sidetone out midi signal}} */
#define NOTE_MIDI_VOL		4 /* {type note title {volume leak in/out midi}} */
#define NOTE_MIDI_ST_VOL	5 /* {type note title {sidetone volume leak in/out midi}} */
#define NOTE_MIDI_ST_FREQ	6 /* {type note title {sidetone frequency leak in/out midi}} */
#define NOTE_MIDI_SPEED		7 /* {type note title {keyer speed leak in/out midi}} */

/* Notes for KYR_N_PIN hardware input pins */
#define NOTE_PIN0		8  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN1		9  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN2		10  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN3		11  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN4		12  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN5		13  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN6		14  /* {type nrpn title {input hardware pin note}} */
#define NOTE_PIN7		15  /* {type nrpn title {input hardware pin note}} */

/* notes for KYR_N_POUT hardware output pins */
#define NOTE_POUT0		16  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT1		17  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT2		18  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT3		19  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT4		20  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT5		21  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT6		22  /* {type nrpn title {output hardware pin note}} */
#define NOTE_POUT7		23  /* {type nrpn title {output hardware pin note}} */

/* default imported/exported signals from/to hardware */
#define NOTE_HW_L_PAD		NOTE_PIN0 /* {type note title {1st left paddle switch}} */
#define NOTE_HW_R_PAD		NOTE_PIN1 /* {type note title {1st right paddle switch}} */
#define NOTE_HW_S_KEY		NOTE_PIN2 /* {type note title {1st straight key switch}} */
#define NOTE_HW_EXT_PTT		NOTE_PIN3 /* {type note title {1st external ptt switch}} */
#define NOTE_HW_KEY_OUT		NOTE_POUT0 /* {type note title {hardware key out}} */
#define NOTE_HW_PTT_OUT		NOTE_POUT1 /* {type note title {hardware ptt out}} */

/* additional signals */
#define NOTE_HW_TUNE		24 /* {type note title {tune switch}} */
#define NOTE_HW_L_PAD2		25 /* {type note title {2nd left paddle switch}} */
#define NOTE_HW_R_PAD2		26 /* {type note title {2nd right paddle switch}} */
#define NOTE_HW_S_KEY2		27 /* {type note title {2nd straight key switch}} */
#define NOTE_HW_EXT_PTT2	28 /* {type note title {2nd external ptt switch}} */
#define NOTE_HW_L_PAD3		29 /* {type note title {3rd left paddle switch}} */
#define NOTE_HW_R_PAD3		30 /* {type note title {3rd right paddle switch}} */
#define NOTE_HW_S_KEY3		31 /* {type note title {34d straight key switch}} */
#define NOTE_HW_KEY_OUT2	32 /* {type note title {2nd hardware key out}} */
#define NOTE_HW_PTT_OUT2	33 /* {type note title {2nd hardware ptt out}} */

/* imported/exported signals from/to audio graph */
#define NOTE_AU_ST_KEY		34 /* {type note title {keying sidetone in audio graph}} */
#define NOTE_AU_IQ_KEY		35 /* {type note title {keying transmit in audio graph}} */
#define NOTE_AU_DECODE		36 /* {type note title {receiving tone signal from audio graph}} */

/* sidetone source signals, ordered from highest priority to lowest */
#define NOTE_ST_S_KEY		37 /* {type note title {straight keyed sidetone}} */
#define NOTE_ST_S_KEY2		38 /* {type note title {2nd straight keyed sidetone}} */
#define NOTE_ST_S_KEY3		39 /* {type note title {3rd straight keyed sidetone}} */
#define NOTE_ST_PAD		40 /* {type note title {paddle keyed sidetone}} */
#define NOTE_ST_PAD2		41 /* {type note title {2nd paddle keyed sidetone}} */
#define NOTE_ST_PAD3		42 /* {type note title {3rd paddle keyed sidetone}} */
#define NOTE_ST_TUNE		43 /* {type note title {tune switch sidetone}} */
#define NOTE_ST_TEXT		44 /* {type note title {text keyed sidetone}} */
#define NOTE_ST_TEXT2		45 /* {type note title {2nd text keyed sidetone}} */
#define NOTE_ST_NONE		46 /* {type note title {no keyed sidetone}} */

/* 
** the real sidetone, a copy of one of the previous sidetone sources,
** the real transmit key signal, with ptt head time applied delayed key out signal,
** and the real PTT out signal, extended by ptt head and prolonged by ptt tail or hang.
** Though these are the real signals, they're not output directly, they get mirrored
** in a final step which applies NRPN_TX_ENABLE, NRPN_ST_ENABLE, KYR_IQ_ENABLE, and other
** mode modifications TBD.
*/
#define NOTE_KEY_ST		47 /* {type note title {key sidetone}} */
#define NOTE_PTT_ST		48 /* {type note title {sidetone ptt}} */
#define NOTE_KEY_OUT		49 /* {type note title {key transmitter}} */
#define NOTE_PTT_OUT		50 /* {type note title {keyed transmitter ptt}} */

/* A hack which happens without even thinking about it, send characters as notes */
#define NOTE_TXT_TEXT		51 /* {type note title {note containing 7 bit ascii text for text channel}} */
#define NOTE_TXT_TEXT2		52 /* {type note title {note containing 7 bit ascii text for 2nd text channel}} */	

/* A decoder channel running on the sidetone key line */
#define NOTE_ELT_DEC		53 /* {type note title {note containing 7 bit ascii element decoded from sidetone}} */
#define NOTE_TXT_DEC		54 /* {type note title {note containing 7 bit ascii text decoded from sidetone}} */


#define KYR_N_NOTE		55 /* {type def title {number of note states maintained in the keyer}} */

/* The external values of MIDI NoteOn/NoteOff, internally 0 is off and 1 is on. */
#define VAL_EXT_NOTE_OFF	0 /* {type val title {external note off velocity}} */
#define VAL_EXT_NOTE_ON	127 /* {type val title {external note on velocity}} */

/*
** midi control change usage.
**
** We implement the control change set from the CWKeyerShield.
** Or we will whenever it gets specified.
**
*/

/* control change messages used */

#define CTRL_MSB		6 /* {type ctrl title {MIDI control change Data Entry (MSB)}} */
#define CTRL_MASTER_VOLUME	7 /* {type ctrl title {set master volume}} */
#define CTRL_MASTER_BALANCE	8 /* {type ctrl title {TODO: stereo balance}} */

#define CTRL_MASTER_PAN		10 /* {type ctrl title {TODO: stereo position of CW tone}} */
#define CTRL_SIDETONE_VOLUME	12 /* {type ctrl title { set sidetone volume}} */
#define CTRL_SIDETONE_FREQUENCY	13 /* {type ctrl title {set sidetone frequency}} */

#define CTRL_INPUT_LEVEL	16 /* {type ctrl title {TODO: set input level}} */

#define CTRL_LSB		38 /* {type ctrl title {MIDI control change Data Entry (LSB)}} */

#define CTRL_ENABLE_POTS	64 /* {type ctrl title {enable/disable potentiometers}} */ 
#define CTRL_KEYER_AUTOPTT	65 /* {type ctrl title {enable/disable auto-PTT from CW keyer}} */ 
#define CTRL_RESPONSE		66 /* {type ctrl title {enable/disable reporting back to SDR and MIDI controller}} */ 
#define CTRL_MUTE_CWPTT		67 /* {type ctrl title {enable/disable muting of RX audio during auto-PTT}} */ 
#define CTRL_MICPTT_HWPTT	68 /* {type ctrl title {enable/disable that MICIN triggers the hardware PTT output}} */
#define CTRL_CWPTT_HWPTT	69 /* {type ctrl title {enable/disable that CWPTT triggers the hardware PTT output}} */

#define CTRL_KEYER_HANG		72 /* {type ctrl title {set Keyer hang time (if auto-PTT active)}} */ 
#define CTRL_KEYER_LEADIN	73 /* {type ctrl title {set Keyer lead-in time (if auto-PTT active)}} */ 
#define CTRL_CW_SPEED		74 /* {type ctrl title {set CW speed}} */ 
#define CTRL_INPUT_SELECT	75 /* {type ctrl title {TODO:}} */

#define CTRL_NRPN_LSB		98 /* {type ctrl title {MIDI control change Non-registered Parameter (LSB)}} */
#define CTRL_NRPN_MSB		99 /* {type ctrl title {MIDI control change Non-registered Parameter (MSB)}} */

#define CTRL_SET_CHANNEL	119 /* {type ctrl title {Change the default channel to use}} */ 

#define KYR_N_CTRL		120 /* {type def label N_CTRL title {Number of control change messages handled}} */

/*
** nrpn assigments
**
** Rather than overloading pre-assigned control change messages, which 
** gets out of hand quickly, we use NRPN's for all controls.  NRPN's are 
** Non-Registered Parameter Numbers, so they are specific to particular 
** hardware and don't need to match up to anybody else's list.
**
** Use them like this
**  1) select NRPN with control change 99 (NRPN_MSB) and 98 (NRPN_LSB)
**  2) set value with control change 6 (MSB) and 38 (LSB)
** The actual value set is 14 bits (MSB<<7)|LSB
** The value is latched when CC 38 (LSB) is received.
** Old values of MSB, NRPN_MSB, and NRPN_LSB are reused with the last set value.
**
** This isn't strictly MIDI compliant. That would require us to recognize RPN_MSB(101)
** and RPN_LSB(100) and direct LSB(38) and MSB(6) to a registered parameter handler or
** non-registered parameter handler depending on whether a RPN or NRPN was last sent.
**
** We should also recognize INCR(96) and DECR(97) for adjusting NRPN, and that may yet
** happen.
**
** But I believe we're strictly consistent in the way we act, the subset of MIDI which
** we implement operates exactly as permitted by the standard.
**
** nrpns are organized into blocks: 
** NRPN_FIRST - commands and information for the midi handler
** start of saved parameters
** NRPN_SECOND - saved parameters for the midi handler
** NRPN_ENABLE - enable flags
** NRPN_LVL - level settings
** NRPN_KEYER -  keyer parameters
** NRPN_PTT - ptt timing
** NRPN_RAMP - keying ramps
** NRPN_PAD - paddle keyer
** NRPN_MISC -
** NRPN_PIN - digital inputs
** NRPN_POUT - digital outputs
** NRPN_ADC - analog inputs
** NRPN_MORSE - morse code table
** NRPN_MIXER -  24 output mixer levels
** NRPN_MIXER2 - 24 output mixer enables
** end of saved parameters
** NRPN_CODEC - general codec commands
** NRPN_WM8960 - WM8960 codec specific commands
** NRPN_EXEC - commands
** NRPN_INFO - information
*/
#define VAL_NOT_SET -1		/* {type val title {16 bit not set value}} */
#define VAL_MASK    0x3fff	/* {type val title {14 bit mask}} */
#define VAL_SIGN(v) ((((int16_t)(v))<<2)>>2) /* {type mac title {sign extend masked 14 bit to 16 bit twos complement}} */

/* NRPN midi block, parameters common to all midi.h based firmware */
#define NRPN_FIRST		0 /* {type rel title {base of nrpns}} */

#define NRPN_NOTHING		(NRPN_FIRST+0) /* {type nrpn title {nothng parameter value, zero is not a valid parameter} property nothing} */
#define NRPN_ID_DEVICE          (NRPN_FIRST+1) /* {type nrpn title {identify this keyer for the correspondent} property idKeyer} */
#define NRPN_ID_VERSION		(NRPN_FIRST+2) /* {type nrpn title {identify this keyer version for the correspondent} property idVersion} */
#define NRPN_ID_JSON		(NRPN_FIRST+3) /* {type nrpn sub cmd title {send the JSON string which describes the keyer} property idJSON} */
#define NRPN_STRING_START	(NRPN_FIRST+4) /* {type nrpn sub cmd title {start a string transfer} property stringBegin} */
#define NRPN_STRING_END		(NRPN_FIRST+5) /* {type nrpn sub cmd title {finish a string transfer} property stringEnd} */
#define NRPN_STRING_BYTE	(NRPN_FIRST+6) /* {type nrpn sub cmd title {transfer a byte for a string} property stringByte} */

#define NRPN_MIDI_INPUTS	(NRPN_FIRST+7) /* {type nrpn sub info title {number of midi input messages received} listen 0} */
#define NRPN_MIDI_OUTPUTS	(NRPN_FIRST+8) /* {type nrpn sub info title {number of midi output messages sent} listen 0} */
#define NRPN_MIDI_ECHOES	(NRPN_FIRST+9) /* {type nrpn sub info title {number of midi automatic echo messages sent} listen 0} */
#define NRPN_MIDI_SENDS		(NRPN_FIRST+10) /* {type nrpn sub info title {number of midi explicit send messages} listen 0} */
#define NRPN_MIDI_NOTES		(NRPN_FIRST+11) /* {type nrpn sub info title {number of midi notes} listen 0} */
#define NRPN_MIDI_CTRLS		(NRPN_FIRST+12) /* {type nrpn sub info title {number of midi notes} listen 0} */
#define NRPN_MIDI_NRPNS		(NRPN_FIRST+13) /* {type nrpn sub info title {number of midi notes} listen 0} */
#define NRPN_LISTENER_NODES     (NRPN_FIRST+14) /* {type nrpn sub info title {number of listener nodes allocated} listen 0} */
#define NRPN_LISTENER_LISTS	(NRPN_FIRST+15) /* {type nrpn sub info title {number of listener lists allocated} listen 0} */
#define NRPN_LISTENER_CALLS	(NRPN_FIRST+16) /* {type nrpn sub info title {number of listener nodes called} listen 0} */
#define NRPN_LISTENER_FIRES	(NRPN_FIRST+17) /* {type nrpn sub info title {number of listener lists called} listen 0} */
#define NRPN_LISTENER_LOOPS	(NRPN_FIRST+18) /* {type nrpn sub info title {number of listener loops detected} listen 0} */
#define NRPN_STATS_RESET	(NRPN_FIRST+19) /* {type nrpn sub cmd title {reset MIDI and listener counts}} */

#define NRPN_SAVED		(NRPN_FIRST+20) /* {type rel title {base of saved NRPN parameters}} */

#define NRPN_SECOND		(NRPN_SAVED) /* {type rel title {base of persistent midi.h parameters}} */

#define NRPN_CHANNEL		(NRPN_SECOND+0) /* {type nrpn label Channel title {the MIDI channel} property midiChannel} */
#define NRPN_INPUT_ENABLE	(NRPN_SECOND+1) /* {type ctrl label Input title {enable input from the MIDI channel} property midiInput} */ 
#define NRPN_OUTPUT_ENABLE	(NRPN_SECOND+2) /* {type ctrl label Output title {enable output to the MIDI channel} property midiOutput} */ 
#define NRPN_ECHO_ENABLE	(NRPN_SECOND+3) /* {type ctrl label Echo title {enable echo to the MIDI channel} property midiEcho}} */ 
#define NRPN_LISTENER_ENABLE	(NRPN_SECOND+4) /* {type ctrl label Listen title {enable listening to note, ctrl, and nrpn values} property midiListen}} */

#define NRPN_ENABLE		(NRPN_SECOND+5) /* {type rel title {base of keyer enable nrpns}} */

#define NRPN_PIN_ENABLE		(NRPN_ENABLE+0) /* {type ctrl label Input title {enable input pin processing} property keyerInput} */ 
#define NRPN_POUT_ENABLE	(NRPN_ENABLE+1) /* {type ctrl label Output title {enable output pin processing} property keyerOutput} */ 
#define NRPN_PADC_ENABLE	(NRPN_ENABLE+2) /* {type ctrl label Adc title {enable adc input pin processing} property keyerAdc} */ 
#define NRPN_ST_ENABLE		(NRPN_ENABLE+3) /* {type nrpn label ST title {enable sidetone generation} range {0 1} property sidetoneEnable} */
#define NRPN_TX_ENABLE		(NRPN_ENABLE+4) /* {type nrpn label TX title {enable TX ouputs} range {0 1} property txEnable} */
#define NRPN_IQ_ENABLE		(NRPN_ENABLE+5) /* {type nrpn label IQ title {enable IQ generation} range {0 1} property iqEnable} */
#define NRPN_PTT_REQUIRE	(NRPN_ENABLE+6) /* {type nrpn label PTTReq title {require EXT_PTT to transmit} range {0 1} ignore 1 property externalPTTRequire} */
#define NRPN_RKEY_ENABLE	(NRPN_ENABLE+7) /* {type nrpn label Rem title {enable direct remote control of tune by midi note} range {0 1} property remoteKey} */
#define NRPN_CW_AUTOPTT		(NRPN_ENABLE+8) /* {type ctrl label CWPtt title {enable auto-PTT from CW keyer} note {should always generate, but where does it go?}} */
#define NRPN_RX_MUTE		(NRPN_ENABLE+9) /* {type ctrl label RX title {enable muting of RX audio during CW PTT}} */ 
#define NRPN_MIC_HWPTT		(NRPN_ENABLE+10) /* {type ctrl label MIC_HWPTT title {enable that MICIN triggers the hardware PTT output}} */
#define NRPN_CW_HWPTT		(NRPN_ENABLE+11) /* {type ctrl label CW_HWPTT title {enable that CWPTT triggers the hardware PTT output}} */
#define NRPN_HDW_IN_ENABLE	(NRPN_ENABLE+12) /* {type nrpn label HdwIn title {enable hardware input audio channel} range {0 1} property hdwInEnable} */
#define NRPN_HDW_OUT_ENABLE	(NRPN_ENABLE+13) /* {type nrpn label HdwOut title {enable hardware output audio channel} range {0 1} property hdwOutEnable} */

#define NRPN_LVL		(NRPN_ENABLE+14) /* {type rel title {base of keyer level setting nrpns}} */

#define NRPN_VOLUME		(NRPN_LVL+0) /* {type nrpn label Vol title {output volume} unit dB/10 range {-128 24} property masterVolume} */
#define NRPN_LEVEL		(NRPN_LVL+1) /* {type nrpn title {STlvl} title {sidetone volume} range {-128 24} default 0 unit dB/10 property sidetoneLevel} */
#define NRPN_IQ_LEVEL		(NRPN_LVL+2) /* {type nrpn title {IQlvl} title {IQ volume} range {-128 24} default 0 unit dB/10 property iqLevel} */
#define NRPN_I2S_LEVEL		(NRPN_LVL+3) /* {type nrpn title I2Slvl} title {I2S level} range {-128 24} default 0 unit dB/10 property i2sLevel} */
#define NRPN_HDW_LEVEL		(NRPN_LVL+4) /* {type nrpn title HdwLVL} title {HDW level} range {-128 24} default 0 unit dB/10 property hdwLevel} */
#define NRPN_ST_BALANCE		(NRPN_LVL+5) /* {type nrpn label STPan title {sidetone pan left or right} range {-8192 8191} unit pp8191 ignore 1 property sidetonePan} */
#define NRPN_IQ_BALANCE		(NRPN_LVL+6) /* {type nrpn label IQBal title {adjustment to IQ balance} range {-8192 8191} unit pp8191 ignore 1 property iqBalance} */

#define NRPN_KEYER		(NRPN_LVL+7) /* {type rel title {base of vox specialized keyer parameters}} */

#define NRPN_TONE		(NRPN_KEYER+0) /* {type nrpn title {sidetone and IQ oscillator frequency} range {0 16383} unit Hz/10 property keyerTone} */
#define NRPN_SPEED		(NRPN_KEYER+1) /* {type nrpn title {keyer speed control} range {0 16384} unit WPM property keyerSpeed} */
#define NRPN_WEIGHT		(NRPN_KEYER+2) /* {type nrpn title {keyer mark/space weight} range {25 75} unit % default 50 property keyerWeight} */
#define NRPN_RATIO		(NRPN_KEYER+3) /* {type nrpn title {keyer dit/dah ratio} range {25 75} unit % default 50 property keyerRatio} */
#define NRPN_FARNS		(NRPN_KEYER+4) /* {type nrpn title {Farnsworth keying speed} range {0 127} default 0 unit WPM property keyerFarnsworth} */
#define NRPN_COMP		(NRPN_KEYER+5) /* {type nrpn title {keyer compensation} range {-8192 8191} default 0 unit sample property keyerCompensation} */
#define NRPN_SPEED_FRAC		(NRPN_KEYER+6) /* {type nrpn title {keyer speed fraction} range {0 127} default 0 unit WPM/128 property keyerSpeedFraction} */

#define NRPN_PTT		(NRPN_KEYER+7) /* {type rel title {PTT timing parameters}} */

#define NRPN_HEAD_TIME		(NRPN_PTT+0) /* {type nrpn label PTTHead title {time ptt should lead key, key delay} range {0 16383} unit sample property pttHeadTime} */
#define NRPN_TAIL_TIME		(NRPN_PTT+1) /* {type nrpn label PTTTail title {time ptt should linger after key} range {0 16383} unit sample property pttTailTime} */
#define NRPN_HANG_TIME		(NRPN_PTT+2) /* {type nrpn label PTTHang title {time in dits ptt should linger after key} range {0 127} unit dit property pttHangTime} */

#define NRPN_RAMP		(NRPN_PTT+3) /* {type rel title {base of the keyer ramp parameters}} */

#define NRPN_RISE_TIME		(NRPN_RAMP+0) /* {type nrpn title {key rise ramp length} range {0 16383} unit sample property keyerRiseTime} */
#define NRPN_FALL_TIME		(NRPN_RAMP+1) /* {type nrpn title {key fall ramp length} range {0 16383} unit sample property keyerFallTime} */
#define NRPN_RISE_RAMP		(NRPN_RAMP+2) /* {type nrpn title {key rise ramp} values VAL_RAMP_* default VAL_RAMP_HANN property keyerRiseRamp valuesProperty keyerRamps} */
#define NRPN_FALL_RAMP		(NRPN_RAMP+3) /* {type nrpn title {key fall ramp} values VAL_RAMP_* default VAL_RAMP_HANN property keyerFallRamp valuesProperty keyerRamps} */
#define VAL_RAMP_HANN		0 /* {type val label Hann title {ramp from Hann window function, raised cosine} value-of {NRPN_*_RAMP} property keyerRamps} */
#define VAL_RAMP_BLACKMAN_HARRIS 1 /* {type val label {Blackman Harris} title {ramp from Blackman Harris window function} value-of {NRPN_*_RAMP} property keyerRamps} */
#define VAL_RAMP_LINEAR	2 /* {type val label Linear title {linear ramp, for comparison} value-of {NRPN_*_RAMP} property keyerRamps} */

#define NRPN_PAD		(NRPN_RAMP+4) /* {type rel title {base of paddle keyer parameters}} */

#define NRPN_PAD_MODE		(NRPN_PAD+0)	/* {type nrpn title {iambic keyer mode A/B/S} values VAL_MODE_* default VAL_MODE_A property paddleMode} */
#define VAL_MODE_A		0 /* {type val label A title {paddle keyer iambic mode A} value-of {NRPN_PAD_MODE}} */
#define VAL_MODE_B		1 /* {type val label B title {paddle keyer iambic mode B} value-of {NRPN_PAD_MODE}} */
#define VAL_MODE_S		2 /* {type val label S title {paddle keyer bug mode} value-of {NRPN_PAD_MODE}} */
#define NRPN_PAD_SWAP		(NRPN_PAD+1) /* {type nrpn title {swap paddles} range {0 1} default 0 property paddleSwapped} */
#define NRPN_PAD_ADAPT		(NRPN_PAD+2) /* {type nrpn title {paddle adapter normal/ultimatic/single lever} values VAL_ADAPT_* default VAL_ADAPT_NORMAL property paddleAdapter} */
#define VAL_ADAPT_NORMAL	0 /* {type val label Normal title {paddle keyer unmodified} value-of {NRPN_PAD_ADAPT}} */
#define VAL_ADAPT_ULTIMATIC	1 /* {type val label Ultimatic title {paddle keyer modified to produce ultimatic keyer} value-of {NRPN_PAD_ADAPT}} */
#define VAL_ADAPT_SINGLE	2 /* {type val label Single title {paddle keyer modified to simulate single lever paddle} value-of {NRPN_PAD_ADAPT}} */
#define NRPN_AUTO_ILS		(NRPN_PAD+3) /* {type nrpn title {automatic letter space timing} range {0 1} default 1 property autoLetterSpace} */
#define NRPN_AUTO_IWS		(NRPN_PAD+4) /* {type nrpn title {automatic word space timing} range {0 1} default 0 property autoWordSpace} */
#define NRPN_PAD_KEYER		(NRPN_PAD+5) /* {type nrpn title {paddle keyer implementation} values VAL_KEYER_* default VAL_KEYER_VK6PH property paddleKeyer} */
#define VAL_KEYER_AD5DZ	0 /* {type val label ad5dz title {paddle keyer algorithm by ad5dz} value-of {NRPN_PAD_KEYER} property paddleKeyers}  */
#define VAL_KEYER_K1EL		1 /* {type val label k1el title {paddle keyer algorithm by k1el} value-of {NRPN_PAD_KEYER} property paddleKeyers} */
#define VAL_KEYER_ND7PA	2 /* {type val label nd7pa title {paddle keyer algorithm by nd7pa} value-of {NRPN_PAD_KEYER} property paddleKeyers} */
#define VAL_KEYER_VK6PH	3 /* {type val label vk6ph title {paddle keyer algorithm by vk6ph} value-of {NRPN_PAD_KEYER} property paddleKeyers} */

#define NRPN_MISC		(NRPN_PAD+6) /* {type rel title {base of miscellaneous parameters}} */

#define NRPN_ACTIVE_ST		(NRPN_MISC+0) /* {type nrpn sub info title {note number of active sidetone source} property keyerActiveSidetone} */
#define NRPN_MIXER_SLEW_RAMP	(NRPN_MISC+1) /* {type nrpn title {slew ramp for mixer changes} values VAL_RAMP_* default VAL_RAMP_HANN property mixerSlewRamp valuesProperty keyerRamps} */
#define NRPN_MIXER_SLEW_TIME	(NRPN_MISC+2) /* {type nrpn title {slew time for mixer changes} range {0 16383} unit sample property mixerSlewTime} */
#define NRPN_FREQ_SLEW_RAMP	(NRPN_MISC+3) /* {type nrpn title {slew ramp for frequency changes} values VAL_RAMP_* default VAL_RAMP_HANN property mixerSlewRamp valuesProperty keyerRamps} */
#define NRPN_FREQ_SLEW_TIME	(NRPN_MISC+4) /* {type nrpn title {slew time for frquency changes} range {0 16383} unit sample property mixerSlewTime} */
#define NRPN_MIX_ENABLE		(NRPN_MISC+5) /* {type nrpn sub short label OutMix title {output mixer enable bits, shorthand} range {0 4095} property outputEnable} */
#define NRPN_MIX_ENABLE_L	(NRPN_MISC+6) /* {type nrpn sub short label OutMixL title {output mixer left enable bits, shorthand} range {0 4095} property outputEnableLeft} */
#define NRPN_MIX_ENABLE_R	(NRPN_MISC+7) /* {type nrpn sub short label OutMixR title {output mixer right enable bits, shorthand} range {0 4095} property outputEnableRight} */
#define NRPN_PIN_DEBOUNCE	(NRPN_MISC+8) /* {type nrpn label Deb title {debounce period} range {0 16383} unit samples property pinDebounce} */
#define NRPN_POUT_LOGIC		(NRPN_MISC+9) /* {type nrpn label OutLog title {output pin logic} range {0 1} property pinLogic} */
#define NRPN_PADC_RATE		(NRPN_MISC+10) /* {type nrpn label AdcRate title {sample rate for analog sampling} range {0 16383} unit ms property adc2Control valuesProperty adcControls} */
#define NRPN_XIQ_FREQ		(NRPN_MISC+11) /* {type nrpn sub ext title {IQ frequency} units hz/10 range {-480000 480000}} */
#define NRPN_IQ_USB		(NRPN_MISC+13) /* {type nrpn title {IQ upper sideband} range {0 1}} */

#define NRPN_PIN		(NRPN_MISC+14) /* {type rel title {base of hardware digital input pin common  block}} */

#define NRPN_PIN0_PIN	(NRPN_PIN+0)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN1_PIN	(NRPN_PIN+1)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN2_PIN	(NRPN_PIN+2)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN3_PIN	(NRPN_PIN+3)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN4_PIN	(NRPN_PIN+4)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN5_PIN	(NRPN_PIN+5)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN6_PIN	(NRPN_PIN+6)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define NRPN_PIN7_PIN	(NRPN_PIN+7)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */

#define NRPN_POUT	(NRPN_PIN+KYR_N_PIN) /* {type rel title {base of hardware digital output pin block}} */

#define NRPN_POUT0_PIN	(NRPN_POUT+0)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT1_PIN	(NRPN_POUT+1)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT2_PIN	(NRPN_POUT+2)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT3_PIN	(NRPN_POUT+3)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT4_PIN	(NRPN_POUT+4)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT5_PIN	(NRPN_POUT+5)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT6_PIN	(NRPN_POUT+6)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define NRPN_POUT7_PIN	(NRPN_POUT+7)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */

#define NRPN_PADC	(NRPN_POUT+KYR_N_POUT) /* {type rel title {base of hardware analog input pin block}} */

#define NRPN_PADC0_PIN	(NRPN_PADC+0) /* {type nrpn label adc0Pin title {hardware pin for adc0} range {-1 127} property adc0Pin} */
#define NRPN_PADC0_VAL	(NRPN_PADC+1) /* {type nrpn label adc0Val title {analog value for adc0} range {0 1023} property adc0Val} */
#define NRPN_PADC0_NRPN	(NRPN_PADC+2) /* {type nrpn label adc0Nrpn title {nrpn connected to adc0} values VAL_PADC_* property adc0Nrpn} */
#define NRPN_PADC1_PIN	(NRPN_PADC0_PIN+3) /* {type nrpn label adc1Pin title {hardware pin for adc1} range {-1 127} property adc1Pin} */
#define NRPN_PADC1_VAL	(NRPN_PADC0_VAL+3) /* {type nrpn label adc1Val title {analog value for adc1} range {0 1023} property adc1Val} */
#define NRPN_PADC1_NRPN	(NRPN_PADC0_NRPN+3) /* {type nrpn label adc1Nrpn title {nrpn connected to adc1} values VAL_PADC_* property adc1Nrpn} */
#define NRPN_PADC2_PIN	(NRPN_PADC1_PIN+3) /* {type nrpn label adc2Pin title {hardware pin for adc2} range {-1 127} property adc2Pin} */
#define NRPN_PADC2_VAL	(NRPN_PADC1_VAL+3) /* {type nrpn label adc2Val title {analog value for adc2} range {0 1023} property adc2Val} */
#define NRPN_PADC2_NRPN	(NRPN_PADC1_NRPN+3) /* {type nrpn label adc2Nrpn title {nrpn connected to adc2} values VAL_PADC_* property adc2Nrpn} */
#define NRPN_PADC3_PIN	(NRPN_PADC2_PIN+3) /* {type nrpn label adc3Pin title {hardware pin for adc3} range {-1 127} property adc3Pin} */
#define NRPN_PADC3_VAL	(NRPN_PADC2_VAL+3) /* {type nrpn label adc3Val title {analog value for adc3} range {0 1023} property adc3Val} */
#define NRPN_PADC3_NRPN	(NRPN_PADC2_NRPN+3) /* {type nrpn label adc3Nrpn title {nrpn connected to adc3} values VAL_PADC_* property adc3Nrpn} */
#define NRPN_PADC4_PIN	(NRPN_PADC3_PIN+3) /* {type nrpn label adc4Pin title {hardware pin for adc4} range {-1 127} property adc4Pin} */
#define NRPN_PADC4_VAL	(NRPN_PADC3_VAL+3) /* {type nrpn label adc4Val title {analog value for adc4} range {0 1023} property adc4Val} */
#define NRPN_PADC4_NRPN	(NRPN_PADC3_NRPN+3) /* {type nrpn label adc4Nrpn title {nrpn connected to adc4} values VAL_PADC_* property adc4Nrpn} */
#define NRPN_PADC5_PIN	(NRPN_PADC4_PIN+3) /* {type nrpn label adc5Pin title {hardware pin for adc5} range {-1 127} property adc5Pin} */
#define NRPN_PADC5_VAL	(NRPN_PADC4_VAL+3) /* {type nrpn label adc5Val title {analog value for adc5} range {0 1023} property adc5Val} */
#define NRPN_PADC5_NRPN	(NRPN_PADC4_NRPN+3) /* {type nrpn label adc5Nrpn title {nrpn connected to adc5} values VAL_PADC_* property adc5Nrpn} */
#define NRPN_PADC6_PIN	(NRPN_PADC5_PIN+3) /* {type nrpn label adc6Pin title {hardware pin for adc6} range {-1 127} property adc6Pin} */
#define NRPN_PADC6_VAL	(NRPN_PADC5_VAL+3) /* {type nrpn label adc6Val title {analog value for adc6} range {0 1023} property adc6Val} */
#define NRPN_PADC6_NRPN	(NRPN_PADC5_NRPN+3) /* {type nrpn label adc6Nrpn title {nrpn connected to adc6} values VAL_PADC_* property adc6Nrpn} */
#define NRPN_PADC7_PIN	(NRPN_PADC6_PIN+3) /* {type nrpn label adc7Pin title {hardware pin for adc7} range {-1 127} property adc7Pin} */
#define NRPN_PADC7_VAL	(NRPN_PADC6_VAL+3) /* {type nrpn label adc7Val title {analog value for adc7} range {0 1023} property adc7Val} */
#define NRPN_PADC7_NRPN	(NRPN_PADC6_NRPN+3) /* {type nrpn label adc7Nrpn title {nrpn connected to adc7} values VAL_PADC_* property adc7Nrpn} */

#define NRPN_MORSE	(NRPN_PADC+KYR_N_PADC*3) /* {type rel title {morse code table base}} */

/* 64 morse code translations */
/* morse table for (7 bit ascii)-33, covers ! through `, with many holes */
/* lower case alpha are mapped to upper case on input */
// #define VAL_MORSE_TABLE "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"

#define NRPN_MIXER		(NRPN_MORSE+64) /* {type rel title {base of output mixer level block}} */

/* 24 output mixer levels */
/* left and right channel mixers for four channels for each of usb_out, i2s_out, and mqs_out */
/* also left and right channel enables for the same channels */
/*
**        : usb_out  i2s_out  mqs_out
**  chan 0: i2s_in   usb_in   usb_in
**       1: ST       ST       ST
**       2: IQ       IQ       IQ
**       3: usb_in   i2s_in   i2s_in
** The imagined modes were:
**   keyer:  usb_in+sidetone -> i2s_out or mqs_out,
**   soundcard: usb_in -> i2s_out, i2s_in -> usb_out
**   IQ keyer: usb_in+sidetone -> i2s_out or mqs_out, iq -> usb_out
**   SDR: IQ -> i2s_out, i2s_in x IQ -> mqs out
*/

#define NRPN_MIX_USB_L0		(NRPN_MIXER+0)	/* {type nrpn title {i2s_in left to usb_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_USB_L1		(NRPN_MIXER+1)	/* {type nrpn title {sidetone left to usb_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_USB_L2		(NRPN_MIXER+2)	/* {type nrpn title {IQ left to usb_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_USB_L3		(NRPN_MIXER+3)  /* {type nrpn title {usb_in left to usb_out left} range {-128 24} unit dB/10} */

#define NRPN_MIX_USB_R0		(NRPN_MIXER+4)	/* {type nrpn title {i2s_in right to usb_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_USB_R1		(NRPN_MIXER+5)	/* {type nrpn title {sidetone right to usb_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_USB_R2		(NRPN_MIXER+6)	/* {type nrpn title {IQ right to usb_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_USB_R3		(NRPN_MIXER+7)  /* {type nrpn title {usb_in right to usb_out right} range {-128 24} unit dB/10} */

#define NRPN_MIX_I2S_L0		(NRPN_MIXER+8)  /* {type nrpn title {usb_in left to i2s_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_I2S_L1		(NRPN_MIXER+9)  /* {type nrpn title {sidetone left to i2s_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_I2S_L2		(NRPN_MIXER+10) /* {type nrpn title {IQ left to i2s_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_I2S_L3		(NRPN_MIXER+11) /* {type nrpn title {i2s_in right to i2s_out right} range {-128 24} unit dB/10} */

#define NRPN_MIX_I2S_R0		(NRPN_MIXER+12)	/* {type nrpn title {usb_in right to i2s_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_I2S_R1		(NRPN_MIXER+13)	/* {type nrpn title {sidetone right to i2s_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_I2S_R2		(NRPN_MIXER+14) /* {type nrpn title {IQ right to i2s_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_I2S_R3		(NRPN_MIXER+15) /* {type nrpn title {i2s_in right to i2s_out right} range {-128 24} unit dB/10} */

#define NRPN_MIX_HDW_L0		(NRPN_MIXER+16) /* {type nrpn title {usb_in left to hdw_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_HDW_L1		(NRPN_MIXER+17) /* {type nrpn title {sidetone left to hdw_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_HDW_L2		(NRPN_MIXER+18) /* {type nrpn title {IQ left to hdw_out left} range {-128 24} unit dB/10} */
#define NRPN_MIX_HDW_L3		(NRPN_MIXER+19) /* {type nrpn title {i2s_in left to hdw_out left} range {-128 24} unit dB/10} */

#define NRPN_MIX_HDW_R0		(NRPN_MIXER+20) /* {type nrpn title {usb_in right to hdw_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_HDW_R1		(NRPN_MIXER+21) /* {type nrpn title {sidetone right to hdw_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_HDW_R2		(NRPN_MIXER+22) /* {type nrpn title {IQ right to hdw_out right} range {-128 24} unit dB/10} */
#define NRPN_MIX_HDW_R3		(NRPN_MIXER+23) /* {type nrpn title {i2s_in right to hdw_out right} range {-128 24} unit dB/10} */

#define NRPN_MIXER2		(NRPN_MIXER+24) /* {type rel title {base of output mixer enable block}} */

#define NRPN_MIX_EN_USB_L0	(NRPN_MIXER2+0)	/* {type nrpn title {enable i2s_in left to usb_out left} range {0 1}} */
#define NRPN_MIX_EN_USB_L1	(NRPN_MIXER2+1)	/* {type nrpn title {enable sidetone left to usb_out left} range {0 1}} */
#define NRPN_MIX_EN_USB_L2	(NRPN_MIXER2+2)	/* {type nrpn title {enable IQ left to usb_out left} range {0 1}} */
#define NRPN_MIX_EN_USB_L3	(NRPN_MIXER2+3)  /* {type nrpn title {enable usb_in left to usb_out left} range {0 1}} */

#define NRPN_MIX_EN_USB_R0	(NRPN_MIXER2+4)	/* {type nrpn title {enable i2s_in right to usb_out right} range {0 1}} */
#define NRPN_MIX_EN_USB_R1	(NRPN_MIXER2+5)	/* {type nrpn title {enable sidetone right to usb_out right} range {0 1}} */
#define NRPN_MIX_EN_USB_R2	(NRPN_MIXER2+6)	/* {type nrpn title {enable IQ right to usb_out right} range {0 1}} */
#define NRPN_MIX_EN_USB_R3	(NRPN_MIXER2+7)  /* {type nrpn title {enable usb_in right to usb_out right} range {0 1}} */

#define NRPN_MIX_EN_I2S_L0	(NRPN_MIXER2+8)  /* {type nrpn title {enable usb_in left to i2s_out left} range {0 1}} */
#define NRPN_MIX_EN_I2S_L1	(NRPN_MIXER2+9)  /* {type nrpn title {enable sidetone left to i2s_out left} range {0 1}} */
#define NRPN_MIX_EN_I2S_L2	(NRPN_MIXER2+10) /* {type nrpn title {enable IQ left to i2s_out left} range {0 1}} */
#define NRPN_MIX_EN_I2S_L3	(NRPN_MIXER2+11) /* {type nrpn title {enable i2s_in right to i2s_out right} range {0 1}} */

#define NRPN_MIX_EN_I2S_R0	(NRPN_MIXER2+12) /* {type nrpn title {enable usb_in right to i2s_out right} range {0 1}} */
#define NRPN_MIX_EN_I2S_R1	(NRPN_MIXER2+13) /* {type nrpn title {enable sidetone right to i2s_out right} range {0 1}} */
#define NRPN_MIX_EN_I2S_R2	(NRPN_MIXER2+14) /* {type nrpn title {enable IQ right to i2s_out right} range {0 1}} */
#define NRPN_MIX_EN_I2S_R3	(NRPN_MIXER2+15) /* {type nrpn title {enable i2s_in right to i2s_out right} range {0 1}} */

#define NRPN_MIX_EN_HDW_L0	(NRPN_MIXER2+16) /* {type nrpn title {enable usb_in left to hdw_out left} range {0 1}} */
#define NRPN_MIX_EN_HDW_L1	(NRPN_MIXER2+17) /* {type nrpn title {enable sidetone left to hdw_out left} range {0 1}} */
#define NRPN_MIX_EN_HDW_L2	(NRPN_MIXER2+18) /* {type nrpn title {enable IQ left to hdw_out left} range {0 1}} */
#define NRPN_MIX_EN_HDW_L3	(NRPN_MIXER2+19) /* {type nrpn title {enable i2s_in left to hdw_out left} range {0 1}} */

#define NRPN_MIX_EN_HDW_R0	(NRPN_MIXER2+20) /* {type nrpn title {enable usb_in right to hdw_out right} range {0 1}} */
#define NRPN_MIX_EN_HDW_R1	(NRPN_MIXER2+21) /* {type nrpn title {enable sidetone right to hdw_out right} range {0 1}} */
#define NRPN_MIX_EN_HDW_R2	(NRPN_MIXER2+22) /* {type nrpn title {enable IQ right to hdw_out right} range {0 1}} */
#define NRPN_MIX_EN_HDW_R3	(NRPN_MIXER2+23) /* {type nrpn title {enable i2s_in right to hdw_out right} range {0 1}} */

#define NRPN_CODEC		(NRPN_MIXER2+24) /* {type rel title {base of codec nrpns}} */
      
#define NRPN_CODEC_VOLUME	(NRPN_CODEC+0) /* {type nrpn label Vol title {output volume} unit dB/10 range {-128 24} property masterVolume} */
#define NRPN_INPUT_SELECT	(NRPN_CODEC+1) /* {type nrpn label InSel title {input select} values VAL_INPUT_* property inputSelect} */
#define VAL_INPUT_MIC		0	       /* {type val label Mic title {input select microphone} value-of NRPN_INPUT_SELECT property inputSelects} */
#define VAL_INPUT_LINE		1	       /* {type val label LineIn title {input select line in} value-of NRPN_INPUT_SELECT property inputSelects} */
#define NRPN_INPUT_LEVEL	(NRPN_CODEC+2) /* {type nrpn label InLvl title {input level} range {-128 24} unit dB/10 property inputLevel} */

#define NRPN_LAST_SAVED		(NRPN_CODEC+3) /* {type rel title {end of persistent params}} */

#define NRPN_EPHEMERA		(NRPN_LAST_SAVED) /* {type rel title {base of ephemeral nrpns}} */

#define NRPN_WM8960		(NRPN_CODEC+3) /* {type rel title {base of WM8960 commands}} */

#define NRPN_WM8960_ENABLE            (NRPN_WM8960+11-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_INPUT_LEVEL       (NRPN_WM8960+12-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_INPUT_SELECT      (NRPN_WM8960+13-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_VOLUME            (NRPN_WM8960+14-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_HEADPHONE_VOLUME  (NRPN_WM8960+15-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_HEADPHONE_POWER   (NRPN_WM8960+16-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_SPEAKER_VOLUME    (NRPN_WM8960+17-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_SPEAKER_POWER     (NRPN_WM8960+18-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_DISABLE_ADCHPF    (NRPN_WM8960+19-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_ENABLE_MICBIAS    (NRPN_WM8960+20-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_ENABLE_ALC        (NRPN_WM8960+21-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_MIC_POWER         (NRPN_WM8960+22-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_LINEIN_POWER      (NRPN_WM8960+23-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_RAW_MASK          (NRPN_WM8960+24-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_RAW_DATA          (NRPN_WM8960+25-11) /* {type cmd title {WM8960 operation}} */
#define NRPN_WM8960_RAW_WRITE         (NRPN_WM8960+26-11) /* {type cmd title {WM8960 operation}} */

/* relocate the exec and info blocks to contiguous */
#define NRPN_EXEC		(NRPN_WM8960+16) /* {type rel title {base of command nrpns}} */

#define NRPN_WRITE_EEPROM	(NRPN_EXEC+0) /* {type nrpn sub cmd label {Write EEPROM} title {write nrpn+msgs to eeprom} property writeEEPROM} */
#define NRPN_READ_EEPROM	(NRPN_EXEC+1) /* {type nrpn sub cmd label {Read EEPROM} title {read nrpn+msgs from eeprom} property loadEEPROM} */
#define NRPN_SET_DEFAULT	(NRPN_EXEC+2) /* {type nrpn sub cmd label {Reset to default} title {load nrpn with default values} property loadDefaults} */
#define NRPN_ECHO_ALL		(NRPN_EXEC+3) /* {type nrpn sub cmd label {Echo all} title {echo all set nrpns to midi} property echoAll} */
#define NRPN_SEND_TEXT		(NRPN_EXEC+4) /* {type nrpn sub cmd label {Send to Text} title {send character value to primary text keyer} property sendText} */
#define NRPN_SEND_TEXT2		(NRPN_EXEC+5) /* {type nrpn sub cmd label {Send to Text2} title {send character value to secondary text keyer} property sendText2} */
#define NRPN_MSG_INDEX		(NRPN_EXEC+6) /* {type nrpn sub cmd label {Message byte index} title {set index into msgs} property messageIndex} */
#define NRPN_MSG_WRITE		(NRPN_EXEC+7) /* {type nrpn sub cmd label {Message write byte} title {set msgs[index++] to value} property messageWrite} */
#define NRPN_MSG_READ		(NRPN_EXEC+8) /* {type nrpn sub cmd label {Message reqd byte} title {read msgs[index++] and echo the value} property messageRead} */

#define NRPN_INFO		(NRPN_EXEC+9) /* {type rel title {base of information nrpns}} */

#define NRPN_NRPN_SIZE		(NRPN_INFO+0) /* {type nrpn sub info title {size of nrpn array} property nrpnSize} */
#define NRPN_MSG_SIZE		(NRPN_INFO+1) /* {type nrpn sub info title {send the size of msgs array} property messageSize} */
#define NRPN_SAMPLE_RATE	(NRPN_INFO+2) /* {type nrpn sub info title {sample rate of audio library} unit sps/100 property sampleRate} */
#define NRPN_EEPROM_LENGTH	(NRPN_INFO+3) /* {type nrpn sub info title {result of EEPROM.length()} unit bytes property eepromSize} */
#define NRPN_ID_CPU		(NRPN_INFO+4) /* {type nrpn sub info title {which teensy microprocessor are we running} property identifyCPU} */
#define NRPN_ID_CODEC		(NRPN_INFO+5) /* {type nrpn sub info title {which codec are we running} property identifyCodec} */

#define NRPN_SCRATCH		(NRPN_INFO+6) /* {type rel title {base of scratch nrpns}} */

/* xnrpns, two word values */
#define NRPN_XPER_DIT		(NRPN_SCRATCH+0) /* {type nrpn sub ext title {samples per dit}} */
#define NRPN_XPER_DAH		(NRPN_SCRATCH+2) /* {type nrpn sub ext title {samples per dah}} */
#define NRPN_XPER_IES		(NRPN_SCRATCH+4) /* {type nrpn sub ext title {samples per inter element space}} */
#define NRPN_XPER_ILS		(NRPN_SCRATCH+6) /* {type nrpn sub ext title {samples per inter letter space}} */
#define NRPN_XPER_IWS		(NRPN_SCRATCH+8) /* {type nrpn sub ext title {samples per inter word space}} */

#define NRPN_LOOP		(NRPN_SCRATCH+10) /* {type nrpn sub info title {loop counter, ?1.5MHz}} */
#define NRPN_SAMPLE		(NRPN_SCRATCH+11) /* {type nrpn sub info title {sample counter, 48000Hz}} */
#define NRPN_UPDATE		(NRPN_SCRATCH+12) /* {type nrpn sub info title {buffer update counter, 1500Hz }} */
#define NRPN_MILLI		(NRPN_SCRATCH+13) /* {type nrpn sub info title {millisecond counter, 1000Hz}} */
#define NRPN_10MILLI		(NRPN_SCRATCH+14) /* {type nrpn sub info title {10 millisecond counter, 100Hz}} */
#define NRPN_100MILLI		(NRPN_SCRATCH+15) /* {type nrpn sub info title {100 millisecond counter, 10Hz}} */

#define NRPN_LAST		(NRPN_SCRATCH+16) /* {type rel title {end of all nrpns}} */

#define KYR_N_NRPN		(NRPN_LAST)   /* {type def title {number of nrpns defined}} */

/* definitions of ADC targets, these had undefined symbols when placed immediately after NRPN_PADC* */
#define VAL_PADC_NOTHING	(NRPN_NOTHING) /* {type val label None title {pot controls nothing} value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_VOLUME	(NRPN_VOLUME) /* {type val label Volume title {pot controls master volume} value-of NRPN_PADC*_NRP property adcControls} */
#define VAL_PADC_LEVEL		(NRPN_LEVEL) /* {type val label Level title {pot controls sidetone level} value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_TONE		(NRPN_TONE) /* {type val label Tone title {pot controls sidetone pitch} value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_SPEED		(NRPN_SPEED) /* {type val label Speed title {pot controls keyer speed} value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_FARNS		(NRPN_FARNS) /* {type val label Farnsworth title {pot controls keyer Farnsworh speed} value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_COMP		(NRPN_COMP) /* {type val label Comp title {} value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_HEAD_TIME	(NRPN_HEAD_TIME) /* {type val label PTTHead title NRPN_HEAD_TIME value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_TAIL_TIME	(NRPN_TAIL_TIME) /* {type val label PTTTail title NRPN_TAIL_TIME value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_RISE_TIME	(NRPN_RISE_TIME) /* {type val label Rise title NRPN_RISE_TIME value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_FALL_TIME	(NRPN_FALL_TIME) /* {type val label Fall title NRPN_FALL_TIME value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_WEIGHT	(NRPN_WEIGHT) /* {type val label Weight title NRPN_WEIGHT value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_RATIO		(NRPN_RATIO) /* {type val label Ratio title NRPN_RATIO value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_SPEED_FRAC	(NRPN_SPEED_FRAC) /* {type val label Frac title NRPN_SPEED_FRAC value-of NRPN_PADC*_NRPN property adcControls} */
#define VAL_PADC_CODEC_VOLUME	(NRPN_CODEC_VOLUME) /* {type val label Frac title NRPN_CODEC_VOLUME value-of NRPN_PADC*_NRPN property adcControls} */

/* end of defined nrpns */
/* end of config.h */
#endif
