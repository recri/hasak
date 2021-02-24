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

/* key vox priorities */
#define KYR_PRI_S_KEY	0
#define KYR_PRI_PAD	0
#define KYR_PRI_WINK	1
#define KYR_PRI_KYR	1

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
#define KYR_HEAD_SW	14	/* A0 */
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
#define KYR_HEAD_SW	14	/* A0 */
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
** Maybe use the NRPN_MSB NRPN_LSB bytes like this
** MSB = 0 -> global parameters, defaults for unspecified other parameters
** MSB = 1 -> parameter bank specific for paddle 1
** MSB = 2 -> parameter bank specific for straight key 1
** MSB = 3 -> parameter bank specific for Winkey keyed text
** MSB = 4 -> parameter bank specific for text keyed by keyer for op
** and so on for as many additional keyers.
**
** Use them like this
**  1) select NRPN with control change 98 and 99
**  2) then alter parameter value via
**  2a) increment with control change 96,
**  2b) decrement with control change 97
**  2c) set value with control change 6
**  2d) set fine tune value with control change 38
** Once 1) is done, any number/combination of 2a-2d) is permitted.
**
** I recall there being a pair of control change messages to set
** values to true and false,
*/

/* control change messages used */
#define KYR_CC_MSB	6	/* Data Entry (MSB) */
#define KYR_CC_LSB	38	/* Data Entry (LSB) */
#define KYR_CC_UP	96	/* Data Button increment */
#define KYR_CC_DOWN	97	/* Data Button decrement */
#define KYR_CC_NRPN_LSB	98	/* Non-registered Parameter (LSB) */
#define KYR_CC_NRPN_MSB	99	/* Non-registered Parameter (MSB) */

/* nrpn numbers used */

/* nrpn LSB numbers, item select */
/* these are NRPN's while apply to keyer voices, some voices do not use all of them */
#define KYRP_WPM	0	/* keyer speed control */
#define KYRP_WEIGHT	1	/* keyer weight */
#define KYRP_RATIO	2	/* keyer ratio */
#define KYRP_COMP	3	/* keyer compensation */
#define KYRP_FARNS	4	/* Farnsworth keying factor */
#define KYRP_ON_TIME	5	/* key ramp on µs */
#define KYRP_OFF_TIME	6	/* key ramp off µs */
#define KYRP_ON_RAMP	7	/* key ramp on window */
#define KYRP_OFF_RAMP	8	/* key ramp off window */
#define KYRP_TONE	9	/* oscillator frequency */
#define KYRP_TONE_NEG	10	/* treat frequency as negative */
#define KYRP_HEAD_TIME	11	/* time ptt should lead key */
#define KYRP_TAIL_TIME	12	/* time ptt should linger after key */
#define KYRP_PAD_MODE	13	/* paddle keyer mode A/B/S/U */
#define KYRP_PAD_SWAP	14	/* swap paddles T/F */
/* the following are computed from WPM, WEIGHT, RATIO, COMP, FARNS, and sample rate */
#define KYRP_DIT_LEN	15	/* dot mark length in samples */
#define KYRP_DAH_LEN	16	/* dash mark length in samples */
#define KYRP_IES_LEN	17	/* inter-element space length in samples */
#define KYRP_ILS_LEN	18	/* inter-letter space length in samples, reduced by IES. */
#define KYRP_IWS_LEN	19	/* inter-word space length in samples, reduced by IES+ILS */
/* the following are computed from ON_TIME, OFF_TIME, HEAD_TIME, TAIL_TIME, and sample rate */
#define KYRP_ON_SAMP	20	/* key ramp on samples */
#define KYRP_OFF_SAMP	21	/* key ramp off samples */
#define KYRP_HEAD_SAMP	22	/* samples ptt should lead key  */
#define KYRP_TAIL_SAMP	23	/* samples ptt should linger after key */

/* the following are global only parameters, they don't specialize by vox */
#define KYRP_FIRST_GLOBAL	24	/* relocation base */
#define KYRP_OUTLVL	(KYRP_FIRST_GLOBAL+0)	/* sgl5000 output volume */
#define KYRP_AUDIO_MODE	(KYRP_FIRST_GLOBAL+1)	/* sound card operation mode */
#define KYRP_ST_PAN	(KYRP_FIRST_GLOBAL+2)	/* pan sidetone left or right */
#define KYRP_LAST_GLOBAL	26

/* nrpn MSB numbers, bank select */
#define KYRP_GLOBAL_OFFSET	0
#define KYRP_PAD_OFFSET		32
#define KYRP_S_KEY_OFFSET	64
#define KYRP_WIND_OFFSET	96
#define KYRP_KYR_OFFSET		128

/* identified special NRPN values */
/* slew ramps */
#define KYRP_RAMP_HANN			'H' /* ON_RAMP, OFF_RAMP */
#define KYRP_RAMP_BLACKMAN_HARRIS	'B' /* ON_RAMP, OFF_RAMP */
/* paddle keyer modes */
#define KYRP_MODE_A			'A' /* PAD_MODE */
#define KYRP_MODE_B			'B' /* PAD_MODE */
#define KYRP_MODE_S			'S' /* PAD_MODE */
