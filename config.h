#ifndef config_h_
#define config_h_

/* KYR configuration */

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

/* keyer voices in order of priority */
#define KYR_N_VOX 4		/* number of keyer voices */
#define KYR_VOX_NONE	0
#define KYR_VOX_S_KEY	1	/* Straight Key */
#define KYR_VOX_PAD	2	/* Paddle */
#define KYR_VOX_WINK	3	/* Winkey Key */
#define KYR_VOX_KYR	4	/* Kyr Key */

/* pins assigned for audio card interface */
#if defined(TEENSY4)
/* I2S 7, 8, 20, 21, 23 */
#define KYR_DIN		7
#define KYR_DOUT	8
#define KYR_MCLK	23
#define KYR_BCLK	21
#define KYR_LRCLK	20
#define KYR_DUP_LRCLK	22	/* wire 20 to 22 and see if I get an LRCLK interrupt on the Teensy 4 w/out AudioAdapter */
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

/* pin assignments for input switches and output latches */
#define KYR_L_PAD_PIN	1
#define KYR_R_PAD_PIN	0
#define KYR_S_KEY_PIN	2
#define KYR_PTT_SW_PIN	3
#define KYR_KEY_OUT_PIN	4
#define KYR_PTT_OUT_PIN	5

/* iambic keyer selection, not used */

/* MIDI usage */
/* midi channel usage */
#define KYR_CHANNEL	1	/* default channel used for keyer */

/* midi note usage, undefine to not send */
#define KYR_L_PAD_NOTE		0      /* note used to report raw left paddle switch */
#define KYR_R_PAD_NOTE		1      /* note used to report raw right paddle switch */
#define KYR_S_KEY_NOTE		2      /* note used to report raw straight key switch */
#define KYR_PTT_SW_NOTE		3      /* note used to report raw ptt switch */
#define KYR_KEY_OUT_NOTE	4      /* note used to send external key signal */
#define KYR_PTT_OUT_NOTE	5      /* note used to send external ptt signal */

/*
** midi control change usage,
** rather than overloading assigned control change messages, which gets out
** of hand quickly, use NRPN's for all controls.  NRPN's are Non-Registered
** Parameter Numbers, so they are specific to particular hardware and don't
** need to match up to anybody else's list.  We will have a lot of parameters.
**
** Use them like this
**  1) select NRPN with control change 99 (NRPN_MSB) and 98 (NRPN_LSB)
**  2) set value with control change 6 (MSB) and 38 (LSB)
** The actual value set is 14 bits (MSB<<7)|LSB
** The value is stored when CC 38 (LSB) is received.
*/

/* control change messages used */
#define KYR_CC_MSB	6	/* Data Entry (MSB) */
#define KYR_CC_LSB	38	/* Data Entry (LSB) */
//#define KYR_CC_UP	96	/* Data Button increment */
//#define KYR_CC_DOWN	97	/* Data Button decrement */
#define KYR_CC_NRPN_LSB	98	/* Non-registered Parameter (LSB) */
#define KYR_CC_NRPN_MSB	99	/* Non-registered Parameter (MSB) */

/* nrpn numbers used, 280 total */
/* nrpns are organized into four blocks: NRPN_GLOBAL_BLOCK, NRPN_CODEC_BLOCK, NRPN_KEYER_BLOCK, NRPN_VOX_BLOCKS */
/* NRPN_GLOBAL_BLOCK - parameters controlling the entire keyer */
#define KYRP_GLOBAL		0

/* relocation base */
#define KYRP_CODEC		(KYRP_GLOBAL)	/* relocation base = 0 */
#define KYRP_HEAD_PHONE_VOLUME	(KYRP_CODEC+0)	/* sgl5000 output volume */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1)	/* 0..1 input from microphone or line-in */
#define KYRP_MIC_GAIN		(KYRP_CODEC+2)	/* 0..63 dB */
#define KYRP_MUTE_HEAD_PHONES	(KYRP_CODEC+3)	/* 0..1 true or false */
#define KYRP_MUTE_LINE_OUT	(KYRP_CODEC+4) /* 0..1 true or false */
#define KYRP_LINE_IN_LEVEL	(KYRP_CODEC+5) /* 0..15 default 5 */
#define KYRP_LINE_OUT_LEVEL	(KYRP_CODEC+6) /* 13..31 default 29 */
#define KYRP_LINE_IN_LEVEL_L	(KYRP_CODEC+7) /* 0..15 default 5 */
#define KYRP_LINE_IN_LEVEL_R	(KYRP_CODEC+8) /* 0..15 default 5 */
#define KYRP_LINE_OUT_LEVEL_L	(KYRP_CODEC+9) /* 13..31 default 29 */
#define KYRP_LINE_OUT_LEVEL_R	(KYRP_CODEC+10) /* 13..31 default 29 */

/* relocation base */
#define KYRP_SOFT		(KYRP_CODEC+16) /* == 16 */
/* other things that we might control */
#define KYRP_AUDIO_MODE		(KYRP_SOFT+0) /* sound card operation mode */
#define KYRP_ST_PAN		(KYRP_SOFT+1) /* pan sidetone left or right */
#define KYRP_SEND_MIDI		(KYRP_SOFT+2) /* send input paddle key events to midi notes */
#define KYRP_RECV_MIDI		(KYRP_SOFT+3) /* send input straight key events to midi notes */
#define KYRP_IQ_ENABLE		(KYRP_SOFT+7) /* 0,1,2 -> none, LSB, USB */
#define KYRP_IQ_ADJUST		(KYRP_SOFT+8) /* adjustment to iq phase, +/- units tbd */
#define KYRP_TX_ENABLE		(KYRP_SOFT+9) /* 0, 1 -> disable, enable */
#define KYRP_ST_ENABLE		(KYRP_SOFT+10) /* 0, 1 -> disable, enable  */

/* 64 morse code translations */
/* morse table for (7 bit ascii)-33, covers ! through ` */
/* relocation base */
#define KYRP_MORSE		(KYRP_SOFT+16) /* == 32 */

/* 24 output mixer levels */
/* left and right channel mixers for four channels for each of usb_out, i2s_out, and mqs_out */
/* the incoming channels are usb_in audio raw, usb_in audio muted by sidetone, i2s in audio, sidetone */
/* relocation base */
#define KYRP_MIXER		(KYRP_MORSE+64) /* == 96 */

#define KYRP_MIX_USB_L0		(KYRP_MIXER+0)	/* i2s in raw */
#define KYRP_MIX_USB_L1		(KYRP_MIXER+1)	/* i2s in unmuted by ptt_sw */
#define KYRP_MIX_USB_L2		(KYRP_MIXER+2)	/* sidetone */
#define KYRP_MIX_USB_L3		(KYRP_MIXER+3)
#define KYRP_MIX_USB_R0		(KYRP_MIXER+4)  /* ditto for right channel */
#define KYRP_MIX_USB_R1		(KYRP_MIXER+5)
#define KYRP_MIX_USB_R2		(KYRP_MIXER+6)
#define KYRP_MIX_USB_R3		(KYRP_MIXER+7)
#define KYRP_MIX_I2S_L0		(KYRP_MIXER+8)  /* usb in raw */
#define KYRP_MIX_I2S_L1		(KYRP_MIXER+9)  /* usb in muted by sidetone vox */
#define KYRP_MIX_I2S_L2		(KYRP_MIXER+10) /* sidetone */
#define KYRP_MIX_I2S_L3		(KYRP_MIXER+11)
#define KYRP_MIX_I2S_R0		(KYRP_MIXER+12) /* ditto for right channel */
#define KYRP_MIX_I2S_R1		(KYRP_MIXER+13)
#define KYRP_MIX_I2S_R2		(KYRP_MIXER+14)
#define KYRP_MIX_I2S_R3		(KYRP_MIXER+15)
#define KYRP_MIX_MQS_L0		(KYRP_MIXER+16) /* usb in raw */
#define KYRP_MIX_MQS_L1		(KYRP_MIXER+17) /* usb in muted by sidetone vox */
#define KYRP_MIX_MQS_L2		(KYRP_MIXER+18) /* sidetone  */
#define KYRP_MIX_MQS_L3		(KYRP_MIXER+19)
#define KYRP_MIX_MQS_R0		(KYRP_MIXER+20) /* ditto for right channel */
#define KYRP_MIX_MQS_R1		(KYRP_MIXER+21)
#define KYRP_MIX_MQS_R2		(KYRP_MIXER+22)
#define KYRP_MIX_MQS_R3		(KYRP_MIXER+23)

/* NRPN_KEYER_BLOCK - default keyer parameters *//* these are NRPN's which apply to keyer voices, some voices do not use all of them */
/* relocation base */
#define KYRP_KEYER		(KYRP_MIXER+24) /* == 120 */

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
#define KYRP_HEAD_TIME		(KYRP_KEYER+10)	/* time us ptt should lead key, key delay */
#define KYRP_TAIL_TIME		(KYRP_KEYER+11)	/* time us ptt should linger after key, hang time */

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

/* four (or more) repetitions of the keyer block for per voice customizations */
#define KYRP_VOX_1		(KYRP_KEYER+32)	 /* == 152 */
#define KYRP_VOX_2		(KYRP_KEYER+64)	 /* == 184 */
#define KYRP_VOX_3		(KYRP_KEYER+96)	 /* == 216 */
#define KYRP_VOX_4		(KYRP_KEYER+128) /* == 248 */

/* sgtl5000 control */
#define KYRP_LAST		(KYRP_VOX_4+32)	/* == 280 */

/* nrpn MSB numbers, bank select, not quite yet */
#define KYRP_GLOBAL_OFFSET	KYRP_KEYER
#define KYRP_S_KEY_OFFSET	KYRP_VOX_1
#define KYRP_PAD_OFFSET		KYRP_VOX_2
#define KYRP_WIND_OFFSET	KYRP_VOX_3
#define KYRP_KYR_OFFSET		KYRP_VOX_4

/* Named NRPN values */

/* slew ramps */
#define KYRP_RAMP_HANN			0 /* RISE_RAMP, FALL_RAMP */
#define KYRP_RAMP_BLACKMAN_HARRIS	1 /* not implemented */
#define KYRP_RAMP_LINEAR		2 /* not implemented */
#define KYRP_RAMP_SLINEAR		3 /* smoothed linear */

/* paddle keyer modes */
#define KYRP_MODE_A			0 /* PAD_MODE */
#define KYRP_MODE_B			1 /* PAD_MODE */
#define KYRP_MODE_S			2 /* PAD_MODE */

/* paddle adapter modes */
#define KYRP_ADAPT_NORMAL		0
#define KYRP_ADAPT_ULTIMATIC		1
#define KYRP_ADAPT_SINGLE		2

/* iq enable modes */
#define KYRP_IQ_NONE			0 /* IQ_ENABLE */
#define KYRP_IQ_LSB			1
#define KYRP_IQ_USB			2

#endif
