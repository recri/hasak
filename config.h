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
** KYRN_* are MIDI note numbers or note number relocations, N is for Note.
** KYRC_* are MIDI control change numbers, or control number relocations, C is for Control Change
** KYRP_* are MIDI NRPNs, nonregistered parameter numbers, or nrpn relocations, P is for Parameter
** KYRV_* are parameter values, V is for Value
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
#define KYR_ENABLEx_WINKEY 1  /* {type def title {Should we enable the winkey emulator}} */

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
** Notes are named with KYRN_ prefix and the following addtional conventions
** KYRN_MIDI_* are notes that enter or exit via the MIDI interface.
** KYRN_HW_* are notes derived from or delivered to hardware switches or latches.
** KYRN_AU_* are notes that go into or come out of the audio library graph.
** KYRN_ST_* are notes which serve as sidetone sources for keying.
** KYRN_*_ST are the keyed sidetone and sidetone ptt.
** KYRN_*_OUT are the transmitted key and transmitted ptt.
** KYRN_TXT_* are notes with a velocity value that is a 7 bit ascii character.
** KYRN_ELT_* are notes with velocity values chosen from ".- \t".
**
** all the notes which simply signal on/off have velocity 1 or 0 internally.
** externally they use velocity 127 and 0 for on and off, KYRV_EXT_NOTE_*
**
** there's a group of 4 KYRN_MIDI_* notes which communicate volume, frequency, and speed
** with legacy SDR apps.
**
** there's a group of KYRN_TXT_* and KYRN_ELT_* which pass 7 bit ascii velocities.
**
** The order of the KYRN_ST_* note numbers is significant, lower numbered sidetones
** have higher priority, and higher priority sidetones may take the key away from lower
** priority sidetones.
*/

/* imported/exported signals from/to MIDI */
#define KYRN_MIDI_OUT_KEY	0 /* {type note title {tx key out midi signal}} */
#define KYRN_MIDI_OUT_PTT	1 /* {type note title {tx ptt out midi signal}} */
#define KYRN_MIDI_IN_TUNE	2 /* {type note title {tune key in midi signal}} */
#define KYRN_MIDI_OUT_ST	3 /* {type note title {sidetone out midi signal}} */
#define KYRN_MIDI_VOL		4 /* {type note title {volume leak in/out midi}} */
#define KYRN_MIDI_ST_VOL	5 /* {type note title {sidetone volume leak in/out midi}} */
#define KYRN_MIDI_ST_FREQ	6 /* {type note title {sidetone frequency leak in/out midi}} */
#define KYRN_MIDI_SPEED		7 /* {type note title {keyer speed leak in/out midi}} */

/* Notes for KYR_N_PIN hardware input pins */
#define KYRN_PIN0		8  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN1		9  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN2		10  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN3		11  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN4		12  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN5		13  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN6		14  /* {type nrpn title {input hardware pin note}} */
#define KYRN_PIN7		15  /* {type nrpn title {input hardware pin note}} */

/* notes for KYR_N_POUT hardware output pins */
#define KYRN_POUT0		16  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT1		17  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT2		18  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT3		19  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT4		20  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT5		21  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT6		22  /* {type nrpn title {output hardware pin note}} */
#define KYRN_POUT7		23  /* {type nrpn title {output hardware pin note}} */

/* default imported/exported signals from/to hardware */
#define KYRN_HW_L_PAD		KYRN_PIN0 /* {type note title {1st left paddle switch}} */
#define KYRN_HW_R_PAD		KYRN_PIN1 /* {type note title {1st right paddle switch}} */
#define KYRN_HW_S_KEY		KYRN_PIN2 /* {type note title {1st straight key switch}} */
#define KYRN_HW_EXT_PTT		KYRN_PIN3 /* {type note title {1st external ptt switch}} */
#define KYRN_HW_KEY_OUT		KYRN_POUT0 /* {type note title {hardware key out}} */
#define KYRN_HW_PTT_OUT		KYRN_POUT1 /* {type note title {hardware ptt out}} */

/* additional signals */
#define KYRN_HW_TUNE		24 /* {type note title {tune switch}} */
#define KYRN_HW_L_PAD2		25 /* {type note title {2nd left paddle switch}} */
#define KYRN_HW_R_PAD2		26 /* {type note title {2nd right paddle switch}} */
#define KYRN_HW_S_KEY2		27 /* {type note title {2nd straight key switch}} */
#define KYRN_HW_EXT_PTT2	28 /* {type note title {2nd external ptt switch}} */
#define KYRN_HW_L_PAD3		29 /* {type note title {3rd left paddle switch}} */
#define KYRN_HW_R_PAD3		30 /* {type note title {3rd right paddle switch}} */
#define KYRN_HW_S_KEY3		31 /* {type note title {34d straight key switch}} */
#define KYRN_HW_KEY_OUT2	32 /* {type note title {2nd hardware key out}} */
#define KYRN_HW_PTT_OUT2	33 /* {type note title {2nd hardware ptt out}} */

/* imported/exported signals from/to audio graph */
#define KYRN_AU_ST_KEY		34 /* {type note title {keying sidetone in audio graph}} */
#define KYRN_AU_IQ_KEY		35 /* {type note title {keying transmit in audio graph}} */
#define KYRN_AU_DECODE		36 /* {type note title {receiving tone signal from audio graph}} */

/* sidetone source signals, ordered from highest priority to lowest */
#define KYRN_ST_S_KEY		37 /* {type note title {straight keyed sidetone}} */
#define KYRN_ST_S_KEY2		38 /* {type note title {2nd straight keyed sidetone}} */
#define KYRN_ST_S_KEY3		39 /* {type note title {3rd straight keyed sidetone}} */
#define KYRN_ST_PAD		40 /* {type note title {paddle keyed sidetone}} */
#define KYRN_ST_PAD2		41 /* {type note title {2nd paddle keyed sidetone}} */
#define KYRN_ST_PAD3		42 /* {type note title {3rd paddle keyed sidetone}} */
#define KYRN_ST_TUNE		43 /* {type note title {tune switch sidetone}} */
#define KYRN_ST_TEXT		44 /* {type note title {text keyed sidetone}} */
#define KYRN_ST_TEXT2		45 /* {type note title {2nd text keyed sidetone}} */
#define KYRN_ST_NONE		46 /* {type note title {no keyed sidetone}} */

/* 
** the real sidetone, a copy of one of the previous sidetone sources,
** the real transmit key signal, with ptt head time applied delayed key out signal,
** and the real PTT out signal, extended by ptt head and prolonged by ptt tail or hang.
** Though these are the real signals, they're not output directly, they get mirrored
** in a final step which applies KYRP_TX_ENABLE, KYRP_ST_ENABLE, KYR_IQ_ENABLE, and other
** mode modifications TBD.
*/
#define KYRN_KEY_ST		47 /* {type note title {key sidetone}} */
#define KYRN_PTT_ST		48 /* {type note title {sidetone ptt}} */
#define KYRN_KEY_OUT		49 /* {type note title {key transmitter}} */
#define KYRN_PTT_OUT		50 /* {type note title {keyed transmitter ptt}} */

/* A hack which happens without even thinking about it, send characters as notes */
#define KYRN_TXT_TEXT		51 /* {type note title {note containing 7 bit ascii text for text channel}} */
#define KYRN_TXT_TEXT2		52 /* {type note title {note containing 7 bit ascii text for 2nd text channel}} */	

/* A decoder channel running on the sidetone key line */
#define KYRN_ELT_DEC		53 /* {type note title {note containing 7 bit ascii element decoded from sidetone}} */
#define KYRN_TXT_DEC		54 /* {type note title {note containing 7 bit ascii text decoded from sidetone}} */


#define KYR_N_NOTE		55 /* {type def title {number of note states maintained in the keyer}} */

/* The external values of MIDI NoteOn/NoteOff, internally 0 is off and 1 is on. */
#define KYRV_EXT_NOTE_OFF	0 /* {type val title {external note off velocity}} */
#define KYRV_EXT_NOTE_ON	127 /* {type val title {external note on velocity}} */

/*
** midi control change usage.
**
** We implement the control change set from the CWKeyerShield.
** Or we will whenever it gets specified.
**
*/

/* control change messages used */

#define KYRC_MSB		6 /* {type ctrl title {MIDI control change Data Entry (MSB)}} */
#define KYRC_MASTER_VOLUME	7 /* {type ctrl title {set master volume}} */
#define KYRC_MASTER_BALANCE	8 /* {type ctrl title {TODO: stereo balance}} */

#define KYRC_MASTER_PAN		10 /* {type ctrl title {TODO: stereo position of CW tone}} */
#define KYRC_SIDETONE_VOLUME	12 /* {type ctrl title { set sidetone volume}} */
#define KYRC_SIDETONE_FREQUENCY	13 /* {type ctrl title {set sidetone frequency}} */

#define KYRC_INPUT_LEVEL	16 /* {type ctrl title {TODO: set input level}} */

#define KYRC_LSB		38 /* {type ctrl title {MIDI control change Data Entry (LSB)}} */

#define KYRC_ENABLE_POTS	64 /* {type ctrl title {enable/disable potentiometers}} */ 
#define KYRC_KEYER_AUTOPTT	65 /* {type ctrl title {enable/disable auto-PTT from CW keyer}} */ 
#define KYRC_RESPONSE		66 /* {type ctrl title {enable/disable reporting back to SDR and MIDI controller}} */ 
#define KYRC_MUTE_CWPTT		67 /* {type ctrl title {enable/disable muting of RX audio during auto-PTT}} */ 
#define KYRC_MICPTT_HWPTT	68 /* {type ctrl title {enable/disable that MICIN triggers the hardware PTT output}} */
#define KYRC_CWPTT_HWPTT	69 /* {type ctrl title {enable/disable that CWPTT triggers the hardware PTT output}} */

#define KYRC_KEYER_HANG		72 /* {type ctrl title {set Keyer hang time (if auto-PTT active)}} */ 
#define KYRC_KEYER_LEADIN	73 /* {type ctrl title {set Keyer lead-in time (if auto-PTT active)}} */ 
#define KYRC_CW_SPEED		74 /* {type ctrl title {set CW speed}} */ 
#define KYRC_INPUT_SELECT	75 /* {type ctrl title {TODO:}} */

#define KYRC_NRPN_LSB		98 /* {type ctrl title {MIDI control change Non-registered Parameter (LSB)}} */
#define KYRC_NRPN_MSB		99 /* {type ctrl title {MIDI control change Non-registered Parameter (MSB)}} */

#define KYRC_SET_CHANNEL	119 /* {type ctrl title {Change the default channel to use}} */ 

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
** KYRP_FIRST - commands and information for the midi handler
** start of saved parameters
** KYRP_SECOND - saved parameters for the midi handler
** KYRP_ENABLE - enable flags
** KYRP_LVL - level settings
** KYRP_KEYER -  keyer parameters
** KYRP_PTT - ptt timing
** KYRP_RAMP - keying ramps
** KYRP_PAD - paddle keyer
** KYRP_MISC -
** KYRP_PIN - digital inputs
** KYRP_POUT - digital outputs
** KYRP_ADC - analog inputs
** KYRP_MORSE - morse code table
** KYRP_MIXER -  24 output mixer levels
** KYRP_MIXER2 - 24 output mixer enables
** end of saved parameters
** KYRP_CODEC - general codec commands
** KYRP_WM8960 - WM8960 codec specific commands
** KYRP_EXEC - commands
** KYRP_INFO - information
*/
#define KYRV_NOT_SET -1		/* {type val title {16 bit not set value}} */
#define KYRV_MASK    0x3fff	/* {type val title {14 bit mask}} */
#define KYRV_SIGN(v) ((((int16_t)(v))<<2)>>2) /* {type mac title {sign extend masked 14 bit to 16 bit twos complement}} */

/* NRPN midi block, parameters common to all midi.h based firmware */
#define KYRP_FIRST		0 /* {type rel title {base of nrpns}} */

#define KYRP_NOTHING		(KYRP_FIRST+0) /* {type nrpn title {nothng parameter value, zero is not a valid parameter} property nothing} */
#define KYRP_ID_DEVICE           (KYRP_FIRST+1) /* {type nrpn title {identify this keyer for the correspondent} property idKeyer} */
#define KYRP_ID_VERSION		(KYRP_FIRST+2) /* {type nrpn title {identify this keyer version for the correspondent} property idVersion} */
#define KYRP_ID_JSON		(KYRP_FIRST+3) /* {type nrpn sub cmd title {send the JSON string which describes the keyer} property idJSON} */
#define KYRP_STRING_START	(KYRP_FIRST+4) /* {type nrpn sub cmd title {start a string transfer} property stringBegin} */
#define KYRP_STRING_END		(KYRP_FIRST+5) /* {type nrpn sub cmd title {finish a string transfer} property stringEnd} */
#define KYRP_STRING_BYTE	(KYRP_FIRST+6) /* {type nrpn sub cmd title {transfer a byte for a string} property stringByte} */

#define KYRP_MIDI_INPUTS	(KYRP_FIRST+7) /* {type nrpn sub info title {number of midi input messages received} listen 0} */
#define KYRP_MIDI_OUTPUTS	(KYRP_FIRST+8) /* {type nrpn sub info title {number of midi output messages sent} listen 0} */
#define KYRP_MIDI_ECHOES	(KYRP_FIRST+9) /* {type nrpn sub info title {number of midi automatic echo messages sent} listen 0} */
#define KYRP_MIDI_SENDS		(KYRP_FIRST+10) /* {type nrpn sub info title {number of midi explicit send messages} listen 0} */
#define KYRP_MIDI_NOTES		(KYRP_FIRST+11) /* {type nrpn sub info title {number of midi notes} listen 0} */
#define KYRP_MIDI_CTRLS		(KYRP_FIRST+12) /* {type nrpn sub info title {number of midi notes} listen 0} */
#define KYRP_MIDI_NRPNS		(KYRP_FIRST+13) /* {type nrpn sub info title {number of midi notes} listen 0} */
#define KYRP_LISTENER_NODES     (KYRP_FIRST+14) /* {type nrpn sub info title {number of listener nodes allocated} listen 0} */
#define KYRP_LISTENER_LISTS	(KYRP_FIRST+15) /* {type nrpn sub info title {number of listener lists allocated} listen 0} */
#define KYRP_LISTENER_CALLS	(KYRP_FIRST+16) /* {type nrpn sub info title {number of listener nodes called} listen 0} */
#define KYRP_LISTENER_FIRES	(KYRP_FIRST+17) /* {type nrpn sub info title {number of listener lists called} listen 0} */
#define KYRP_LISTENER_LOOPS	(KYRP_FIRST+18) /* {type nrpn sub info title {number of listener loops detected} listen 0} */
#define KYRP_STATS_RESET	(KYRP_FIRST+19) /* {type nrpn sub cmd title {reset MIDI and listener counts}} */
#define KYRP_LOOP		(KYRP_FIRST+20) /* {type nrpn sub info title {loop counter, ?1.5MHz}} */
#define KYRP_SAMPLE		(KYRP_FIRST+21) /* {type nrpn sub info title {sample counter, 48000Hz}} */
#define KYRP_UPDATE		(KYRP_FIRST+22) /* {type nrpn sub info title {buffer update counter, 1500Hz }} */
#define KYRP_MILLI		(KYRP_FIRST+23) /* {type nrpn sub info title {millisecond counter, 1000Hz}} */
#define KYRP_10MILLI		(KYRP_FIRST+24) /* {type nrpn sub info title {10 millisecond counter, 100Hz}} */
#define KYRP_100MILLI		(KYRP_FIRST+25) /* {type nrpn sub info title {100 millisecond counter, 10Hz}} */

#define KYRP_SAVED		(KYRP_FIRST+26) /* {type rel title {base of saved NRPN parameters}} */

#define KYRP_SECOND		(KYRP_SAVED) /* {type rel title {base of persistent midi.h parameters}} */

#define KYRP_CHANNEL		(KYRP_SECOND+0) /* {type nrpn label Channel title {the MIDI channel} property midiChannel} */
#define KYRP_INPUT_ENABLE	(KYRP_SECOND+1) /* {type ctrl label Input title {enable input from the MIDI channel} property midiInput} */ 
#define KYRP_OUTPUT_ENABLE	(KYRP_SECOND+2) /* {type ctrl label Output title {enable output to the MIDI channel} property midiOutput} */ 
#define KYRP_ECHO_ENABLE	(KYRP_SECOND+3) /* {type ctrl label Echo title {enable echo to the MIDI channel} property midiEcho}} */ 
#define KYRP_LISTENER_ENABLE	(KYRP_SECOND+4) /* {type ctrl label Listen title {enable listening to note, ctrl, and nrpn values} property midiListen}} */

#define KYRP_ENABLE		(KYRP_SECOND+5) /* {type rel title {base of keyer enable nrpns}} */

#define KYRP_PIN_ENABLE		(KYRP_ENABLE+0) /* {type ctrl label Input title {enable input pin processing} property keyerInput} */ 
#define KYRP_POUT_ENABLE	(KYRP_ENABLE+1) /* {type ctrl label Output title {enable output pin processing} property keyerOutput} */ 
#define KYRP_PADC_ENABLE	(KYRP_ENABLE+2) /* {type ctrl label Adc title {enable adc input pin processing} property keyerAdc} */ 
#define KYRP_ST_ENABLE		(KYRP_ENABLE+3) /* {type nrpn label ST title {enable sidetone generation} range {0 1} property sidetoneEnable} */
#define KYRP_TX_ENABLE		(KYRP_ENABLE+4) /* {type nrpn label TX title {enable TX ouputs} range {0 1} property txEnable} */
#define KYRP_IQ_ENABLE		(KYRP_ENABLE+5) /* {type nrpn label IQ title {enable IQ generation} range {0 1} property iqEnable} */
#define KYRP_PTT_REQUIRE	(KYRP_ENABLE+6) /* {type nrpn label PTTReq title {require EXT_PTT to transmit} range {0 1} ignore 1 property externalPTTRequire} */
#define KYRP_RKEY_ENABLE	(KYRP_ENABLE+7) /* {type nrpn label Rem title {enable direct remote control of tune by midi note} range {0 1} property remoteKey} */
#define KYRP_CW_AUTOPTT		(KYRP_ENABLE+8) /* {type ctrl label CWPtt title {enable auto-PTT from CW keyer}} */ /* should always generate, but where does it go? */
#define KYRP_RX_MUTE		(KYRP_ENABLE+9) /* {type ctrl label RX title {enable muting of RX audio during CW PTT}} */ 
#define KYRP_MIC_HWPTT		(KYRP_ENABLE+10) /* {type ctrl label MIC_HWPTT title {enable that MICIN triggers the hardware PTT output}} */
#define KYRP_CW_HWPTT		(KYRP_ENABLE+11) /* {type ctrl label CW_HWPTT title {enable that CWPTT triggers the hardware PTT output}} */
#define KYRP_HDW_IN_ENABLE	(KYRP_ENABLE+12) /* {type nrpn label HdwIn title {enable hardware input audio channel} range {0 1} property hdwInEnable} */
#define KYRP_HDW_OUT_ENABLE	(KYRP_ENABLE+13) /* {type nrpn label HdwOut title {enable hardware output audio channel} range {0 1} property hdwOutEnable} */

#define KYRP_LVL		(KYRP_ENABLE+14) /* {type rel title {base of keyer level setting nrpns}} */

#define KYRP_VOLUME		(KYRP_LVL+0) /* {type nrpn label Vol title {output volume} unit dB/10 range {-128 24} property masterVolume} */
#define KYRP_LEVEL		(KYRP_LVL+1) /* {type nrpn title {STlvl} title {sidetone volume} range {-128 24} default 0 unit dB/10 property sidetoneLevel} */
#define KYRP_IQ_LEVEL		(KYRP_LVL+2) /* {type nrpn title {IQlvl} title {IQ volume} range {-128 24} default 0 unit dB/10 property iqLevel} */
#define KYRP_I2S_LEVEL		(KYRP_LVL+3) /* {type nrpn title I2Slvl} title {I2S level} range {-128 24} default 0 unit dB/10 property i2sLevel} */
#define KYRP_HDW_LEVEL		(KYRP_LVL+4) /* {type nrpn title HdwLVL} title {HDW level} range {-128 24} default 0 unit dB/10 property hdwLevel} */
#define KYRP_ST_BALANCE		(KYRP_LVL+5) /* {type nrpn label STPan title {sidetone pan left or right} range {-8192 8191} unit pp8191 ignore 1 property sidetonePan} */
#define KYRP_IQ_BALANCE		(KYRP_LVL+6) /* {type nrpn label IQBal title {adjustment to IQ balance} range {-8192 8191} unit pp8191 ignore 1 property iqBalance} */

#define KYRP_KEYER		(KYRP_LVL+7) /* {type rel title {base of vox specialized keyer parameters}} */

#define KYRP_TONE		(KYRP_KEYER+0) /* {type nrpn title {sidetone and IQ oscillator frequency} range {0 16383} unit Hz/10 property keyerTone} */
#define KYRP_SPEED		(KYRP_KEYER+1) /* {type nrpn title {keyer speed control} range {0 16384} unit WPM property keyerSpeed} */
#define KYRP_WEIGHT		(KYRP_KEYER+2) /* {type nrpn title {keyer mark/space weight} range {25 75} unit % default 50 property keyerWeight} */
#define KYRP_RATIO		(KYRP_KEYER+3) /* {type nrpn title {keyer dit/dah ratio} range {25 75} unit % default 50 property keyerRatio} */
#define KYRP_FARNS		(KYRP_KEYER+4) /* {type nrpn title {Farnsworth keying speed} range {0 127} default 0 unit WPM property keyerFarnsworth} */
#define KYRP_COMP		(KYRP_KEYER+5) /* {type nrpn title {keyer compensation} range {-8192 8191} default 0 unit sample property keyerCompensation} */
#define KYRP_SPEED_FRAC		(KYRP_KEYER+6) /* {type nrpn title {keyer speed fraction} range {0 127} default 0 unit WPM/128 property keyerSpeedFraction} */

#define KYRP_PTT		(KYRP_KEYER+7) /* {type rel title {PTT timing parameters}} */

#define KYRP_HEAD_TIME		(KYRP_PTT+0) /* {type nrpn label PTTHead title {time ptt should lead key, key delay} range {0 16383} unit sample property pttHeadTime} */
#define KYRP_TAIL_TIME		(KYRP_PTT+1) /* {type nrpn label PTTTail title {time ptt should linger after key} range {0 16383} unit sample property pttTailTime} */
#define KYRP_HANG_TIME		(KYRP_PTT+2) /* {type nrpn label PTTHang title {time in dits ptt should linger after key} range {0 127} unit dit property pttHangTime} */

#define KYRP_RAMP		(KYRP_PTT+3) /* {type rel title {base of the keyer ramp parameters}} */

#define KYRP_RISE_TIME		(KYRP_RAMP+0) /* {type nrpn title {key rise ramp length} range {0 16383} unit sample property keyerRiseTime} */
#define KYRP_FALL_TIME		(KYRP_RAMP+1) /* {type nrpn title {key fall ramp length} range {0 16383} unit sample property keyerFallTime} */
#define KYRP_RISE_RAMP		(KYRP_RAMP+2) /* {type nrpn title {key rise ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN property keyerRiseRamp valuesProperty keyerRamps} */
#define KYRP_FALL_RAMP		(KYRP_RAMP+3) /* {type nrpn title {key fall ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN property keyerFallRamp valuesProperty keyerRamps} */
#define KYRV_RAMP_HANN		0 /* {type val label Hann title {ramp from Hann window function, raised cosine} value-of {KYRP_*_RAMP} property keyerRamps} */
#define KYRV_RAMP_BLACKMAN_HARRIS 1 /* {type val label {Blackman Harris} title {ramp from Blackman Harris window function} value-of {KYRP_*_RAMP} property keyerRamps} */
#define KYRV_RAMP_LINEAR	2 /* {type val label Linear title {linear ramp, for comparison} value-of {KYRP_*_RAMP} property keyerRamps} */

#define KYRP_PAD		(KYRP_RAMP+4) /* {type rel title {base of paddle keyer parameters}} */

#define KYRP_PAD_MODE		(KYRP_PAD+0)	/* {type nrpn title {iambic keyer mode A/B/S} values KYRV_MODE_* default KYRV_MODE_A property paddleMode} */
#define KYRV_MODE_A		0 /* {type val label A title {paddle keyer iambic mode A} value-of {KYRP_PAD_MODE}} */
#define KYRV_MODE_B		1 /* {type val label B title {paddle keyer iambic mode B} value-of {KYRP_PAD_MODE}} */
#define KYRV_MODE_S		2 /* {type val label S title {paddle keyer bug mode} value-of {KYRP_PAD_MODE}} */
#define KYRP_PAD_SWAP		(KYRP_PAD+1) /* {type nrpn title {swap paddles} range {0 1} default 0 property paddleSwapped} */
#define KYRP_PAD_ADAPT		(KYRP_PAD+2) /* {type nrpn title {paddle adapter normal/ultimatic/single lever} values KYRV_ADAPT_* default KYRV_ADAPT_NORMAL property paddleAdapter} */
#define KYRV_ADAPT_NORMAL	0 /* {type val label Normal title {paddle keyer unmodified} value-of {KYRP_PAD_ADAPT}} */
#define KYRV_ADAPT_ULTIMATIC	1 /* {type val label Ultimatic title {paddle keyer modified to produce ultimatic keyer} value-of {KYRP_PAD_ADAPT}} */
#define KYRV_ADAPT_SINGLE	2 /* {type val label Single title {paddle keyer modified to simulate single lever paddle} value-of {KYRP_PAD_ADAPT}} */
#define KYRP_AUTO_ILS		(KYRP_PAD+3) /* {type nrpn title {automatic letter space timing} range {0 1} default 1 property autoLetterSpace} */
#define KYRP_AUTO_IWS		(KYRP_PAD+4) /* {type nrpn title {automatic word space timing} range {0 1} default 0 property autoWordSpace} */
#define KYRP_PAD_KEYER		(KYRP_PAD+5) /* {type nrpn title {paddle keyer implementation} values KYRV_KEYER_* default KYRV_KEYER_VK6PH property paddleKeyer} */
#define KYRV_KEYER_AD5DZ	0 /* {type val label ad5dz title {paddle keyer algorithm by ad5dz} value-of {KYRP_PAD_KEYER} property paddleKeyers}  */
#define KYRV_KEYER_K1EL		1 /* {type val label k1el title {paddle keyer algorithm by k1el} value-of {KYRP_PAD_KEYER} property paddleKeyers} */
#define KYRV_KEYER_ND7PA	2 /* {type val label nd7pa title {paddle keyer algorithm by nd7pa} value-of {KYRP_PAD_KEYER} property paddleKeyers} */
#define KYRV_KEYER_VK6PH	3 /* {type val label vk6ph title {paddle keyer algorithm by vk6ph} value-of {KYRP_PAD_KEYER} property paddleKeyers} */

#define KYRP_MISC		(KYRP_PAD+6) /* {type rel title {base of miscellaneous parameters}} */

#define KYRP_ACTIVE_ST		(KYRP_MISC+0) /* {type nrpn sub info title {note number of active sidetone source} property keyerActiveSidetone} */
#define KYRP_MIXER_SLEW_RAMP	(KYRP_MISC+1) /* {type nrpn title {slew ramp for mixer changes} values KYRV_RAMP_* default KYRV_RAMP_HANN property mixerSlewRamp valuesProperty keyerRamps} */
#define KYRP_MIXER_SLEW_TIME	(KYRP_MISC+2) /* {type nrpn title {slew time for mixer changes} range {0 16383} unit sample property mixerSlewTime} */
#define KYRP_FREQ_SLEW_RAMP	(KYRP_MISC+3) /* {type nrpn title {slew ramp for frequency changes} values KYRV_RAMP_* default KYRV_RAMP_HANN property mixerSlewRamp valuesProperty keyerRamps} */
#define KYRP_FREQ_SLEW_TIME	(KYRP_MISC+4) /* {type nrpn title {slew time for frquency changes} range {0 16383} unit sample property mixerSlewTime} */
#define KYRP_MIX_ENABLE		(KYRP_MISC+5) /* {type nrpn sub short label OutMix title {output mixer enable bits, shorthand} range {0 4095} property outputEnable} */
#define KYRP_MIX_ENABLE_L	(KYRP_MISC+6) /* {type nrpn sub short label OutMixL title {output mixer left enable bits, shorthand} range {0 4095} property outputEnableLeft} */
#define KYRP_MIX_ENABLE_R	(KYRP_MISC+7) /* {type nrpn sub short label OutMixR title {output mixer right enable bits, shorthand} range {0 4095} property outputEnableRight} */
#define KYRP_PIN_DEBOUNCE	(KYRP_MISC+8) /* {type nrpn label Deb title {debounce period} range {0 16383} unit samples property pinDebounce} */
#define KYRP_POUT_LOGIC		(KYRP_MISC+9) /* {type nrpn label OutLog title {output pin logic} range {0 1} property pinLogic} */
#define KYRP_PADC_RATE		(KYRP_MISC+10) /* {type nrpn label AdcRate title {sample rate for analog sampling} range {0 16383} unit ms property adc2Control valuesProperty adcControls} */
#define KYRP_XIQ_FREQ		(KYRP_MISC+11) /* {type nrpn sub ext title {IQ frequency} units hz/10 range {-480000 480000}} */
#define KYRP_IQ_USB		(KYRP_MISC+13) /* {type nrpn title {IQ upper sideband} range {0 1}} */

#define KYRP_PIN		(KYRP_MISC+14) /* {type rel title {base of hardware digital input pin common  block}} */

#define KYRP_PIN0_PIN	(KYRP_PIN+0)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN1_PIN	(KYRP_PIN+1)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN2_PIN	(KYRP_PIN+2)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN3_PIN	(KYRP_PIN+3)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN4_PIN	(KYRP_PIN+4)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN5_PIN	(KYRP_PIN+5)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN6_PIN	(KYRP_PIN+6)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */
#define KYRP_PIN7_PIN	(KYRP_PIN+7)  /* {type nrpn label Pin title {input hardware pin to read} range {-1 127} property inpinPin} */

#define KYRP_POUT	(KYRP_PIN+KYR_N_PIN) /* {type rel title {base of hardware digital output pin block}} */

#define KYRP_POUT0_PIN	(KYRP_POUT+0)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT1_PIN	(KYRP_POUT+1)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT2_PIN	(KYRP_POUT+2)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT3_PIN	(KYRP_POUT+3)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT4_PIN	(KYRP_POUT+4)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT5_PIN	(KYRP_POUT+5)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT6_PIN	(KYRP_POUT+6)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */
#define KYRP_POUT7_PIN	(KYRP_POUT+7)  /* {type nrpn label Pin title {output hardware pin to latch} range {-1 127} property outpinPin} */

#define KYRP_PADC	(KYRP_POUT+KYR_N_POUT) /* {type rel title {base of hardware analog input pin block}} */

#define KYRP_PADC0_PIN	(KYRP_PADC+0) /* {type nrpn label adc0Pin title {hardware pin for adc0} range {-1 127} property adc0Pin} */
#define KYRP_PADC0_VAL	(KYRP_PADC+1) /* {type nrpn label adc0Val title {analog value for adc0} range {0 1023} property adc0Val} */
#define KYRP_PADC0_NRPN	(KYRP_PADC+2) /* {type nrpn label adc0Nrpn title {nrpn connected to adc0} values KYRV_PADC_* property adc0Nrpn} */
#define KYRP_PADC1_PIN	(KYRP_PADC0_PIN+3) /* {type nrpn label adc1Pin title {hardware pin for adc1} range {-1 127} property adc1Pin} */
#define KYRP_PADC1_VAL	(KYRP_PADC0_VAL+3) /* {type nrpn label adc1Val title {analog value for adc1} range {0 1023} property adc1Val} */
#define KYRP_PADC1_NRPN	(KYRP_PADC0_NRPN+3) /* {type nrpn label adc1Nrpn title {nrpn connected to adc1} values KYRV_PADC_* property adc1Nrpn} */
#define KYRP_PADC2_PIN	(KYRP_PADC1_PIN+3) /* {type nrpn label adc2Pin title {hardware pin for adc2} range {-1 127} property adc2Pin} */
#define KYRP_PADC2_VAL	(KYRP_PADC1_VAL+3) /* {type nrpn label adc2Val title {analog value for adc2} range {0 1023} property adc2Val} */
#define KYRP_PADC2_NRPN	(KYRP_PADC1_NRPN+3) /* {type nrpn label adc2Nrpn title {nrpn connected to adc2} values KYRV_PADC_* property adc2Nrpn} */
#define KYRP_PADC3_PIN	(KYRP_PADC2_PIN+3) /* {type nrpn label adc3Pin title {hardware pin for adc3} range {-1 127} property adc3Pin} */
#define KYRP_PADC3_VAL	(KYRP_PADC2_VAL+3) /* {type nrpn label adc3Val title {analog value for adc3} range {0 1023} property adc3Val} */
#define KYRP_PADC3_NRPN	(KYRP_PADC2_NRPN+3) /* {type nrpn label adc3Nrpn title {nrpn connected to adc3} values KYRV_PADC_* property adc3Nrpn} */
#define KYRP_PADC4_PIN	(KYRP_PADC3_PIN+3) /* {type nrpn label adc4Pin title {hardware pin for adc4} range {-1 127} property adc4Pin} */
#define KYRP_PADC4_VAL	(KYRP_PADC3_VAL+3) /* {type nrpn label adc4Val title {analog value for adc4} range {0 1023} property adc4Val} */
#define KYRP_PADC4_NRPN	(KYRP_PADC3_NRPN+3) /* {type nrpn label adc4Nrpn title {nrpn connected to adc4} values KYRV_PADC_* property adc4Nrpn} */
#define KYRP_PADC5_PIN	(KYRP_PADC4_PIN+3) /* {type nrpn label adc5Pin title {hardware pin for adc5} range {-1 127} property adc5Pin} */
#define KYRP_PADC5_VAL	(KYRP_PADC4_VAL+3) /* {type nrpn label adc5Val title {analog value for adc5} range {0 1023} property adc5Val} */
#define KYRP_PADC5_NRPN	(KYRP_PADC4_NRPN+3) /* {type nrpn label adc5Nrpn title {nrpn connected to adc5} values KYRV_PADC_* property adc5Nrpn} */
#define KYRP_PADC6_PIN	(KYRP_PADC5_PIN+3) /* {type nrpn label adc6Pin title {hardware pin for adc6} range {-1 127} property adc6Pin} */
#define KYRP_PADC6_VAL	(KYRP_PADC5_VAL+3) /* {type nrpn label adc6Val title {analog value for adc6} range {0 1023} property adc6Val} */
#define KYRP_PADC6_NRPN	(KYRP_PADC5_NRPN+3) /* {type nrpn label adc6Nrpn title {nrpn connected to adc6} values KYRV_PADC_* property adc6Nrpn} */
#define KYRP_PADC7_PIN	(KYRP_PADC6_PIN+3) /* {type nrpn label adc7Pin title {hardware pin for adc7} range {-1 127} property adc7Pin} */
#define KYRP_PADC7_VAL	(KYRP_PADC6_VAL+3) /* {type nrpn label adc7Val title {analog value for adc7} range {0 1023} property adc7Val} */
#define KYRP_PADC7_NRPN	(KYRP_PADC6_NRPN+3) /* {type nrpn label adc7Nrpn title {nrpn connected to adc7} values KYRV_PADC_* property adc7Nrpn} */

#define KYRP_MORSE	(KYRP_PADC+KYR_N_PADC*3) /* {type rel title {morse code table base}} */

/* 64 morse code translations */
/* morse table for (7 bit ascii)-33, covers ! through `, with many holes */
/* lower case alpha are mapped to upper case on input */
// #define KYRV_MORSE_TABLE "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"

#define KYRP_MIXER		(KYRP_MORSE+64) /* {type rel title {base of output mixer level block}} */

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

#define KYRP_MIX_USB_L0		(KYRP_MIXER+0)	/* {type nrpn title {i2s_in left to usb_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_L1		(KYRP_MIXER+1)	/* {type nrpn title {sidetone left to usb_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_L2		(KYRP_MIXER+2)	/* {type nrpn title {IQ left to usb_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_L3		(KYRP_MIXER+3)  /* {type nrpn title {usb_in left to usb_out left} range {-128 24} unit dB/10} */

#define KYRP_MIX_USB_R0		(KYRP_MIXER+4)	/* {type nrpn title {i2s_in right to usb_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_R1		(KYRP_MIXER+5)	/* {type nrpn title {sidetone right to usb_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_R2		(KYRP_MIXER+6)	/* {type nrpn title {IQ right to usb_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_R3		(KYRP_MIXER+7)  /* {type nrpn title {usb_in right to usb_out right} range {-128 24} unit dB/10} */

#define KYRP_MIX_I2S_L0		(KYRP_MIXER+8)  /* {type nrpn title {usb_in left to i2s_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_L1		(KYRP_MIXER+9)  /* {type nrpn title {sidetone left to i2s_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_L2		(KYRP_MIXER+10) /* {type nrpn title {IQ left to i2s_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_L3		(KYRP_MIXER+11) /* {type nrpn title {i2s_in right to i2s_out right} range {-128 24} unit dB/10} */

#define KYRP_MIX_I2S_R0		(KYRP_MIXER+12)	/* {type nrpn title {usb_in right to i2s_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_R1		(KYRP_MIXER+13)	/* {type nrpn title {sidetone right to i2s_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_R2		(KYRP_MIXER+14) /* {type nrpn title {IQ right to i2s_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_R3		(KYRP_MIXER+15) /* {type nrpn title {i2s_in right to i2s_out right} range {-128 24} unit dB/10} */

#define KYRP_MIX_HDW_L0		(KYRP_MIXER+16) /* {type nrpn title {usb_in left to hdw_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_L1		(KYRP_MIXER+17) /* {type nrpn title {sidetone left to hdw_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_L2		(KYRP_MIXER+18) /* {type nrpn title {IQ left to hdw_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_L3		(KYRP_MIXER+19) /* {type nrpn title {i2s_in left to hdw_out left} range {-128 24} unit dB/10} */

#define KYRP_MIX_HDW_R0		(KYRP_MIXER+20) /* {type nrpn title {usb_in right to hdw_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_R1		(KYRP_MIXER+21) /* {type nrpn title {sidetone right to hdw_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_R2		(KYRP_MIXER+22) /* {type nrpn title {IQ right to hdw_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_R3		(KYRP_MIXER+23) /* {type nrpn title {i2s_in right to hdw_out right} range {-128 24} unit dB/10} */

#define KYRP_MIXER2		(KYRP_MIXER+24) /* {type rel title {base of output mixer enable block}} */

#define KYRP_MIX_EN_USB_L0	(KYRP_MIXER2+0)	/* {type nrpn title {enable i2s_in left to usb_out left} range {0 1}} */
#define KYRP_MIX_EN_USB_L1	(KYRP_MIXER2+1)	/* {type nrpn title {enable sidetone left to usb_out left} range {0 1}} */
#define KYRP_MIX_EN_USB_L2	(KYRP_MIXER2+2)	/* {type nrpn title {enable IQ left to usb_out left} range {0 1}} */
#define KYRP_MIX_EN_USB_L3	(KYRP_MIXER2+3)  /* {type nrpn title {enable usb_in left to usb_out left} range {0 1}} */

#define KYRP_MIX_EN_USB_R0	(KYRP_MIXER2+4)	/* {type nrpn title {enable i2s_in right to usb_out right} range {0 1}} */
#define KYRP_MIX_EN_USB_R1	(KYRP_MIXER2+5)	/* {type nrpn title {enable sidetone right to usb_out right} range {0 1}} */
#define KYRP_MIX_EN_USB_R2	(KYRP_MIXER2+6)	/* {type nrpn title {enable IQ right to usb_out right} range {0 1}} */
#define KYRP_MIX_EN_USB_R3	(KYRP_MIXER2+7)  /* {type nrpn title {enable usb_in right to usb_out right} range {0 1}} */

#define KYRP_MIX_EN_I2S_L0	(KYRP_MIXER2+8)  /* {type nrpn title {enable usb_in left to i2s_out left} range {0 1}} */
#define KYRP_MIX_EN_I2S_L1	(KYRP_MIXER2+9)  /* {type nrpn title {enable sidetone left to i2s_out left} range {0 1}} */
#define KYRP_MIX_EN_I2S_L2	(KYRP_MIXER2+10) /* {type nrpn title {enable IQ left to i2s_out left} range {0 1}} */
#define KYRP_MIX_EN_I2S_L3	(KYRP_MIXER2+11) /* {type nrpn title {enable i2s_in right to i2s_out right} range {0 1}} */

#define KYRP_MIX_EN_I2S_R0	(KYRP_MIXER2+12) /* {type nrpn title {enable usb_in right to i2s_out right} range {0 1}} */
#define KYRP_MIX_EN_I2S_R1	(KYRP_MIXER2+13) /* {type nrpn title {enable sidetone right to i2s_out right} range {0 1}} */
#define KYRP_MIX_EN_I2S_R2	(KYRP_MIXER2+14) /* {type nrpn title {enable IQ right to i2s_out right} range {0 1}} */
#define KYRP_MIX_EN_I2S_R3	(KYRP_MIXER2+15) /* {type nrpn title {enable i2s_in right to i2s_out right} range {0 1}} */

#define KYRP_MIX_EN_HDW_L0	(KYRP_MIXER2+16) /* {type nrpn title {enable usb_in left to hdw_out left} range {0 1}} */
#define KYRP_MIX_EN_HDW_L1	(KYRP_MIXER2+17) /* {type nrpn title {enable sidetone left to hdw_out left} range {0 1}} */
#define KYRP_MIX_EN_HDW_L2	(KYRP_MIXER2+18) /* {type nrpn title {enable IQ left to hdw_out left} range {0 1}} */
#define KYRP_MIX_EN_HDW_L3	(KYRP_MIXER2+19) /* {type nrpn title {enable i2s_in left to hdw_out left} range {0 1}} */

#define KYRP_MIX_EN_HDW_R0	(KYRP_MIXER2+20) /* {type nrpn title {enable usb_in right to hdw_out right} range {0 1}} */
#define KYRP_MIX_EN_HDW_R1	(KYRP_MIXER2+21) /* {type nrpn title {enable sidetone right to hdw_out right} range {0 1}} */
#define KYRP_MIX_EN_HDW_R2	(KYRP_MIXER2+22) /* {type nrpn title {enable IQ right to hdw_out right} range {0 1}} */
#define KYRP_MIX_EN_HDW_R3	(KYRP_MIXER2+23) /* {type nrpn title {enable i2s_in right to hdw_out right} range {0 1}} */

#define KYRP_CODEC		(KYRP_MIXER2+24) /* {type rel title {base of codec nrpns}} */
      
#define KYRP_CODEC_VOLUME	(KYRP_CODEC+0) /* {type nrpn label Vol title {output volume} unit dB/10 range {-128 24} property masterVolume} */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1) /* {type nrpn label InSel title {input select} values KYRV_INPUT_* property inputSelect} */
#define KYRV_INPUT_MIC		0	       /* {type val label Mic title {input select microphone} value-of KYRP_INPUT_SELECT property inputSelects} */
#define KYRV_INPUT_LINE		1	       /* {type val label LineIn title {input select line in} value-of KYRP_INPUT_SELECT property inputSelects} */
#define KYRP_INPUT_LEVEL	(KYRP_CODEC+2) /* {type nrpn label InLvl title {input level} range {-128 24} unit dB/10 property inputLevel} */

#define KYRP_LAST_SAVED		(KYRP_CODEC+3) /* {type rel title {end of persistent params}} */

#define KYRP_WM8960		(KYRP_CODEC+3) /* {type rel title {base of WM8960 commands}} */

#define KYRP_WM8960_ENABLE            (KYRP_WM8960+11-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_INPUT_LEVEL       (KYRP_WM8960+12-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_INPUT_SELECT      (KYRP_WM8960+13-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_VOLUME            (KYRP_WM8960+14-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_HEADPHONE_VOLUME  (KYRP_WM8960+15-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_HEADPHONE_POWER   (KYRP_WM8960+16-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_SPEAKER_VOLUME    (KYRP_WM8960+17-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_SPEAKER_POWER     (KYRP_WM8960+18-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_DISABLE_ADCHPF    (KYRP_WM8960+19-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_ENABLE_MICBIAS    (KYRP_WM8960+20-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_ENABLE_ALC        (KYRP_WM8960+21-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_MIC_POWER         (KYRP_WM8960+22-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_LINEIN_POWER      (KYRP_WM8960+23-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_RAW_MASK          (KYRP_WM8960+24-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_RAW_DATA          (KYRP_WM8960+25-11) /* {type cmd title {WM8960 operation}} */
#define KYRP_WM8960_RAW_WRITE         (KYRP_WM8960+26-11) /* {type cmd title {WM8960 operation}} */

/* relocate the exec and info blocks to contiguous */
#define KYRP_EXEC		(KYRP_WM8960+16) /* {type rel title {base of command nrpns}} */

#define KYRP_WRITE_EEPROM	(KYRP_EXEC+0) /* {type nrpn sub cmd label {Write EEPROM} title {write nrpn+msgs to eeprom} property writeEEPROM} */
#define KYRP_READ_EEPROM	(KYRP_EXEC+1) /* {type nrpn sub cmd label {Read EEPROM} title {read nrpn+msgs from eeprom} property loadEEPROM} */
#define KYRP_SET_DEFAULT	(KYRP_EXEC+2) /* {type nrpn sub cmd label {Reset to default} title {load nrpn with default values} property loadDefaults} */
#define KYRP_ECHO_ALL		(KYRP_EXEC+3) /* {type nrpn sub cmd label {Echo all} title {echo all set nrpns to midi} property echoAll} */
#define KYRP_SEND_TEXT		(KYRP_EXEC+4) /* {type nrpn sub cmd label {Send to Text} title {send character value to primary text keyer} property sendText} */
#define KYRP_SEND_TEXT2		(KYRP_EXEC+5) /* {type nrpn sub cmd label {Send to Text2} title {send character value to secondary text keyer} property sendText2} */
#define KYRP_MSG_INDEX		(KYRP_EXEC+6) /* {type nrpn sub cmd label {Message byte index} title {set index into msgs} property messageIndex} */
#define KYRP_MSG_WRITE		(KYRP_EXEC+7) /* {type nrpn sub cmd label {Message write byte} title {set msgs[index++] to value} property messageWrite} */
#define KYRP_MSG_READ		(KYRP_EXEC+8) /* {type nrpn sub cmd label {Message reqd byte} title {read msgs[index++] and echo the value} property messageRead} */

#define KYRP_INFO		(KYRP_EXEC+9) /* {type rel title {base of information nrpns}} */

#define KYRP_NRPN_SIZE		(KYRP_INFO+0) /* {type nrpn sub info title {size of nrpn array} property nrpnSize} */
#define KYRP_MSG_SIZE		(KYRP_INFO+1) /* {type nrpn sub info title {send the size of msgs array} property messageSize} */
#define KYRP_SAMPLE_RATE	(KYRP_INFO+2) /* {type nrpn sub info title {sample rate of audio library} unit sps/100 property sampleRate} */
#define KYRP_EEPROM_LENGTH	(KYRP_INFO+3) /* {type nrpn sub info title {result of EEPROM.length()} unit bytes property eepromSize} */
#define KYRP_ID_CPU		(KYRP_INFO+4) /* {type nrpn sub info title {which teensy microprocessor are we running} property identifyCPU} */
#define KYRP_ID_CODEC		(KYRP_INFO+5) /* {type nrpn sub info title {which codec are we running} property identifyCodec} */

#define KYRP_SCRATCH		(KYRP_INFO+6) /* {type rel title {base of scratch nrpns}} */

/* xnrpns, two word values */
#define KYRP_XPER_DIT		(KYRP_SCRATCH+0) /* {type nrpn sub ext title {samples per dit}} */
#define KYRP_XPER_DAH		(KYRP_SCRATCH+2) /* {type nrpn sub ext title {samples per dah}} */
#define KYRP_XPER_IES		(KYRP_SCRATCH+4) /* {type nrpn sub ext title {samples per inter element space}} */
#define KYRP_XPER_ILS		(KYRP_SCRATCH+6) /* {type nrpn sub ext title {samples per inter letter space}} */
#define KYRP_XPER_IWS		(KYRP_SCRATCH+8) /* {type nrpn sub ext title {samples per inter word space}} */

#define KYRP_LAST		(KYRP_SCRATCH+10) /* {type rel title {end of all nrpns}} */

#define KYR_N_NRPN		(KYRP_LAST)   /* {type def title {number of nrpns defined}} */

/* definitions of ADC targets, these had undefined symbols when placed immediately after KYRP_PADC* */
#define KYRV_PADC_NOTHING	(KYRP_NOTHING) /* {type val label None title {pot controls nothing} value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_VOLUME	(KYRP_VOLUME) /* {type val label Volume title {pot controls master volume} value-of KYRP_PADC*_NRP property adcControls} */
#define KYRV_PADC_LEVEL		(KYRP_LEVEL) /* {type val label Level title {pot controls sidetone level} value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_TONE		(KYRP_TONE) /* {type val label Tone title {pot controls sidetone pitch} value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_SPEED		(KYRP_SPEED) /* {type val label Speed title {pot controls keyer speed} value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_FARNS		(KYRP_FARNS) /* {type val label Farnsworth title {pot controls keyer Farnsworh speed} value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_COMP		(KYRP_COMP) /* {type val label Comp title {} value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_HEAD_TIME	(KYRP_HEAD_TIME) /* {type val label PTTHead title KYRP_HEAD_TIME value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_TAIL_TIME	(KYRP_TAIL_TIME) /* {type val label PTTTail title KYRP_TAIL_TIME value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_RISE_TIME	(KYRP_RISE_TIME) /* {type val label Rise title KYRP_RISE_TIME value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_FALL_TIME	(KYRP_FALL_TIME) /* {type val label Fall title KYRP_FALL_TIME value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_WEIGHT	(KYRP_WEIGHT) /* {type val label Weight title KYRP_WEIGHT value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_RATIO		(KYRP_RATIO) /* {type val label Ratio title KYRP_RATIO value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_SPEED_FRAC	(KYRP_SPEED_FRAC) /* {type val label Frac title KYRP_SPEED_FRAC value-of KYRP_PADC*_NRPN property adcControls} */
#define KYRV_PADC_CODEC_VOLUME	(KYRP_CODEC_VOLUME) /* {type val label Frac title KYRP_CODEC_VOLUME value-of KYRP_PADC*_NRPN property adcControls} */

/* end of defined nrpns */
/* end of config.h */
#endif
