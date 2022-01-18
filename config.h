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
#define KYRC_VERSION 100

/* per sample interrupt source */
//#define KYRC_USE_LRCLK	1	/* Should we use LRCLK directly for 48k interrupts/second */
//#define KYRC_DUP_LRCLK	1	/* Should we duplicate LRCLK to another pin for 48k interrupts/second */
#define KYRC_USE_TIMER		1	/* Should we use an interval timer for 48k interrupts/second */
#if defined(KYRC_USE_LRCLK) + defined(KYRC_DUP_LRCLK) + defined(KYRC_USE_TIMER) > 1
#error "only one of KYRC_USE_LRCLK, KYRC_DUP_LRCLK, and KYRC_USE_TIMER should be defined"
#elif defined(KYRC_USE_LRCLK) + defined(KYRC_DUP_LRCLK) + defined(KYRC_USE_TIMER) < 1
#error "one of KYRC_USE_LRCLK, KYRC_DUP_LRCLK, and KYRC_USE_TIMER should be defined"
#endif

/* potentiometer input */
#define KYRC_ENABLE_POTS 1	/* Should we read potentiometers for volume, pitch, and speed */

/* enable hardware output */
//#define KYRC_ENABLE_HDW_OUT 1

/* enable hardware input */
//#define KYRC_ENABLE_ADC_IN 1

/* decide which Teensy we're running on */
#if defined(TEENSYDUINO)
  #if defined(ARDUINO_TEENSY40)
  #define TEENSY4 1
  #define TEENSY40 1
  #elif defined(ARDUINO_TEENSY41)
  #define TEENSY4 1
  #define TEENSY41 1
  #elif defined(ARDUINO_TEENSY31)
  #define TEENSY3 1
  #define TEENSY31 1
  #elif defined(ARDUINO_TEENSY32)
  #define TEENSY3 1
  #define TEENSY32 1
  #elif defined(ARDUINO_TEENSY35)
  #define TEENSY3 1
  #define TEENSY35 1
  #elif defined(ARDUINO_TEENSY36)
  #define TEENSY3 1
  #define TEENSY36 1
  #else
  #error "You're not building for a Teensy 4.1, 4.0, 3.6, 3.5, 3.2 or 3.1?"
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
#define KYR_DIN		7
#define KYR_DOUT	8
#define KYR_MCLK	23
#define KYR_BCLK	21
#define KYR_LRCLK	20
#if defined(KYRC_DUP_LRCLK)
#define KYR_DUP_LRCLK	25	/* wire 20 to 25 to get an LRCLK interrupt on the Teensy 4 */
#endif
/* I2C 18, 19 */
#define KYR_SCL		19
#define KYR_SDA		18
/* Teensy 4.0, 4.1 MQS 10, 12 */
#define KYR_MQSR	10
#define KYR_MQSL	12
/* Headset switch detect */
#define KYR_HEAD_SW	16	/* A2 */
#elif defined(TEENSY3)
/* I2S 9, 11, 13, 22, 23 */
#define KYR_DIN		22
#define KYR_DOUT	13
#define KYR_MCLK	11
#define KYR_BCLK	9
#define KYR_LRCLK	23
// this doesn't appear to be necessary on the Teensy 3, but I still need to test
//#define KYR_DUP_LRCLK	21	/* wire 20 to 22 and see if I get an LRCLK interrupt on the Teensy 3 w/out AudioAdapter */
/* I2C 18, 19 */
#define KYR_SCL		19
#define KYR_SDA		18
/* DAC */
/* Teensy 3.1, 3.2  DAC A14 */
#define KYR_DAC		A14
/* Teensy 3.5, 3.6 DAC A21, A22 */
#define KYR_DAC0	A21
#define KYR_DAC1	A22
/* Teensy LC DAC 26/A12 */
/* Headset switch detect */
#define KYR_HEAD_SW	16	/* A2 */
#endif

/* 
** pin allocations for standard input switches and output latches
*/
#define KYR_R_PAD_PIN	0	// [x] CWKeyer proto 
#define KYR_L_PAD_PIN	1	// [x] CWKeyer proto
#define KYR_S_KEY_PIN	2	// [x] CWKeyer proto
#define KYR_EXT_PTT_PIN	3	// [x] CWKeyer proto
#define KYR_KEY_OUT_PIN	4	// [x] CWKeyer proto
#define KYR_PTT_OUT_PIN	5	// [x] CWKeyer proto
#define KYR_VOLUME_POT	A1	// [x] CWKeyer proto = 
#define KYR_ST_VOL_POT	A2	// [x] CWKeyer proto =
#define KYR_ST_FREQ_POT	A3	// [x] CWKeyer proto =
#define KYR_SPEED_POT	A8	// [x] CWKeyer proto = 22!

/*
** whether to enable potentiometers or not
*/
//#define KYR_SHIFT_POTS  5	// right shift adc results to clear noise
//#define KYR_SKIP_READ	1	// reads to skip
//#define KYR_AVG_READ	2	// reads to average

/*
** whether to enable line level inputs/outputs
*/
#define KYR_ENABLE_LINE 0	// CWKeyer proto has no line level

/*
** keyer voices
** A keyer vox is a key or paddle or text input that can produce
** a sidetone and a transmit output.
** Four (or five) default voices.
** Voices can have priority, lowest wins, and can be local, never transmitted,
** and can have their own keyer properties.
*/
#define KYR_N_VOX 6		/* number of keyer voices */
#define KYR_VOX_NONE	0	/* no active voice */
#define KYR_VOX_TUNE	1	/* tune switch */
#define KYR_VOX_S_KEY	2	/* Straight Key */
#define KYR_VOX_PAD	3	/* Paddle */
#define KYR_VOX_WINK	4	/* Winkey Key */
#define KYR_VOX_KYR	5	/* Kyr Key */
#define KYR_VOX_BUT	6	/* headset button (not implemented) */

/* 
** MIDI usage
*/
/* midi channel usage */
#define KYRC_CC_CHAN_IN		1	/* default input channel used for keyer */
#define KYRC_CC_CHAN_OUT	1	/* default output channel used for keyer */
#define KYRC_NOTE_IN_CHAN_OUT	1
#define KYRC_NOTE_OUT_CHAN_OUT	1
#define KYRC_NOTE_IN_CHAN_IN	0
#define KYRC_NOTE_OUT_CHAN_IN	0

/*
** midi note usage
** KYRP_SEND_MIDI controls whether any notes are sent at all.
** by default we send notes for the following input keys
** and transmit key events.
*/
#define KYR_L_PAD_NOTE		0      /* note used to report raw left paddle switch */
#define KYR_R_PAD_NOTE		1      /* note used to report raw right paddle switch */
#define KYR_S_KEY_NOTE		2      /* note used to report raw straight key switch */
#define KYR_EXT_PTT_NOTE	3      /* note used to report raw ptt switch */
#define KYR_KEY_OUT_NOTE	4      /* note used to send external key signal */
#define KYR_PTT_OUT_NOTE	5      /* note used to send external ptt signal */

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
#define KYR_CC_MSB	6	/* Data Entry (MSB) */
#define KYR_CC_LSB	38	/* Data Entry (LSB) */
#define KYR_CC_NRPN_LSB	98	/* Non-registered Parameter (LSB) */
#define KYR_CC_NRPN_MSB	99	/* Non-registered Parameter (MSB) */

/*
** nrpn assigments
** nrpns are organized into blocks: 
** KYRP_CODEC - global codec controls
** KYRP_SOFT  - soft global controls
** KYRP_MORSE - the 64 character morse code table
** KYRP_MIXER - the 24 output mixer levels
** KYRP_KEYER - the default keyer parameters
** KYRP_VOX_* - the voice specific keyer parameters
*/

/* NRPN_GLOBAL_BLOCK - parameters controlling the entire keyer */
#define KYRP_GLOBAL		0		/* relocation */

/* relocation base */
#define KYRP_CODEC		(KYRP_GLOBAL)	/* relocation -> */
#define KYRP_HEAD_PHONE_VOLUME	(KYRP_CODEC+0)	/* sgl5000 output volume */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1)	/* 0..1 input from microphone or line-in */
#define KYRP_MIC_PREAMP_GAIN	(KYRP_CODEC+2)	/* 0-3 -> 0, 20, 30, 40 dB */
#define KYRP_MUTE_HEAD_PHONES	(KYRP_CODEC+3)	/* 0..1 true or false */
#define KYRP_MUTE_LINE_OUT	(KYRP_CODEC+4) /* 0..1 true or false */
#define KYRP_LINE_IN_LEVEL	(KYRP_CODEC+5) /* 0..15 default 5 */
#define KYRP_LINE_OUT_LEVEL	(KYRP_CODEC+6) /* 13..31 default 29 */
#define KYRP_LINE_IN_LEVEL_L	(KYRP_CODEC+7) /* 0..15 default 5 */
#define KYRP_LINE_IN_LEVEL_R	(KYRP_CODEC+8) /* 0..15 default 5 */
#define KYRP_LINE_OUT_LEVEL_L	(KYRP_CODEC+9) /* 13..31 default 29 */
#define KYRP_LINE_OUT_LEVEL_R	(KYRP_CODEC+10) /* 13..31 default 29 */
//#define KYRP_MIC_BIAS		(KYRP_CODEC+11) /* 0..7, 1.25 .. 3.00 V by 0.250 V steps */
//#define KYRP_MIC_IMPEDANCE	(KYRP_CODEC+12)	/* 0..3, 0 off, else 2^n kilohm */

/* relocation base */
#define KYRP_SOFT		(KYRP_CODEC+16) /* relocation -> */
/* other things that we might control */
#define KYRP_BUTTON_0		(KYRP_SOFT+0) /* headset button 0 - none pressed */
#define KYRP_BUTTON_1		(KYRP_SOFT+1) /* headset button 1 - center or only pressed */
#define KYRP_BUTTON_2		(KYRP_SOFT+2) /* headset button 2 - up pressed */
#define KYRP_BUTTON_3		(KYRP_SOFT+3) /* headset button 3 - down pressed */
#define KYRP_BUTTON_4		(KYRP_SOFT+4) /* headset button 4 - hey pressed */
//#define KYRP_SEND_MIDI		(KYRP_SOFT+5) /* send input paddle key events to midi notes */
//#define KYRP_RECV_MIDI		(KYRP_SOFT+6) /* receive and act on midi notes */
#define KYRP_PTT_ENABLE		(KYRP_SOFT+7) /* enable PTT to operate */
#define KYRP_IQ_ENABLE		(KYRP_SOFT+8) /* 0,1,2 -> none, LSB, USB */
#define KYRP_IQ_ADJUST		(KYRP_SOFT+9) /* adjustment to iq phase, +/- units tbd, excess 8096 */
#define KYRP_TX_ENABLE		(KYRP_SOFT+10) /* 0, 1 -> disable, enable */
#define KYRP_ST_ENABLE		(KYRP_SOFT+11) /* 0, 1 -> disable, enable  */
#define KYRP_IQ_BALANCE		(KYRP_SOFT+12) /*  adjustment to iq balance, +/- units tbd, excess 8096 */
#define KYRP_ST_AUDIO_MODE	(KYRP_SOFT+13) /* sound card operation mode */
#define KYRP_ST_PAN		(KYRP_SOFT+14) /* pan sidetone left or right */
#define KYRP_DEBOUNCE		(KYRP_SOFT+15) /* debounce period in ms/10 */

#define KYRP_CC_CHAN_OUT	(KYRP_SOFT+16) /* midi channel for sending controls */
#define KYRP_CC_CHAN_IN		(KYRP_SOFT+17) /* midi channle for receiving controls */
#define KYRP_NOTE_IN_CHAN_OUT	(KYRP_SOFT+18) /* midi channel for sending notes */
#define KYRP_NOTE_OUT_CHAN_OUT	(KYRP_SOFT+19) /* midi channel for sending notes */
#define KYRP_NOTE_IN_CHAN_IN	(KYRP_SOFT+20) /* midi channle for receiving notes */
#define KYRP_NOTE_OUT_CHAN_IN	(KYRP_SOFT+21) /* midi channle for receiving notes */

#define KYRP_VERSION		(KYRP_SOFT+22) /* version of hasak nrpn set */

#define KYRP_L_PAD_NOTE		(KYRP_SOFT+23+0)      /* note used to report raw left paddle switch */
#define KYRP_R_PAD_NOTE		(KYRP_SOFT+23+1)      /* note used to report raw right paddle switch */
#define KYRP_S_KEY_NOTE		(KYRP_SOFT+23+2)      /* note used to report raw straight key switch */
#define KYRP_EXT_PTT_NOTE	(KYRP_SOFT+23+3)      /* note used to report raw ptt switch */
#define KYRP_KEY_OUT_NOTE	(KYRP_SOFT+23+4)      /* note used to send external key signal */
#define KYRP_PTT_OUT_NOTE	(KYRP_SOFT+23+5)      /* note used to send external ptt signal */

/* 64 morse code translations */
/* morse table for (7 bit ascii)-33, covers ! through ` */
/* lower case alpha are mapped to upper case on input */
/* relocation base */
#define KYRP_MORSE		(KYRP_SOFT+32) /* relocation -> */

/* 24 output mixer levels */
/* left and right channel mixers for four channels for each of usb_out, i2s_out, and mqs_out */
/* the incoming channels are usb_in audio raw, usb_in audio muted by sidetone, i2s in audio, sidetone */
/* relocation base */
#define KYRP_MIXER		(KYRP_MORSE+64) /* relocation -> */

#define KYRP_MIX_USB_L0		(KYRP_MIXER+0)	/* i2s_in left */
#define KYRP_MIX_USB_L1		(KYRP_MIXER+1)	/* sidetone left */
#define KYRP_MIX_USB_L2		(KYRP_MIXER+2)	/* IQ left */
#define KYRP_MIX_USB_L3		(KYRP_MIXER+3)  /* usb_in */
#define KYRP_MIX_USB_R0		(KYRP_MIXER+4)  /* ditto for right channel */
#define KYRP_MIX_USB_R1		(KYRP_MIXER+5)
#define KYRP_MIX_USB_R2		(KYRP_MIXER+6)
#define KYRP_MIX_USB_R3		(KYRP_MIXER+7)
#define KYRP_MIX_I2S_L0		(KYRP_MIXER+8)  /* usb_in left */
#define KYRP_MIX_I2S_L1		(KYRP_MIXER+9)  /* sidetone left */
#define KYRP_MIX_I2S_L2		(KYRP_MIXER+10) /* IQ left */
#define KYRP_MIX_I2S_L3		(KYRP_MIXER+11) /* i2s_in left */
#define KYRP_MIX_I2S_R0		(KYRP_MIXER+12) /* ditto for right channel */
#define KYRP_MIX_I2S_R1		(KYRP_MIXER+13)
#define KYRP_MIX_I2S_R2		(KYRP_MIXER+14)
#define KYRP_MIX_I2S_R3		(KYRP_MIXER+15)
#define KYRP_MIX_HDW_L0		(KYRP_MIXER+16) /* usb_in left */
#define KYRP_MIX_HDW_L1		(KYRP_MIXER+17) /* sidetone left */
#define KYRP_MIX_HDW_L2		(KYRP_MIXER+18) /* IQ left */
#define KYRP_MIX_HDW_L3		(KYRP_MIXER+19) /* i2s_in left */
#define KYRP_MIX_HDW_R0		(KYRP_MIXER+20) /* ditto for right channel */
#define KYRP_MIX_HDW_R1		(KYRP_MIXER+21)
#define KYRP_MIX_HDW_R2		(KYRP_MIXER+22)
#define KYRP_MIX_HDW_R3		(KYRP_MIXER+23)

/* NRPN_KEYER_BLOCK - default keyer parameters */
/* these are NRPN's which apply to keyer voices, some voices do not use all of them */
/* relocation base */
#define KYRP_KEYER		(KYRP_MIXER+24) /* relocation -> */

/* keyer timing */
#define KYRP_SPEED		(KYRP_KEYER+0)	/* keyer speed control */
#define KYRP_WEIGHT		(KYRP_KEYER+1)	/* keyer weight */
#define KYRP_RATIO		(KYRP_KEYER+2)	/* keyer ratio */
#define KYRP_COMP		(KYRP_KEYER+3)	/* keyer compensation */
#define KYRP_FARNS		(KYRP_KEYER+4)	/* Farnsworth keying factor */

/* keyer ramp */
#define KYRP_RISE_TIME		(KYRP_KEYER+5)	/* key ramp on µs */
#define KYRP_FALL_TIME		(KYRP_KEYER+6)	/* key ramp off µs */
#define KYRP_RISE_RAMP		(KYRP_KEYER+7)	/* key ramp on window */
#define KYRP_FALL_RAMP		(KYRP_KEYER+8)	/* key ramp off window */

/* keyer oscillator */
#define KYRP_TONE		(KYRP_KEYER+9)	/* oscillator frequency */

/* keyer ptt timing */
#define KYRP_HEAD_TIME		(KYRP_KEYER+10)	/* time ms ptt should lead key, key delay */
#define KYRP_TAIL_TIME		(KYRP_KEYER+11)	/* time ms ptt should linger after key */

/* keyer modes */
#define KYRP_PAD_MODE		(KYRP_KEYER+12)	/* paddle iambic keyer mode A/B */
#define KYRP_PAD_SWAP		(KYRP_KEYER+13)	/* swap paddles T/F */
#define KYRP_PAD_ADAPT		(KYRP_KEYER+14)	/* paddle adapter normal/ultimatic/single lever */
#define KYRP_AUTO_ILS		(KYRP_KEYER+15) /* automatic letter space timing */
#define KYRP_AUTO_IWS		(KYRP_KEYER+16)	/* automatic word space timing */

/* keyer timings in samples - automatically computed */
#define KYRP_PER_DIT		(KYRP_KEYER+17) /* samples per dit */
#define KYRP_PER_DAH		(KYRP_KEYER+18) /* samples per dah */
#define KYRP_PER_IES		(KYRP_KEYER+19) /* samples per inter element space */
#define KYRP_PER_ILS		(KYRP_KEYER+20) /* samples per inter letter space */
#define KYRP_PER_IWS		(KYRP_KEYER+21) /* samples per inter word space */

#define KYRP_PAD_KEYER		(KYRP_KEYER+22) /* paddle keyer implementation */
#define KYRP_HANG_TIME		(KYRP_KEYER+23) /* time in dits ptt should linger after key */
#define KYRP_LEVEL		(KYRP_KEYER+24) /* sidetone level 0 .. 0x3FFF */

/* four (or more) repetitions of the keyer block for per voice customizations */
#define KYRP_VOX_0		(KYRP_KEYER+0*32) /* relocation -> default keyer */
#define KYRP_VOX_1		(KYRP_KEYER+1*32) /* relocation -> tune keyer */
#define KYRP_VOX_2		(KYRP_KEYER+2*32) /* relocation ->  straight key */
#define KYRP_VOX_3		(KYRP_KEYER+3*32) /* relocation -> paddle */
#define KYRP_VOX_4		(KYRP_KEYER+4*32) /* relocation -> text from winkey */
#define KYRP_VOX_5		(KYRP_KEYER+5*32) /* relocation -> text from keyer */
#define KYRP_VOX_6		(KYRP_KEYER+6*32) /* relocation -> headset button */

#define KYRP_LAST		(KYRP_KEYER+7*32) /* relocation */

/* nrpn MSB numbers, bank select, not quite yet */
#define KYRP_VOX_OFFSET		32
#define KYRP_GLOBAL_OFFSET	KYRP_KEYER
#define KYRP_TUNE_OFFSET	KYRP_VOX_1
#define KYRP_S_KEY_OFFSET	KYRP_VOX_2
#define KYRP_PAD_OFFSET		KYRP_VOX_3
#define KYRP_WIND_OFFSET	KYRP_VOX_4
#define KYRP_KYR_OFFSET		KYRP_VOX_5
#define KYRP_BUT_OFFSET		KYRP_VOX_6

/* 
** these are named NRPN values
*/

/* the unset value */
#define KYRV_NOT_SET			-1

/* slew ramps, values of KYRP_RISE_RAMP and KYRP_FALL_RAMP */
#define KYRV_RAMP_HANN			0 /* ramp from Hann window, raised cosine */
#define KYRV_RAMP_BLACKMAN_HARRIS	1 /* ramp from Blackman Harris window */
#define KYRV_RAMP_LINEAR		2 /* linear ramp, for comparison */

/* paddle keyer modes, values of KYRP_PAD_MODE */
#define KYRV_MODE_A			0 /* iambic mode A */
#define KYRV_MODE_B			1 /* iambic mode  */
#define KYRV_MODE_S			2 /* bug mode */

/* paddle adapter modes, values of KYRP_PAD_ADAPT */
#define KYRV_ADAPT_NORMAL		0 /* unmodified input paddles */
#define KYRV_ADAPT_ULTIMATIC		1 /* modified to produce ultimatic */
#define KYRV_ADAPT_SINGLE		2 /* modified to simulate single lever paddle */

/* paddle keyers, values of KYRP_PAD_KEYER */
#define KYRV_KEYER_AD5DZ		0 /* iambic keyer by ad5dz */
#define KYRV_KEYER_K1EL			1 /* iambic keyer by k1el */
#define KYRV_KEYER_ND7PA		2 /* iambic keyer by nd7pa */
#define KYRV_KEYER_VK6PH		3 /* iambic keyer by vk6ph */

/* iq enable modes, values of KYRP_IQ_ENABLE */
#define KYRV_IQ_NONE			0 /* no IQ */
#define KYRV_IQ_LSB			1 /* IQ for lower sideband */
#define KYRV_IQ_USB			2 /* IQ for upper sideband */

/* send midi enable modes, values KYRP_SEND_MIDI */
//#define KYRV_SM_NONE			0 /* no midi */
//#define KYRV_SM_OUTPUT			1 /* send output key events */
//#define KYRV_SM_INPUT			2 /* send input key events */
//#define KYRV_SM_BOTH			3 /* send input and output events */

#endif
