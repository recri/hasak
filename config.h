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
#define KYR_IDENT   0x0ad5	/* {type def title {identifier of the hasak keyer}} */

/* Version of hasak */
#define KYR_VERSION 110	/* {type def title {version number of the NRPN set implemented}} */

/* Magic number to identify valid eeprom contents */
#define KYR_MAGIC   0xad5d	/* {type def title {magic number to identify valid EEPROM contents}} */

/* start hot */
#define KYR_ENABLE_TX	1	/* {type conf title {Should we start with TX_ENABLE true}} */

/* and whistling */
#define KYR_ENABLE_ST	1	/* {type conf title {Should we start with ST_ENABLE true}} */

/* potentiometer input */
#define KYR_ENABLE_POTS 1	/* {type conf title {Should we start with potentiometers for volume, pitch, and speed enabled}} */

/* enable hardware output */
#define KYR_ENABLE_HDW_OUT 1	/* {type conf title {Should we enable the hardware audio out channel}} */

/* enable hardware input */
//#define KYR_ENABLE_ADC_IN 1	/* {type conf title {Should we enable a hardware input input channel}} */

/* enable winkey emulator, but don't, it doesn't work */
#define KYR_ENABLE_WINKEY 1  /* {type conf title {Should we enable the winkey emulator}} */

/* enable extra straight keys */
#define KYR_ENABLE_SKEY2
#define KYR_ENABLE_SKEY3
#define KYR_ENABLE_BUT

/* enable extra paddles */
#define KYR_ENABLE_PADDLE2
#define KYR_ENABLE_PADDLE3

/* enable extra text keyer */
#define KYR_ENABLE_TEXT2

/*
** decide which Teensy we're running on
*/
#if defined(TEENSYDUINO)
  #if defined(ARDUINO_TEENSY40)
  #define TEENSY4 1
  #define TEENSY40 1
  #define EEPROM_BYTES 1080
  #elif defined(ARDUINO_TEENSY41)
  #define TEENSY4 1
  #define TEENSY41 1
  #define EEPROM_BYTES 4284
  #elif defined(ARDUINO_TEENSY30)
  #define TEENSY3 1
  #define TEENSY30 1
  #define EEPROM_BYTES 2048
  #elif defined(ARDUINO_TEENSY31)
  #define TEENSY3 1
  #define TEENSY31 1
  #define EEPROM_BYTES 2048
  #elif defined(ARDUINO_TEENSY32)
  #define TEENSY3 1
  #define TEENSY32 1
  #define EEPROM_BYTES 2048
  #elif defined(ARDUINO_TEENSY35)
  #define TEENSY3 1
  #define TEENSY35 1
  #define EEPROM_BYTES 4096
  #elif defined(ARDUINO_TEENSY36)
  #define TEENSY3 1
  #define TEENSY36 1
  #define EEPROM_BYTES 4096
  #else
  #error "You're not building for a Teensy 4.1, 4.0, 3.6, 3.5, 3.2, 3.1 or 3.0?"
  #endif
#else
  #error "You're not building for a Teensy?"
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
#define KYR_CHANNEL	1	/* {type def title {default MIDI channel for all keyer MIDI}} */

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
** externally they use velocit 127 and 0 for on and off 
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
#define KYRN_MIDI_OUT_ST	3 /* {type note title {sidetone  out midi signal}} */
#define KYRN_MIDI_VOL		4 /* {type note title {volume leak in/out midi}} */
#define KYRN_MIDI_ST_VOL	5 /* {type note title {sidetone volume leak in/out midi}} */
#define KYRN_MIDI_ST_FREQ	6 /* {type note title {sidetone frequency leak in/out midi}} */
#define KYRN_MIDI_SPEED		7 /* {type note title {keyer speed leak in/out midi}} */

/* imported/exported signals from/to hardware */
#define KYRN_HW_L_PAD		8 /* {type note title {1st left paddle switch}} */
#define KYRN_HW_R_PAD		9 /* {type note title {1st right paddle switch}} */
#define KYRN_HW_S_KEY		10 /* {type note title {1st straight key switch}} */
#define KYRN_HW_EXT_PTT		11 /* {type note title {1st external ptt switch}} */
#define KYRN_HW_BUT		12 /* {type note title {button key}} */
#define KYRN_HW_TUNE		13 /* {type note title {tune switch}} */
#define KYRN_HW_L_PAD2		14 /* {type note title {2nd left paddle switch}} */
#define KYRN_HW_R_PAD2		15 /* {type note title {2nd right paddle switch}} */
#define KYRN_HW_S_KEY2		16 /* {type note title {2nd straight key switch}} */
#define KYRN_HW_EXT_PTT2	17 /* {type note title {2nd external ptt switch}} */
#define KYRN_HW_L_PAD3		18 /* {type note title {3rd left paddle switch}} */
#define KYRN_HW_R_PAD3		19 /* {type note title {3rd right paddle switch}} */
#define KYRN_HW_S_KEY3		20 /* {type note title {34d straight key switch}} */
#define KYRN_HW_KEY_OUT		21 /* {type note title {hardware key out}} */
#define KYRN_HW_PTT_OUT		22 /* {type note title {hardware ptt out}} */
#define KYRN_HW_KEY_OUT2	23 /* {type note title {2nd hardware key out}} */
#define KYRN_HW_PTT_OUT2	24 /* {type note title {2nd hardware ptt out}} */

/* imported/exported signals from/to audio graph */
#define KYRN_AU_ST_KEY		25 /* {type note title {keying sidetone in audio graph}} */
#define KYRN_AU_IQ_KEY		26 /* {type note title {keying transmit in audio graph}} */
#define KYRN_AU_DECODE		27 /* {type note title {receiving tone signal from audio graph}} */

/* sidetone source signals, ordered from highest priority to lowest */
#define KYRN_ST_S_KEY		28 /* {type note title {straight keyed sidetone}} */
#define KYRN_ST_S_KEY2		29 /* {type note title {2nd straight keyed sidetone}} */
#define KYRN_ST_S_KEY3		30 /* {type note title {3rd straight keyed sidetone}} */
#define KYRN_ST_PAD		31 /* {type note title {paddle keyed sidetone}} */
#define KYRN_ST_PAD2		32 /* {type note title {2nd paddle keyed sidetone}} */
#define KYRN_ST_PAD3		33 /* {type note title {3rd paddle keyed sidetone}} */
#define KYRN_ST_TUNE		34 /* {type note title {tune switch sidetone}} */
#define KYRN_ST_BUT		35 /* {type note title {button switch sidetone}} */
#define KYRN_ST_TEXT		36 /* {type note title {text keyed sidetone}} */
#define KYRN_ST_TEXT2		37 /* {type note title {2nd text keyed sidetone}} */
#define KYRN_ST_NONE		38 /* {type note title {no keyed sidetone}} */

/* 
** the real sidetone, a copy of one of the previous sidetone sources,
** the real transmit key signal, with ptt head time applied delayed key out signal,
** and the real PTT out signal, extended by ptt head and prolonged by ptt tail or hang.
** Though these are the real signals, they're not output directly, they get mirrored
** in a final step which applies KYRP_TX_ENABLE, KYRP_ST_ENABLE, KYR_IQ_ENABLE, and other
** mode modifications TBD.
*/
#define KYRN_KEY_ST		39 /* {type note title {key sidetone}} */
#define KYRN_PTT_ST		40 /* {type note title {sidetone ptt}} */
#define KYRN_KEY_OUT		41 /* {type note title {key transmitter}} */
#define KYRN_PTT_OUT		42 /* {type note title {keyed transmitter ptt}} */

/* A hack which happens without even thinking about it, send characters as notes */
#define KYRN_TXT_TEXT		43 /* {type note title {note containing 7 bit ascii text for text channel}} */
#define KYRN_TXT_TEXT2		44 /* {type note title {note containing 7 bit ascii text for 2nd text channel}} */	

/* A decoder channel running on the sidetone key line */
#define KYRN_ELT_DEC		45 /* {type note title {note containing 7 bit ascii element decoded from sidetone}} */
#define KYRN_TXT_DEC		46 /* {type note title {note containing 7 bit ascii text decoded from sidetone}} */

/* The external values of MIDI NoteOn/NoteOff, internally 0 is off and 1 is on. */
#define KYR_EXT_NOTE_OFF	0 /* {type def title {external note off velocity}} */
#define KYR_EXT_NOTE_ON		127 /* {type def title {external note on velocity}} */

#define KYR_N_NOTE		47 /* {type def title {number of note states maintained in the keyer}} */

/*
** midi control change usage.
**
** rather than overloading pre-assigned control change messages, which gets out
** of hand quickly, we use NRPN's for all controls.  NRPN's are Non-Registered
** Parameter Numbers, so they are specific to particular hardware and don't
** need to match up to anybody else's list.
**
** Use them like this
**  1) select NRPN with control change 99 (NRPN_MSB) and 98 (NRPN_LSB)
**  2) set value with control change 6 (MSB) and 38 (LSB)
** The actual value set is 14 bits (MSB<<7)|LSB
** The value is stored when CC 38 (LSB) is received.
** Old values of MSB are reused with the last set value.
**
** We also implement the control change set from the CWKeyerShield.
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
** nrpns were organized into blocks: 
** KYRP_CODEC - global codec controls
** KYRP_SOFT  - soft global controls
** KYRP_MORSE - the 64 character morse code table
** KYRP_MIXER - the 24 output mixer levels
** KYRP_KEYER - the default keyer parameters
** but now they're jumbled up.
*/
#define KYRV_NOT_SET -1		/* {type val title {16 bit not set value}} */
#define KYRV_MASK    0x3fff	/* {type val title {14 bit mask}} */
#define KYRV_SIGN(v) ((((int16_t)(v))<<2)>>2) /* {type mac title {sign extend masked 14 bit to 16 bit twos complement}} */

/* NRPN_GLOBAL_BLOCK - parameters controlling the entire keyer */
#define KYRP_FIRST		0 /* {type rel title {base of nrpns}} */

#define KYRP_NOTHING		(KYRP_FIRST+0) /* {type nrpn title {nothng parameter value, zero is not a valid parameter} property nothing} */
#define KYRP_ID_KEYER		(KYRP_FIRST+1) /* {type nrpn sub info title {unique identfier} property keyerIdentifier} */
#define KYRP_ID_VERSION		(KYRP_FIRST+2) /* {type nrpn sub info title {version identfier} property keyerVersion} */

#define KYRP_CODEC		(KYRP_FIRST+3) /* {type rel title {base of codec nrpns}} */
      
#define KYRP_VOLUME		(KYRP_CODEC+0) /* {type nrpn label Vol title {output volume} unit dB/10 range {-128 24} property masterVolume} */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1) /* {type nrpn label InSel title {input select} values KYRV_INPUT_* property inputSelect} */
#define KYRV_INPUT_MIC		0	       /* {type val label Mic title {input select microphone} value-of KYRP_INPUT_SELECT property inputSelects} */
#define KYRV_INPUT_LINE		1	       /* {type val label LineIn title {input select line in} value-of KYRP_INPUT_SELECT property inputSelects} */
#define KYRP_INPUT_LEVEL	(KYRP_CODEC+2) /* {type nrpn label InLvl title {input level} range {-128 24} unit dB/10 property inputLevel} */

#define KYRP_SOFT		(KYRP_CODEC+3) /* {type rel title {base of soft params}} */

#define KYRP_BUTTON_0		(KYRP_SOFT+0) /* {type nrpn label But0 title {headset button 0 - none pressed} range {-8192 8191} unit pp8191 property buttonLevel0} */
#define KYRP_BUTTON_1		(KYRP_SOFT+1) /* {type nrpn label But1 title {headset button 1 - center or only pressed} range {-8192 8191} unit pp8191 property buttonLevel1} */
#define KYRP_BUTTON_2		(KYRP_SOFT+2) /* {type nrpn label But2 title {headset button 2 - up pressed} range {-8192 8191} unit pp8191 property buttonLevel2}  */
#define KYRP_BUTTON_3		(KYRP_SOFT+3) /* {type nrpn label But3 title {headset button 3 - down pressed} range {-8192 8191} unit pp8191 property buttonLevel3}  */
#define KYRP_BUTTON_4		(KYRP_SOFT+4) /* {type nrpn label But4 title {headset button 4 - hey pressed} range {-8192 8191} unit pp8191 property buttonLevel4}  */
#define KYRP_PTT_ENABLE		(KYRP_SOFT+5) /* {type nrpn label PTTReq title {require EXT_PTT to transmit} range {0 1} ignore 1 property externalPTTRequire} */
#define KYRP_IQ_ENABLE		(KYRP_SOFT+6) /* {type nrpn label {IQ mode} title {Mode of IQ sample generation} values KYRV_IQ_* property iqModeSelect} */
#define KYRV_IQ_NONE		0 /* {type val label None title {no IQ} property iqModes value-of KYRP_IQ_ENABLE} */
#define KYRV_IQ_LSB		1 /* {type val label LSB title {IQ for lower sideband} property iqModes value-of KYRP_IQ_ENABLE} */
#define KYRV_IQ_USB		2 /* {type val label USB title {IQ for upper sideband} property iqModes value-of KYRP_IQ_ENABLE} */
#define KYRP_IQ_ADJUST		(KYRP_SOFT+7) /* {type nrpn label IQAdj title {adjustment to IQ phase} range {-8192 8191} unit pp8191 ignore 1 property iqAdjustPhase} */
#define KYRP_TX_ENABLE		(KYRP_SOFT+8) /* {type nrpn label TXEn title {soft enable TX} range {0 1} property txEnable} */
#define KYRP_ST_ENABLE		(KYRP_SOFT+9) /* {type nrpn label STEn title {enable sidetone generation} range {0 1} property sidetoneEnable} */
#define KYRP_IQ_BALANCE		(KYRP_SOFT+10) /* {type nrpn label IQBal title {adjustment to IQ balance} range {-8192 8191} unit pp8191 ignore 1 property iqAdjustBalance} */
#define KYRP_ST_PAN		(KYRP_SOFT+11) /* {type nrpn label STPan title {sidetone pan left or right} range {-8192 8191} unit pp8191 ignore 1 property sidetonePan} */
#define KYRP_OUT_ENABLE		(KYRP_SOFT+12) /* {type nrpn sub short label OutMix title {output mixer enable bits, shorthand} range {0 4095} property outputEnable} */
#define KYRP_REMOTE_KEY		(KYRP_SOFT+13) /* {type nrpn label Remote title {enable direct remote control of tune by midi note} range {0 1} property remoteKey} */

#define KYRP_COMM		(KYRP_SOFT+14) /* {type rel title {keyer parameters shared across fists}} */

#define KYRP_DEBOUNCE		(KYRP_COMM+0) /* {type nrpn label Deb title {debounce period} range {0 16383} unit µs property debouncePeriod} */

#define KYRP_PTT		(KYRP_COMM+1) /* {type rel title {PTT timing parameters}} */

#define KYRP_HEAD_TIME		(KYRP_PTT+0) /* {type nrpn label PTTHead title {time ptt should lead key, key delay} range {0 16383} unit sample property pttHeadTime} */
#define KYRP_TAIL_TIME		(KYRP_PTT+1) /* {type nrpn label PTTTail title {time ptt should linger after key} range {0 16383} unit sample property pttTailTime} */
#define KYRP_HANG_TIME		(KYRP_PTT+2) /* {type nrpn label PTTHang title {time in dits ptt should linger after key} range {0 127} unit dit property pttHangTime} */

#define KYRP_RAMP		(KYRP_PTT+3) /* {type rel title {base of the keyer ramp parameters}} */

#define KYRP_RISE_TIME		(KYRP_RAMP+0)	/* {type nrpn title {key rise ramp length} range {0 16383} unit sample property keyerRiseTime} */
#define KYRP_FALL_TIME		(KYRP_RAMP+1)	/* {type nrpn title {key fall ramp length} range {0 16383} unit sample property keyerFallTime} */
#define KYRP_RISE_RAMP		(KYRP_RAMP+2)	/* {type nrpn title {key rise ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN property keyerRiseRamp valuesProperty keyerRamps} */
#define KYRP_FALL_RAMP		(KYRP_RAMP+3)	/* {type nrpn title {key fall ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN property keyerFallRamp valuesProperty keyerRamps} */
#define KYRV_RAMP_HANN		0 /* {type val label Hann title {ramp from Hann window function, raised cosine} value-of {KYRP_*_RAMP} property keyerRamps} */
#define KYRV_RAMP_BLACKMAN_HARRIS 1 /* {type val label {Blackman Harris} title {ramp from Blackman Harris window function} value-of {KYRP_*_RAMP} property keyerRamps} */
#define KYRV_RAMP_LINEAR	2 /* {type val label Linear title {linear ramp, for comparison} value-of {KYRP_*_RAMP} property keyerRamps} */

#define KYRP_PAD		(KYRP_RAMP+4) /* {type rel title {base of paddle keyer parameters}} */

/* keyer parameters - for paddle keyers - this all could fit into one word 2+1+2+1+1+2 == 9 bits  */
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

#define KYRP_CHAN		(KYRP_PAD+6) /* {type rel title {base of midi channels}} */

#define KYRP_CHAN_CC		(KYRP_CHAN+0) /* {type nrpn title {midi channel for simple controls} unit {} range {0 16} property channelCC note obsolete} */
#define KYRP_CHAN_NOTE		(KYRP_CHAN+1) /* {type nrpn title {midi channel for notes} unit {} range {0 16} property channelNote note obsolete} */
#define KYRP_CHAN_NRPN		(KYRP_CHAN+2) /* {type nrpn title {midi channel for NRPN format controls} unit {} range {0 16} property channelNrpn note obsolete} */
#define KYRV_CHAN_INVALID	0	       /* {type val title {invalid channel, used to disable midi channel}} */

/* I think these note translations should go away */
#define KYRP_NOTE		(KYRP_CHAN+3) /* {type rel title {base of midi notes}} */

#define KYRP_NOTE_KEY_OUT	(KYRP_NOTE+0) /* {type nrpn title {note key output signal} unit {} range {0 128} property noteKeyOut} */
#define KYRP_NOTE_PTT_OUT	(KYRP_NOTE+1) /* {type nrpn title {note for ptt output signal} unit {} range {0 128} property notePTTOut} */
#define KYRP_NOTE_TUNE		(KYRP_NOTE+2) /* {type nrpn title {note to key tune signal input} unit {} range {0 128} property noteTune} */
#define KYRP_NOTE_L_PAD		(KYRP_NOTE+3) /* {type nrpn title {note for left paddle switch} unit {} range {0 128}  property noteLeftPaddle} */
#define KYRP_NOTE_R_PAD		(KYRP_NOTE+4) /* {type nrpn title {note for right paddle switch} unit {} range {0 128} property noteRightPaddle} */
#define KYRP_NOTE_S_KEY		(KYRP_NOTE+5) /* {type nrpn title {note for straight key switch} unit {} range {0 128} property noteStraightKey} */
#define KYRP_NOTE_EXT_PTT	(KYRP_NOTE+6) /* {type nrpn title {note for external ptt switch} unit {} range {0 128} property noteExternalPTT} */
#define KYRP_NOTE_ENABLE	(KYRP_NOTE+7)  /* {type nrpn title {bit mask which enables notes to be sent over usbMIDI} values KYRV_NOTE_* property noteEnable valuesProperty noteEnables} */
#define KYRV_NOTE_KEY_OUT	(0) /* {type val label KeyOut title {note for key/ptt key output} property noteEnables} */
#define KYRV_NOTE_PTT_OUT	(1) /* {type val label PttOut title {note for key/ptt ptt output} property noteEnables} */
#define KYRV_NOTE_TUNE		(2) /* {type val label Tune title {note to key tune signal} property noteEnables} */
#define KYRV_NOTE_L_PAD		(3) /* {type val label LPad title {note for left paddle switch input} property noteEnables} */
#define KYRV_NOTE_R_PAD		(4) /* {type val label RPad title {note for right paddle switch input} property noteEnables} */
#define KYRV_NOTE_S_KEY		(5) /* {type val label SKey title {note for straight key switch input} property noteEnables} */
#define KYRV_NOTE_EXT_PTT	(6) /* {type val label ExtPtt title {note for external ptt switch input} property noteEnables} */

#define KYRP_PINS		(KYRP_NOTE+8) /* {type rel title {base of hardware pin assignments}} */

#define KYRP_ADC0_CONTROL	(KYRP_PINS+0) /* {type nrpn title {property for adc0 = A0} unit {} values KYRV_ADC_* property adc0Control valuesProperty adcControls} */
#define KYRP_ADC1_CONTROL	(KYRP_PINS+1) /* {type nrpn title {property for adc1 = A1 (master volume on the CWKeyer)} unit {} values KYRV_ADC_* property adc1Control valuesProperty adcControls} */
#define KYRP_ADC2_CONTROL	(KYRP_PINS+2) /* {type nrpn title {property for adc2 = A2 (sidetone volume on the CWKeyer)} unit {} values KYRV_ADC_* property adc2Control valuesProperty adcControls} */
#define KYRP_ADC3_CONTROL	(KYRP_PINS+3) /* {type nrpn title {property for adc3 = A3 (sidetone frequency on the CWKeyer} unit {} values KYRV_ADC_* property adc4Control valuesProperty adcControls} */
#define KYRP_ADC4_CONTROL	(KYRP_PINS+4) /* {type nrpn title {property for adc4 = A8 (speed on the CWKeyer)} unit {} values KYRV_ADC_* property adc3Control valuesProperty adcControls} */
#define KYRP_ADC_ENABLE		(KYRP_PINS+5) /* {type nrpn title {enable adc channels} label {ADCEn} range {0 1} property adcEnable} */
/* KYRV_ADC_* is much later because it includes KYRP_* values not yet defined */

/* 64 morse code translations */
/* morse table for (7 bit ascii)-33, covers ! through `, with many holes */
/* lower case alpha are mapped to upper case on input */
/* coded in a neat way that I can't remember off hand */
#define KYRP_MORSE		(KYRP_PINS+6) /* {type rel title {morse code table base}} */

// #define KYRV_MORSE_TABLE "!\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"

/* 24 output mixer levels */
/* left and right channel mixers for four channels for each of usb_out, i2s_out, and mqs_out */
/* the incoming channels are usb_in audio raw, usb_in audio muted by sidetone, i2s in audio, sidetone */
/* relocation base */
#define KYRP_MIXER		(KYRP_MORSE+64) /* {type rel title {base of output mixer block}} */

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

#define KYRP_KEYER		(KYRP_MIXER+24) /* {type rel title {base of vox specialized keyer parameters}} */

#define KYRP_TONE		(KYRP_KEYER+0) /* {type nrpn title {sidetone and IQ oscillator frequency} range {0 16383} unit Hz/10 property keyerTone} */
#define KYRP_LEVEL		(KYRP_KEYER+1) /* {type nrpn title {sidetone level} range {-128 24} default 0 unit dB/10 property keyerLevel} */
#define KYRP_SPEED		(KYRP_KEYER+2) /* {type nrpn title {keyer speed control} range {0 16384} unit WPM property keyerSpeed} */
#define KYRP_WEIGHT		(KYRP_KEYER+3) /* {type nrpn title {keyer mark/space weight} range {25 75} unit % default 50 property keyerWeight} */
#define KYRP_RATIO		(KYRP_KEYER+4) /* {type nrpn title {keyer dit/dah ratio} range {25 75} unit % default 50 property keyerRatio} */
#define KYRP_FARNS		(KYRP_KEYER+5) /* {type nrpn title {Farnsworth keying speed} range {0 127} default 0 unit WPM property keyerFarnsworth} */
#define KYRP_COMP		(KYRP_KEYER+6) /* {type nrpn title {keyer compensation} range {-8192 8191} default 0 unit sample property keyerCompensation} */
#define KYRP_SPEED_FRAC		(KYRP_KEYER+7) /* {type nrpn title {keyer speed fraction} range {0 127} default 0 unit WPM/128 property keyerSpeedFraction} */

/* From here on we're using newly assigned numbers */
/* KYRP_X* are extended NRPNs, they encode 28 bits into two adjacent NRPN registers */
#define KYRP_XPER_DIT		(KYRP_KEYER+8) /* {type nrpn sub ext title {samples per dit}} */
#define KYRP_XPER_DAH		(KYRP_KEYER+10) /* {type nrpn sub ext title {samples per dah}} */
#define KYRP_XPER_IES		(KYRP_KEYER+12) /* {type nrpn sub ext title {samples per inter element space}} */
#define KYRP_XPER_ILS		(KYRP_KEYER+14) /* {type nrpn sub ext title {samples per inter letter space}} */
#define KYRP_XPER_IWS		(KYRP_KEYER+16) /* {type nrpn sub ext title {samples per inter word space}} */

/* here on we're inserting my most recent suggestions for the common core NRPNS */
#define KYRP_KEYER_LAST		(KYRP_KEYER+18) /* {type rel title {end of keyer block}} */

/* definitions of ADC targets, these had undefined symbols when placed immediately after KYRP_ADC* */
#define KYRV_ADC_NOTHING	(KYRP_NOTHING) /* {type val label None title {pot controls nothing} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_VOLUME		(KYRP_VOLUME) /* {type val label Volume title {pot controls master volume} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_LEVEL		(KYRP_LEVEL) /* {type val label Level title {pot controls sidetone level} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_TONE		(KYRP_TONE) /* {type val label Tone title {pot controls sidetone pitch} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_SPEED		(KYRP_SPEED) /* {type val label Speed title {pot controls keyer speed} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_FARNS		(KYRP_FANRS) /* {type val label Speed title {pot controls keyer Farnsworh speed} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_COMP		(KYRP_COMP) /* {type val label {} title {}} */
#define KYRV_ADC_DEBOUNCE	(KYRP_DEBOUNCE) /* {type val label {} title {}} */
#define KYRV_ADC_HEAD_TIME	(KYRP_HEAD_TIME) /* {type val label KYRP_HEAD_TIME title KYRP_HEAD_TIME} */
#define KYRV_ADC_TAIL_TIME	(KYRP_TAIL_TIME) /* {type val label KYRP_TAIL_TIME title KYRP_TAIL_TIME} */
#define KYRV_ADC_RISE_TIME	(KYRP_RISE_TIME) /* {type val label KYRP_RISE_TIME title KYRP_RISE_TIME} */
#define KYRV_ADC_FALL_TIME	(KYRP_FALL_TIME) /* {type val label KYRP_FALL_TIME title KYRP_FALL_TIME} */
#define KYRV_ADC_WEIGHT		(KYRP_WEIGHT) /* {type val label KYRP_WEIGHT title KYRP_WEIGHT} */
#define KYRV_ADC_RATIO		(KYRP_RATIO) /* {type val label KYRP_RATIO title KYRP_RATIO} */
#define KYRV_ADC_INPUT_LEVEL	(KYRP_INPUT_LEVEL) /* {type val label KYRP_INPUT_LEVEL title KYRP_INPUT_LEVEL} */

#define KYRP_INSERT		(KYRP_KEYER_LAST) /* {type rel title {common core NRPN block}} */

#define KYRP_N_NRPN             (KYRP_INSERT+0) /* {type nrpn sub info title {number of NRPNs} property keyerNNrpn} */
#define KYRP_QUERY              (KYRP_INSERT+1) /* {type nrpn sub cmd title {take value as NRPN and return its value, or no response} property keyerQuery} */
#define KYRP_UNSET              (KYRP_INSERT+2) /* {type nrpn sub cmd title {take the value as a nrpn number and make that nrpn unset} property keyerUnset} */
#define KYRP_CHANNEL		(KYRP_INSERT+3) /* {type nrpn title {the MIDI channel} property keyerChannel} */
#define KYRP_ID_JSON		(KYRP_INSERT+4) /* {type nrpn sub cmd title {send the JSON string which describes the keyer} property keyerJSON} */
#define KYRP_STRING_BEGIN	(KYRP_INSERT+5) /* {type nrpn sub cmd title {begin a string transfer} property keyerStringBegin} */
#define KYRP_STRING_END		(KYRP_INSERT+6) /* {type nrpn sub cmd title {finish a string transfer} property keyerStringEnd} */
#define KYRP_STRING_BYTE	(KYRP_INSERT+7) /* {type nrpn sub cmd title {transfer a byte for a string} propertyStringByte} */
#define KYRP_N_NOTE		(KYRP_INSERT+8) /* {type nrpn sub info title {number of notes} property keyerNNote} */
#define KYRP_N_CTRL		(KYRP_INSERT+9) /* {type nrpn sub info title {number of controls} property keyerNCtrl} */
#define KYRP_ACTIVE_ST		(KYRP_INSERT+10) /* {type nrpn sub info title {note number of active sidetone source} property keyerActiveSidetone} */

#define KYRP_MIXER2		(KYRP_INSERT+11) /* {type rel title {base of output mixer block}} */

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

#define KYRP_MIX_SLEW_RAMP	(KYRP_MIXER2+24) /* {type nrpn title {slew ramp for mixer changes} values KYRV_RAMP_* default KYRV_RAMP_HANN property mixerSlewRamp valuesProperty keyerRamps} */
#define KYRP_MIX_SLEW_TIME	(KYRP_MIXER2+25) /* {type nrpn title {slew time for mixer changes} range {0 16383} unit sample property mixerSlewTime} */
#define KYRP_OUT_ENABLE_L	(KYRP_MIXER2+26) /* {type nrpn sub short label OutMixL title {output mixer left enable bits, shorthand} range {0 4095} property outputEnableLeft} */
#define KYRP_OUT_ENABLE_R	(KYRP_MIXER2+27) /* {type nrpn sub short label OutMixR title {output mixer right enable bits, shorthand} range {0 4095} property outputEnableRight} */

/* relocate the exec and info blocks to contiguous */
#define KYRP_EXEC		(KYRP_MIXER2+28) /* {type rel title {base of command nrpns}} */

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

#define KYRP_LAST		(KYRP_INFO+6) /* {type rel title {end of nrpns}} */

#define KYR_N_NRPN		(KYRP_LAST)   /* {type def title {number of nrpns defined}} */

#define KYR_NRPN_SIZE		(KYR_N_NRPN*sizeof(int16_t))
#define KYR_MSG_SIZE		(EEPROM_BYTES-KYR_NRPN_SIZE-6*sizeof(int16_t))

/* end of defined nrpns */
/* end of config.h */
#endif
/*
** NOTES, CTRLS, NRPNs to add:
** FIX.ME KYRP_INPIN_?_PIN KYRP_INPIN_?_NOTE KYRP_INPIN_?_DEBOUNCE
** FIX.ME KYRP_INADC_?_PIN KYRP_INPIN_?_NRPN KYRP_INPIN_?_SAMPLERATE
** FIX.ME KYRP_MAPADC_?_NRPNI KYRP_MAPADC_NRPNO KYRP_MAPADC_?_LOW KYRP_MAPADC_?_HIGH
** FIX.ME KYRP_OUTPIN_?_PIN KYRP_OUTPIN_?_NOTE KYRP_OUTPIN_?_LOGIC
** FIX.ME rewrite Mixer4 to take ramp, time, mute, level from nrpn
*/
