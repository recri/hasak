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
#ifndef config_h_
#define config_h_

/* KYR configuration */
/* KYRC_* are configured values */
/* KYRP_* are parameter numbers */
/* KYRV_* are parameter values */

/* Version of hasak */
#define KYRC_VERSION 100	/* {type def label {version number of the NRPN set implemented}} */

/* Magic number to identify valid eeprom contents */
#define KYRC_MAGIC   0xad5d	/* {type def label {magic number to identify valid EEPROM contents}} */

/* potentiometer input */
#define KYRC_ENABLE_POTS 1	/* {type conf label {Should we read potentiometers for volume, pitch, and speed}} */

/* enable hardware output */
//#define KYRC_ENABLE_HDW_OUT 1	/* {type conf label {Should we enable the hardware out channel}} */

/* enable hardware input */
//#define KYRC_ENABLE_ADC_IN 1	/* {type conf label {Should we enable a hardware input channel}} */

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
*/
#if defined(TEENSY4)
/* I2S 7, 8, 20, 21, 23 */
#define KYR_DIN		7	/* {type pin label {teensy 4.x i2s DIN pin}} */
#define KYR_DOUT	8	/* {type pin label {teensy 4.x i2s DOUT pin}} */
#define KYR_MCLK	23	/* {type pin label {teensy 4.x i2s MCLK pin}} */
#define KYR_BCLK	21	/* {type pin label {teensy 4.x i2s BCLK pin}} */
#define KYR_LRCLK	20	/* {type pin label {teensy 4.x i2s LRCLK pin}} */
/* I2C 18, 19 */
#define KYR_SCL		19	/* {type pin label {teensy 4.x i2c clock pin}} */
#define KYR_SDA		18	/* {type pin label {teensy 4.x i2c data pin}} */
/* Teensy 4.0, 4.1 MQS 10, 12 */
#define KYR_MQSR	10	/* {type pin label {teensy 4.x medium quality sound pin}} */
#define KYR_MQSL	12	/* {type pin label {teensy 4.x medium quality sound pin}} */
#elif defined(TEENSY3)
/* I2S 9, 11, 13, 22, 23 */
#define KYR_DIN		22	/* {type pin label {teensy 3.x i2s DIN pin}} */
#define KYR_DOUT	13	/* {type pin label {teensy 3.x i2s DOUT pin}} */
#define KYR_MCLK	11	/* {type pin label {teensy 3.x i2s MCLK pin}} */
#define KYR_BCLK	9	/* {type pin label {teensy 3.x i2s BCLK pin}} */
#define KYR_LRCLK	23	/* {type pin label {teensy 3.x i2s LRCLK pin}} */
/* I2C 18, 19 */
#define KYR_SCL		19	/* {type pin label {teensy 3.x i2c clock pin}} */
#define KYR_SDA		18	/* {type pin label {teensy 3.x i2c data pin}} */
/* DAC */
#if defined(TEENSY30) || defined(TEENSY31) || defined(TEENSY32)
/* Teensy 3.1, 3.2  DAC A14 */
#define KYR_DAC		40	/* {type pin analog A14 label {teensy 3.[0-2] hardware dac pin} ignore 1} */
#endif
#if defined(TEENSY35) || defined(TEENSY36)
/* Teensy 3.5, 3.6 DAC A21, A22 */
#define KYR_DAC0	66	/* {type pin analog A21 label {teens 3.[56] hardware dac pin} ignore 1} */
#define KYR_DAC1	67	/* {type pin analog A22 label {teens 3.[56] hardware dac pin} ignore 1} */
#endif
/* Teensy LC DAC 26/A12 */
#endif

/* 
** pin allocations for standard input switches and output latches
*/
#define KYR_R_PAD_PIN	0	/* {type pin label {right paddle input pin}} */
#define KYR_L_PAD_PIN	1	/* {type pin label {left paddle input pin}} */
#define KYR_S_KEY_PIN	2	/* {type pin label {straight key input pin}} */
#define KYR_EXT_PTT_PIN	3	/* {type pin label {external PTT switch input pin}} */
#define KYR_KEY_OUT_PIN	4	/* {type pin label {KEY output pin}} */
#define KYR_PTT_OUT_PIN	5	/* {type pin label {PTT output pin}} */

#define KYR_VOLUME_POT	15	/* {type pin label {volume pot input pin} analog A1} */
#define KYR_ST_VOL_POT	16	/* {type pin label {sidetone volume pot input pin} analog A2} */
#define KYR_ST_FREQ_POT	17	/* {type pin label {sidetone frequency pot input pin} analog A3} */
#define KYR_SPEED_POT	22	/* {type pin label {keyer speed pot input pin} analog A8} */

/*
** keyer voices
** A keyer vox is a key or paddle or text input that can produce
** a sidetone and a transmit output.
** Six default voices.
** Voices can have priority, lowest wins, and can be local, never transmitted,
** and can have their own keyer properties.
*/
#define KYR_N_VOX 6		/* {type def label {number of keyer voices}} */
#define KYR_VOX_NONE	0	/* {type def label {no active voice, default parameters for other voices}} */
#define KYR_VOX_TUNE	1	/* {type def label {tune switch}} */
#define KYR_VOX_S_KEY	2	/* {type def label {Straight Key}} */
#define KYR_VOX_PAD	3	/* {type def label {Paddle}} */
#define KYR_VOX_WINK	4	/* {type def label {Winkey Key}} */
#define KYR_VOX_KYR	5	/* {type def label {Kyr Key}} */
#define KYR_VOX_BUT	6	/* {type def label {headset button straight key}} */

/* 
** MIDI usage
*/
/* midi channel usage */
#define KYRC_CHAN_RECV_CC	1	/* {type def label {default input channel  for keyer control change}} */
#define KYRC_CHAN_SEND_CC	1	/* {type def label {default output channel for keyer control change}} */
#define KYRC_CHAN_SEND_NOTE_IN	1	/* {type def label {default output channel for keyer input notes}} */
#define KYRC_CHAN_SEND_NOTE_OUT	1	/* {type def label {default output channel for keyer output notes}} */
#define KYRC_CHAN_RECV_NOTE_IN	0	/* {type def label {default input channel for keyer input notes}} */
#define KYRC_CHAN_RECV_NOTE_OUT	0	/* {type def label {default input channel for keyer output notes}} */

/*
** midi note usage
** KYRP_SEND_MIDI controls whether any notes are sent at all.
** by default we send notes for the following input keys
** and transmit key events.
*/
#define KYR_L_PAD_NOTE		0      /* {type def label {note used to report raw left paddle switch}} */
#define KYR_R_PAD_NOTE		1      /* {type def label {note used to report raw right paddle switch}} */
#define KYR_S_KEY_NOTE		2      /* {type def label {note used to report raw straight key switch}} */
#define KYR_EXT_PTT_NOTE	3      /* {type def label {note used to report raw ptt switch}} */
#define KYR_KEY_OUT_NOTE	4      /* {type def label {note used to send external key signal}} */
#define KYR_PTT_OUT_NOTE	5      /* {type def label {note used to send external ptt signal}} */

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
*/

/* control change messages used */
#define KYR_CC_MSB	6	/* {type def label {MIDI control change Data Entry (MSB)}} */
#define KYR_CC_LSB	38	/* {type def label {MIDI control change Data Entry (LSB)}} */
#define KYR_CC_NRPN_LSB	98	/* {type def label {MIDI control change Non-registered Parameter (LSB)}} */
#define KYR_CC_NRPN_MSB	99	/* {type def label {MIDI control change Non-registered Parameter (MSB)}} */

/*
** nrpn assigments
** nrpns are organized into blocks: 
** KYRP_CODEC - global codec controls
** KYRP_SOFT  - soft global controls
** KYRP_MORSE - the 64 character morse code table
** KYRP_MIXER - the 24 output mixer levels
** KYRP_KEYER - the default keyer parameters
** KYRP_VOX_* - the voice specific keyer parameters
** KYRE_* - commands
** KYRI_* - information
*/
#define KYRV_NOT_SET -1		/* {type val label {16 bit not set value}} */
#define KYRV_MASK    0x3fff	/* {type val label {14 bit mask}} */
#define KYRV_SIGN(v) ((((int16_t)(v))<<2)>>2) /* {type mac label {sign extend masked 14 bit to 16 bit twos complement}} */

/* NRPN_GLOBAL_BLOCK - parameters controlling the entire keyer */
#define KYRP_GLOBAL		0 /* {type rel label {base of nrpns}} */

#define KYRP_CODEC		(KYRP_GLOBAL) /* {type rel label {base of codec nrpns}} */
#define KYRP_VOLUME		(KYRP_CODEC+0) /* {type par label {output volume} units {dB/2} range {0 127}} */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1) /* {type par label {input select} values {0 microphone 1 {line in}}} */
#define KYRP_MIC_PREAMP_GAIN	(KYRP_CODEC+2) /* {type par label {microphone preamp gain} ignore 1} */
#define KYRP_MUTE_HEAD_PHONES	(KYRP_CODEC+3) /* {type par label {head phone mute} range {0 1}} */
#define KYRP_MUTE_LINE_OUT	(KYRP_CODEC+4) /* {type par label {line out mute} range {0 1}} */
#define KYRP_LINE_IN_LEVEL	(KYRP_CODEC+5) /* {type par label {line in level} range {0 15} default 5 ignore 1} */
#define KYRP_LINE_OUT_LEVEL	(KYRP_CODEC+6) /* {type par label {line out level} range {13 31} default 29 ignore 1} */
#define KYRP_LINE_IN_LEVEL_L	(KYRP_CODEC+7) /* {type par label {left line in level} range {0 15} default 5 ignore 1} */
#define KYRP_LINE_IN_LEVEL_R	(KYRP_CODEC+8) /* {type par label {right line in level} range {0 15} default 5 ignore 1} */
#define KYRP_LINE_OUT_LEVEL_L	(KYRP_CODEC+9) /* {type par label {left line out level} range {13 31} default 29 ignore 1} */
#define KYRP_LINE_OUT_LEVEL_R	(KYRP_CODEC+10)	/* {type par label {right line out level} range {13 31} default 29 ignore 1} */

#define KYRP_SOFT		(KYRP_CODEC+11) /* {type rel label {base of soft params}} */

#define KYRP_BUTTON_0		(KYRP_SOFT+0) /* {type par label {headset button 0 - none pressed} range {-8192 8191} ignore 1} */
#define KYRP_BUTTON_1		(KYRP_SOFT+1) /* {type par label {headset button 1 - center or only pressed} range {-8192 8191} ignore 1} */
#define KYRP_BUTTON_2		(KYRP_SOFT+2) /* {type par label {headset button 2 - up pressed} range {-8192 8191} ignore 1}  */
#define KYRP_BUTTON_3		(KYRP_SOFT+3) /* {type par label {headset button 3 - down pressed} range {-8192 8191} ignore 1}  */
#define KYRP_BUTTON_4		(KYRP_SOFT+4) /* {type par label {headset button 4 - hey pressed} range {-8192 8191} ignore 1}  */
#define KYRP_PTT_ENABLE		(KYRP_SOFT+5) /* {type par label {require EXT_PTT to transmit} range {0 1}} */
#define KYRP_IQ_ENABLE		(KYRP_SOFT+6) /* {type par label {IQ mode} values KYRV_IQ_*} */
#define KYRV_IQ_NONE		0 /* {type val value-of KYRP_IQ_ENABLE label {no IQ}} */
#define KYRV_IQ_LSB		1 /* {type val value-of KYRP_IQ_ENABLE label {IQ for lower sideband}} */
#define KYRV_IQ_USB		2 /* {type val value-of KYRP_IQ_ENABLE label {IQ for upper sideband}} */
#define KYRP_IQ_ADJUST		(KYRP_SOFT+7) /* {type par label {adjustment to IQ phase} range {-8192 8191} units {tbd}} */
#define KYRP_TX_ENABLE		(KYRP_SOFT+8) /* {type par label {soft enable TX} range {0 1}} */
#define KYRP_ST_ENABLE		(KYRP_SOFT+9) /* {type par label {enable sidetone generation} range {0 1}} */
#define KYRP_IQ_BALANCE		(KYRP_SOFT+10) /* {type par label {adjustment to IQ balance} range {-8192 8191} units tbd} */
#define KYRP_ST_AUDIO_MODE	(KYRP_SOFT+11) /* {type par label {sidetone operation mode} values tbd cmt {what?}} */
#define KYRP_ST_PAN		(KYRP_SOFT+12) /* {type par label {sidetone pan left or right} values {-8192 8191}} */
#define KYRP_DEBOUNCE		(KYRP_SOFT+13) /* {type par label {debounce period} range {0 127} units ms/10} */
#define KYRP_COMP		(KYRP_SOFT+14)	/* {type par label {keyer compensation} range {0 16383} default 0 units µs} */

#define KYRP_CHAN		(KYRP_SOFT+15) /* {type rel label {base of midi channels}} */
#define KYRP_CHAN_SEND_CC	(KYRP_CHAN+0) /* {type par label {midi channel for sending controls} range {0 16}} */
#define KYRP_CHAN_RECV_CC	(KYRP_CHAN+1) /* {type par label {midi channle for receiving controls} range {0 16}} */
#define KYRP_CHAN_SEND_NOTE_IN	(KYRP_CHAN+2) /* {type par label {midi channel for sending input notes} range {0 16}} */
#define KYRP_CHAN_SEND_NOTE_OUT	(KYRP_CHAN+3) /* {type par label {midi channel for sending output notes} range {0 16}} */
#define KYRP_CHAN_RECV_NOTE_IN	(KYRP_CHAN+4) /* {type par label {midi channel for receiving input notes} range {0 16}} */
#define KYRP_CHAN_RECV_NOTE_OUT	(KYRP_CHAN+5) /* {type par label {midi channel for receiving output notes} range {0 16}} */
#define KYRV_CHAN_INVALID	0	       /* {type val label {invalid channel, used to disable midi channel}} */

#define KYRP_NOTE		(KYRP_CHAN+6) /* {type rel label {base of midi notes}} */
#define KYRP_L_PAD_NOTE		(KYRP_NOTE+0) /* {type par label {note for left paddle switch input} range {0 128}} */
#define KYRP_R_PAD_NOTE		(KYRP_NOTE+1) /* {type par label {note for right paddle switch input} range {0 128}} */
#define KYRP_S_KEY_NOTE		(KYRP_NOTE+2) /* {type par label {note for straight key switch input} range {0 128}} */
#define KYRP_EXT_PTT_NOTE	(KYRP_NOTE+3) /* {type par label {note for external ptt switch input} range {0 128}} */
#define KYRP_KEY_OUT_NOTE	(KYRP_NOTE+4) /* {type par label {note for key/ptt key output} range {0 128}} */
#define KYRP_PTT_OUT_NOTE	(KYRP_NOTE+5) /* {type par label {note for key/ptt ptt output} range {0 128}} */
#define KYRV_INVALID_NOTE	128	       /* {type val label {invalid note, used to disable midi events}} */

#define KYRP_PINS		(KYRP_NOTE+6) /* {type rel label {base of hardware pin assignments}} */
#define KYRP_VOLUME_POT		(KYRP_PINS+0) /* {type par label {pin for volume pot}} */
#define KYRP_ST_VOL_POT		(KYRP_PINS+1) /* {type par label {pin for sidetone volume pot} range {0 127}} */
#define KYRP_ST_FREQ_POT	(KYRP_PINS+2) /* {type par label {pin for sidetone frequency pot} range {0 127}} */
#define KYRP_SPEED_POT		(KYRP_PINS+3) /* {type par label {pin for keyer speed pot} range {0 127}} */
#define KYRV_INVALID_PIN	127	       /* {type val label {invalid pin to disable potentiometer} value-of KYRP_*_POT} */

/* 64 morse code translations */
/* morse table for (7 bit ascii)-33, covers ! through `, with many holes */
/* lower case alpha are mapped to upper case on input */
/* coded in a neat way that I can't remember off hand */
#define KYRP_MORSE		(KYRP_PINS+4) /* {type rel label {morse code table base}} */

/* 24 output mixer levels */
/* left and right channel mixers for four channels for each of usb_out, i2s_out, and mqs_out */
/* the incoming channels are usb_in audio raw, usb_in audio muted by sidetone, i2s in audio, sidetone */
/* relocation base */
#define KYRP_MIXER		(KYRP_MORSE+64) /* {type rel label {base of output mixer block}} */

#define KYRP_MIX_USB_L0		(KYRP_MIXER+0)	/* {type par label {i2s_in left to usb_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_USB_L1		(KYRP_MIXER+1)	/* {type par label {sidetone left to usb_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_USB_L2		(KYRP_MIXER+2)	/* {type par label {IQ left to usb_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_USB_L3		(KYRP_MIXER+3)  /* {type par label {usb_in left to usb_out left} range {0 127} unit {1/127th full scale}} */

#define KYRP_MIX_USB_R0		(KYRP_MIXER+4)	/* {type par label {i2s_in right to usb_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_USB_R1		(KYRP_MIXER+5)	/* {type par label {sidetone right to usb_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_USB_R2		(KYRP_MIXER+6)	/* {type par label {IQ right to usb_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_USB_R3		(KYRP_MIXER+7)  /* {type par label {usb_in right to usb_out right} range {0 127} unit {1/127th full scale}} */

#define KYRP_MIX_I2S_L0		(KYRP_MIXER+8)  /* {type par label {usb_in left to i2s_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_I2S_L1		(KYRP_MIXER+9)  /* {type par label {sidetone left to i2s_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_I2S_L2		(KYRP_MIXER+10) /* {type par label {IQ left to i2s_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_I2S_L3		(KYRP_MIXER+11) /* {type par label {i2s_in right to i2s_out right} range {0 127} unit {1/127th full scale}} */

#define KYRP_MIX_I2S_R0		(KYRP_MIXER+12)	/* {type par label {usb_in right to i2s_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_I2S_R1		(KYRP_MIXER+13)	/* {type par label {sidetone right to i2s_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_I2S_R2		(KYRP_MIXER+14) /* {type par label {IQ right to i2s_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_I2S_R3		(KYRP_MIXER+15) /* {type par label {i2s_in right to i2s_out right} range {0 127} unit {1/127th full scale}} */

#define KYRP_MIX_HDW_L0		(KYRP_MIXER+16) /* {type par label {usb_in left to hdw_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_HDW_L1		(KYRP_MIXER+17) /* {type par label {sidetone left to hdw_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_HDW_L2		(KYRP_MIXER+18) /* {type par label {IQ left to hdw_out left} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_HDW_L3		(KYRP_MIXER+19) /* {type par label {i2s_in left to hdw_out left} range {0 127} unit {1/127th full scale}} */

#define KYRP_MIX_HDW_R0		(KYRP_MIXER+20) /* {type par label {usb_in right to hdw_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_HDW_R1		(KYRP_MIXER+21) /* {type par label {sidetone right to hdw_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_HDW_R2		(KYRP_MIXER+22) /* {type par label {IQ right to hdw_out right} range {0 127} unit {1/127th full scale}} */
#define KYRP_MIX_HDW_R3		(KYRP_MIXER+23) /* {type par label {i2s_in right to hdw_out right} range {0 127} unit {1/127th full scale}} */

#define KYRP_KEYER		(KYRP_MIXER+24) /* {type rel label {base of keyer parameters}} */
/* keyer parameters - for all keys */
#define KYRP_TONE		(KYRP_KEYER+0) /* {type par label {sidetone and IQ oscillator frequency} range {0 16383}} */
#define KYRP_LEVEL		(KYRP_KEYER+1) /* {type par label {sidetone level} range {0 127} default 64} */
#define KYRP_HEAD_TIME		(KYRP_KEYER+2) /* {type par label {time ptt should lead key, key delay} range {0 127} unit ms} */
#define KYRP_TAIL_TIME		(KYRP_KEYER+3) /* {type par label {time ptt should linger after key} range {0 127} unit ms} */
#define KYRP_HANG_TIME		(KYRP_KEYER+4) /* {type par label {time in dits ptt should linger after key} units dits range {0 127}} */

#define KYRP_RAMP		(KYRP_KEYER+5) /* {type rel label {base of the keyer ramp parameters}} */

#define KYRP_RISE_TIME		(KYRP_RAMP+0)	/* {type par label {key rise ramp length} range {0 16383} units µs} */
#define KYRP_FALL_TIME		(KYRP_RAMP+1)	/* {type par label {key fall ramp length} range {0 16383} units µs} */
#define KYRP_RISE_RAMP		(KYRP_RAMP+2)	/* {type par label {key rise ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN} */
#define KYRP_FALL_RAMP		(KYRP_RAMP+3)	/* {type par label {key fall ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN} */
#define KYRV_RAMP_HANN		0 /* {type val label {ramp from Hann window, raised cosine} value-of {KYRP_*_RAMP}} */
#define KYRV_RAMP_BLACKMAN_HARRIS 1 /* {type val label {ramp from Blackman Harris window} value-of {KYRP_*_RAMP}} */
#define KYRV_RAMP_LINEAR	2 /* {type val label {linear ramp, for comparison} value-of {KYRP_*_RAMP}} */

/* keyer parameters - for paddle and text keys */
#define KYRP_TIME		(KYRP_RAMP+4) /* {type rel label {base of keyer element timing parameters}} */

#define KYRP_SPEED		(KYRP_TIME+0)	/* {type par label {keyer speed control} range {0 127} units wpm} */
#define KYRP_WEIGHT		(KYRP_TIME+1)	/* {type par label {keyer mark/space weight} range {0 127} default 64} */
#define KYRP_RATIO		(KYRP_TIME+2)	/* {type par label {keyer dit/dah ratio} range {0 127} default 64} */
#define KYRP_FARNS		(KYRP_TIME+3)	/* {type par label {Farnsworth keying speed} range {0 127} default 0 units wpm} */

/* keyer parameters - for paddle keyers - this all fits into one word 2+1+2+1+1+2 == 9 bits  */
#define KYRP_PAD		(KYRP_TIME+4) /* {type rel label {base of paddle keyer parameters}} */
#define KYRP_PAD_MODE		(KYRP_PAD+0)	/* {type par label {iambic keyer mode A/B/S} values KYRV_MODE_* default KYRV_MODE_A} */
#define KYRV_MODE_A		0 /* {type val label {paddle keyer iambic mode A} value-of {KYRP_PAD_MODE}} */
#define KYRV_MODE_B		1 /* {type val label {paddle keyer iambic mode B} value-of {KYRP_PAD_MODE}} */
#define KYRV_MODE_S		2 /* {type val label {paddle keyer bug mode} value-of {KYRP_PAD_MODE}} */
#define KYRP_PAD_SWAP		(KYRP_PAD+1)	/* {type par label {swap paddles} range {0 1} default 0} */
#define KYRP_PAD_ADAPT		(KYRP_PAD+2)	/* {type par label {paddle adapter normal/ultimatic/single lever} values KYRV_ADAPT_* default KYRV_ADAPT_NORMAL} */
#define KYRV_ADAPT_NORMAL	0 /* {type val label {paddle keyer unmodified} value-of {KYRP_PAD_ADAPT}} */
#define KYRV_ADAPT_ULTIMATIC	1 /* {type val label {paddle keyer modified to produce ultimatic keyer} value-of {KYRP_PAD_ADAPT}} */
#define KYRV_ADAPT_SINGLE	2 /* {type val label {paddle keyer modified to simulate single lever paddle} value-of {KYRP_PAD_ADAPT}} */
#define KYRP_AUTO_ILS		(KYRP_PAD+3) /* {type par label {automatic letter space timing} range {0 1} default 1} */
#define KYRP_AUTO_IWS		(KYRP_PAD+4)	/* {type par label {automatic word space timing} range {0 1} default 0} */
#define KYRP_PAD_KEYER		(KYRP_PAD+5) /* {type par label {paddle keyer implementation} values KYRV_KEYER_* default KYRV_KEYER_VK6PH} */
#define KYRV_KEYER_AD5DZ	0 /* {type val label {paddle keyer algorithm by ad5dz} value-of {KYRP_PAD_KEYER}}  */
#define KYRV_KEYER_K1EL		1 /* {type val label {paddle keyer algorithm by k1el} value-of {KYRP_PAD_KEYER}} */
#define KYRV_KEYER_ND7PA	2 /* {type val label {paddle keyer algorithm by nd7pa} value-of {KYRP_PAD_KEYER}} */
#define KYRV_KEYER_VK6PH	3 /* {type val label {paddle keyer algorithm by vk6ph} value-of {KYRP_PAD_KEYER}} */

/* keyer timings in samples for paddle and text keyers - scratch values */
#define KYRP_SPE		(KYRP_PAD+6) /* {type rel label {base of per sample timing scratch block}} */
#define KYRP_PER_DIT		(KYRP_SPE+0) /* {type scr label {samples per dit}} */
#define KYRP_PER_DAH		(KYRP_SPE+2) /* {type scr label {samples per dah}} */
#define KYRP_PER_IES		(KYRP_SPE+3) /* {type scr label {samples per inter element space}} */
#define KYRP_PER_ILS		(KYRP_SPE+4) /* {type scr label {samples per inter letter space}} */
#define KYRP_PER_IWS		(KYRP_SPE+5) /* {type scr label {samples per inter word space}} */
#define KYRP_KEYER_LAST		(KYRP_SPE+6) /* {type rel label {end of keyer block}} */
/* four (or more) repetitions of the keyer block for per voice customizations */
#define KYRP_VOX_OFFSET		(KYRP_KEYER_LAST-KYRP_KEYER) /* {type rel label {size of keyer parameter block}} */
#define KYRP_VOX_NONE		(KYRP_KEYER+KYR_VOX_NONE*KYRP_VOX_OFFSET) /* {type rel label {base of default keyer parameters}} */
#define KYRP_VOX_TUNE		(KYRP_KEYER+KYR_VOX_TUNE*KYRP_VOX_OFFSET) /* {type rel label {base of tune keyer parameters}} */
#define KYRP_VOX_S_KEY		(KYRP_KEYER+KYR_VOX_S_KEY*KYRP_VOX_OFFSET) /* {type rel label {base of straight key parameters}} */
#define KYRP_VOX_PAD		(KYRP_KEYER+KYR_VOX_PAD*KYRP_VOX_OFFSET) /* {type rel label {base of paddle keyer parameters}} */
#define KYRP_VOX_WINK		(KYRP_KEYER+KYR_VOX_WINK*KYRP_VOX_OFFSET) /* {type rel label {base of text from winkey parameters}} */
#define KYRP_VOX_KYR		(KYRP_KEYER+KYR_VOX_KYR*KYRP_VOX_OFFSET) /* {type rel label {base of text from hasak parameters}} */
#define KYRP_VOX_BUT		(KYRP_KEYER+KYR_VOX_BUT*KYRP_VOX_OFFSET) /* {type rel label {base of headset button keyer parameters}} */

/* end of the nrpns which store values in kyr_nrpn */
#define KYRP_LAST		(KYRP_VOX_BUT+KYRP_VOX_OFFSET) /* {type rel label {end of stored keyer parameters}} */

#define KYRP_EXEC		(KYRP_LAST) /* {type rel label {base of command nrpns}} */
#define KYRP_WRITE_EEPROM	(KYRP_EXEC+0) /* {type cmd label {write kyr_nrpn to eeprom}} */
#define KYRP_READ_EEPROM	(KYRP_EXEC+1) /* {type cmd label {read kyr_nrpn from eeprom}} */
#define KYRP_SET_DEFAULT	(KYRP_EXEC+2) /* {type cmd label {load kyr_nrpn with default values}} */
#define KYRP_ECHO_ALL		(KYRP_EXEC+3) /* {type cmd label {echo all set nrpns to midi}} */
#define KYRP_SEND_WINK		(KYRP_EXEC+4) /* {type cmd label {send character value to wink vox}} */
#define KYRP_SEND_KYR		(KYRP_EXEC+5) /* {type cmd label {send character value to kyr vox}} */
#define KYRP_MSG_INDEX		(KYRP_EXEC+6) /* {type cmd label {set index into kyr_msgs}} */
#define KYRP_MSG_WRITE		(KYRP_EXEC+7) /* {type cmd label {set kyr_msgs[index++]}} */
#define KYRP_MSG_READ		(KYRP_EXEC+8) /* {type cmd label {read kyr_msgs[index++]}} */
#define KYRP_PLAY_WINK		(KYRP_EXEC+9) /* {type cmd label {queue message by number through wink}} */
#define KYRP_PLAY_KYR		(KYRP_EXEC+10) /* {type cmd label {queue message by number through kyr}} */ 

#define KYRP_INFO		(KYRP_EXEC+11) /* {type rel label {base of information nrpns}} */
#define KYRP_VERSION		(KYRP_INFO+0) /* {type inf label {version of hasak nrpn set}} */
#define KYRP_NRPN_SIZE		(KYRP_INFO+1) /* {type inf label {size of kyr_nrpn}} */
#define KYRP_MSG_SIZE		(KYRP_INFO+2) /* {type inf label {send the size of kyr_msgs}} */

/* end of defined nrpns */
/* end of config.h */
#endif
