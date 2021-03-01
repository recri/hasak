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

/* configure hardware variants, not used as yet */
#define KYR_N_PAD 1		/* number of paddles connected */
#define KYR_N_S_KEY 1		/* number of straight keys connected */
#define KYR_N_PTT_SW 1		/* number of PTT switches connected */
#define KYR_N_KEY_OUT 1		/* number of key out connections */
#define KYR_N_PTT_OUT 1		/* number of PTT out connections */
#define KYR_N_VOX 4		/* number of keyer voices */

/* assign key vox identifiers */
#define KYR_VOX_NONE	0
#define KYR_VOX_S_KEY	1	/* Straight Key */
#define KYR_VOX_PAD	2	/* Paddle */
#define KYR_VOX_WINK	3	/* Winkey Key */
#define KYR_VOX_KYR	4	/* Kyr Key */

/* key vox priorities, lowest wins */
#define KYR_PRI_S_KEY	0
#define KYR_PRI_PAD	1
#define KYR_PRI_WINK	2
#define KYR_PRI_KYR	3

/* pins assigned for audio card interface */
#if defined(TEENSY4)
/* I2S 7, 8, 20, 21, 23 */
#define KYR_DIN		7
#define KYR_DOUT	8
#define KYR_MCLK	23
#define KYR_BCLK	21
#define KYR_LRCLK	20
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
#define KYR_L_PAD_PIN	0
#define KYR_R_PAD_PIN	1
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

/* nrpn numbers used */
/* these are NRPN's which apply to keyer voices, some voices do not use all of them */
#define KYRP_BASE	0
#define KYRP_WPM	(KYRP_BASE+0)	/* keyer speed control */
#define KYRP_WEIGHT	(KYRP_BASE+1)	/* keyer weight */
#define KYRP_RATIO	(KYRP_BASE+2)	/* keyer ratio */
#define KYRP_COMP	(KYRP_BASE+3)	/* keyer compensation */
#define KYRP_FARNS	(KYRP_BASE+4)	/* Farnsworth keying factor */

#define KYRP_ON_TIME	(KYRP_BASE+5)	/* key ramp on µs */
#define KYRP_OFF_TIME	(KYRP_BASE+6)	/* key ramp off µs */
#define KYRP_ON_RAMP	(KYRP_BASE+7)	/* key ramp on window */
#define KYRP_OFF_RAMP	(KYRP_BASE+8)	/* key ramp off window */
#define KYRP_TONE	(KYRP_BASE+9)	/* oscillator frequency */
#define KYRP_HEAD_TIME	(KYRP_BASE+10)	/* time us ptt should lead key, key delay */
#define KYRP_TAIL_TIME	(KYRP_BASE+11)	/* time us ptt should linger after key, hang time */
#define KYRP_PAD_MODE	(KYRP_BASE+12)	/* paddle iambic keyer mode A/B */
#define KYRP_PAD_SWAP	(KYRP_BASE+13)	/* swap paddles T/F */
#define KYRP_PAD_ADAPT	(KYRP_BASE+14)	/* paddle adapter normal/ultimatic/single lever */
#define KYRP_TONE_NEG	(KYRP_BASE+15)	/* treat frequency as negative, iq LSB vs USB */
#define KYRP_IQ_ADJUST	(KYRP_BASE+16)	/* adjustment to iq phase */

/* the following are global only parameters, they don't specialize by vox */

/* sgtl5000 control */
#define KYRP_CODEC	(KYRP_IQ_ADJUST+1)	/* relocation base */
#define KYRP_HEAD_PHONE_VOLUME	(KYRP_CODEC+0)	/* sgl5000 output volume */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1)	/* 0..1 input from microphone or line-in */
#define KYRP_MIC_GAIN		(KYRP_CODEC+2)	/* 0..63 dB */
#define KYRP_MUTE_HEAD_PHONES	(KYRP_CODEC+3)	/* 0..1 true or false */
#define KYRP_MUTE_LINE_OUT	(KYRP_CODEC+4) /* 0..1 true or false */
#define KYRP_LINE_IN_LEVEL	(KYRP_CODEC+5) /* 0..15 default 5 */
#define KYRP_LINE_OUT_LEVEL	(KYRP_CODEC+6) /* 13..31 default 29 */

/* 24 output mixer levels */
#define KYRP_MIXER	(KYRP_LINE_OUT_LEVEL+1)	/* relocation base */
#define KYRP_MIX_USB_L0	(KYRP_MIXER+0)
#define KYRP_MIX_USB_L1	(KYRP_MIXER+1)
#define KYRP_MIX_USB_L2	(KYRP_MIXER+2)
#define KYRP_MIX_USB_L3	(KYRP_MIXER+3)
#define KYRP_MIX_USB_R0	(KYRP_MIXER+4)
#define KYRP_MIX_USB_R1	(KYRP_MIXER+5)
#define KYRP_MIX_USB_R2	(KYRP_MIXER+6)
#define KYRP_MIX_USB_R3	(KYRP_MIXER+7)
#define KYRP_MIX_I2S_L0	(KYRP_MIXER+8)
#define KYRP_MIX_I2S_L1	(KYRP_MIXER+9)
#define KYRP_MIX_I2S_L2	(KYRP_MIXER+10)
#define KYRP_MIX_I2S_L3	(KYRP_MIXER+11)
#define KYRP_MIX_I2S_R0	(KYRP_MIXER+12)
#define KYRP_MIX_I2S_R1	(KYRP_MIXER+13)
#define KYRP_MIX_I2S_R2	(KYRP_MIXER+14)
#define KYRP_MIX_I2S_R3	(KYRP_MIXER+15)
#define KYRP_MIX_MQS_L0	(KYRP_MIXER+16)
#define KYRP_MIX_MQS_L1	(KYRP_MIXER+17)
#define KYRP_MIX_MQS_L2	(KYRP_MIXER+18)
#define KYRP_MIX_MQS_L3	(KYRP_MIXER+19)
#define KYRP_MIX_MQS_R0	(KYRP_MIXER+20)
#define KYRP_MIX_MQS_R1	(KYRP_MIXER+21)
#define KYRP_MIX_MQS_R2	(KYRP_MIXER+22)
#define KYRP_MIX_MQS_R3	(KYRP_MIXER+23)


/* relocation base */
#define KYRP_XTRA (KYRP_MIX_MQS_R3+1)
/* other things that we might control */
#define KYRP_AUDIO_MODE		/* sound card operation mode */
/* involves changing codec modes, mixer levels, etc.
 * 2x2 audio card means switch codec to line out and line in 
 * and mix off all channels that aren't tranferring i2s <-> usb
 */
#define KYRP_ST_PAN		/* pan sidetone left or right */
#define KYRP_SMIDI_PAD		/* send input paddle key events to midi notes */
#define KYRP_SMIDI_S_KEY	/* send input straight key events to midi notes */ midi notes as input key events */
#define KYRP_SMIDI_PTT_SW	/* send input ptt switch events to midi notes */
#define KYRP_SMIDI_KEY_OUT	/* send key_out as midi note */
#define KYRP_SMIDI_PTT_OUT	/* send ptt_out as midi note */

#define KYRP_LAST KYRP_XTRA	/* end of keyer NRPN */

/* nrpn MSB numbers, bank select */
#define KYRP_GLOBAL_OFFSET	0
#define KYRP_PAD_OFFSET		32
#define KYRP_S_KEY_OFFSET	64
#define KYRP_WIND_OFFSET	96
#define KYRP_KYR_OFFSET		128

/* identified special NRPN values */
/* slew ramps */
#define KYRP_RAMP_HANN			0 /* ON_RAMP, OFF_RAMP */
#define KYRP_RAMP_BLACKMAN_HARRIS	1 /* ON_RAMP, OFF_RAMP */
/* paddle keyer modes */
#define KYRP_MODE_A			0 /* PAD_MODE */
#define KYRP_MODE_B			1 /* PAD_MODE */
#define KYRP_MODE_S			2 /* PAD_MODE */
