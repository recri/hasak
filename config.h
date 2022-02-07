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
/* KYRP_* are parameter numbers or relocation tags */
/* KYRV_* are parameter values */

/* Identifier of hasak */
#define KYRC_IDENT   0x0ad5	/* {type def title {identifier of the hasak keyer}} */

/* Version of hasak */
#define KYRC_VERSION 100	/* {type def title {version number of the NRPN set implemented}} */

/* Magic number to identify valid eeprom contents */
#define KYRC_MAGIC   0xad5d	/* {type def title {magic number to identify valid EEPROM contents}} */

/* potentiometer input */
#define KYRC_ENABLE_POTS 1	/* {type conf title {Should we read potentiometers for volume, pitch, and speed}} */

/* enable hardware output */
#define KYRC_ENABLE_HDW_OUT 1	/* {type conf title {Should we enable the hardware out channel}} */

/* enable hardware input */
//#define KYRC_ENABLE_ADC_IN 1	/* {type conf title {Should we enable a hardware input channel}} */

/* enable winkey emulator */
//#define KYRC_ENABLE_WINKEY 1  /* {type conf title {Should we enable the winkey emulator}} */

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
** keyer voices
** A keyer vox is a key or paddle or text input that can produce
** a sidetone and a transmit output.
** Six default voices.
** Voices can have priority, lowest wins, and can be local, never transmitted,
** and can have their own keyer properties.
*/
#define KYR_N_VOX 6		/* {type def title {number of keyer voices}} */
#define KYR_VOX_NONE	0	/* {type def title {no active voice, default parameters for other voices}} */
#define KYR_VOX_TUNE	1	/* {type def title {tune switch}} */
#define KYR_VOX_S_KEY	2	/* {type def title {Straight Key}} */
#define KYR_VOX_PAD	3	/* {type def title {Paddle}} */
#define KYR_VOX_WINK	4	/* {type def title {Winkey Key}} */
#define KYR_VOX_KYR	5	/* {type def title {Kyr Key}} */
#define KYR_VOX_BUT	6	/* {type def title {headset button straight key}} */

/* 
** MIDI usage
*/
/* midi channel usage */
#define KYRC_CHAN_CC		1	/* {type def title {default channel for keyer control change}} */
#define KYRC_CHAN_NOTE		1	/* {type def title {default channel for keyer notes}} */

/*
** midi note usage
** by default we send notes for the following input keys
** and transmit key events.
*/
#define KYR_NOTE_L_PAD		0      /* {type def title {note used to report raw left paddle switch}} */
#define KYR_NOTE_R_PAD		1      /* {type def title {note used to report raw right paddle switch}} */
#define KYR_NOTE_S_KEY		2      /* {type def title {note used to report raw straight key switch}} */
#define KYR_NOTE_EXT_PTT	3      /* {type def title {note used to report raw external ptt switch}} */
#define KYR_NOTE_KEY_OUT	4      /* {type def title {note used to send transmitted key out signal}} */
#define KYR_NOTE_PTT_OUT	5      /* {type def title {note used to send transmitted ptt out signal}} */
#define KYR_NOTE_TUNE		6      /* {type def title {note used to key the tune channel}} */

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
#define KYR_CC_MSB	6	/* {type def title {MIDI control change Data Entry (MSB)}} */
#define KYR_CC_LSB	38	/* {type def title {MIDI control change Data Entry (LSB)}} */
#define KYR_CC_NRPN_LSB	98	/* {type def title {MIDI control change Non-registered Parameter (LSB)}} */
#define KYR_CC_NRPN_MSB	99	/* {type def title {MIDI control change Non-registered Parameter (MSB)}} */

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
#define KYRV_NOT_SET -1		/* {type val title {16 bit not set value}} */
#define KYRV_MASK    0x3fff	/* {type val title {14 bit mask}} */
#define KYRV_SIGN(v) ((((int16_t)(v))<<2)>>2) /* {type mac title {sign extend masked 14 bit to 16 bit twos complement}} */

/* NRPN_GLOBAL_BLOCK - parameters controlling the entire keyer */
#define KYRP_FIRST		0 /* {type rel title {base of nrpns}} */

#define KYRP_NOTHING		(KYRP_FIRST+0) /* {type rel title {nothng parameter value, zero is not a valid parameter} property nothing} */
#define KYRP_ID_KEYER		(KYRP_FIRST+1) /* {type rel title {keyer unique identfier} property keyerIdentifier} */
#define KYRP_ID_VERSION		(KYRP_FIRST+2) /* {type rel title {keyer version identfier} property keyerVersion} */

#define KYRP_CODEC		(KYRP_FIRST+3) /* {type rel title {base of codec nrpns}} */

#define KYRP_VOLUME		(KYRP_CODEC+0) /* {type par label Vol title {output volume} unit dB/10 range {-128 24} property masterVolume} */
#define KYRP_INPUT_SELECT	(KYRP_CODEC+1) /* {type par label InSel title {input select} values KYRV_INPUT_* property inputSelect} */
#define KYRV_INPUT_MIC		0	       /* {type val label Mic title {input select microphone} value-of KYRP_INPUT_SELECT property inputSelects} */
#define KYRV_INPUT_LINE		1	       /* {type val label LineIn title {input select line in} value-of KYRP_INPUT_SELECT property inputSelects} */
#define KYRP_INPUT_LEVEL	(KYRP_CODEC+2) /* {type par label InLvl title {input level} range {-128 24} unit dB/10 property inputLevel} */

#define KYRP_SOFT		(KYRP_CODEC+3) /* {type rel title {base of soft params}} */

#define KYRP_BUTTON_0		(KYRP_SOFT+0) /* {type par label But0 title {headset button 0 - none pressed} range {-8192 8191} unit pp8191 property buttonLevel0} */
#define KYRP_BUTTON_1		(KYRP_SOFT+1) /* {type par label But1 title {headset button 1 - center or only pressed} range {-8192 8191} unit pp8191 property buttonLevel1} */
#define KYRP_BUTTON_2		(KYRP_SOFT+2) /* {type par label But2 title {headset button 2 - up pressed} range {-8192 8191} unit pp8191 property buttonLevel2}  */
#define KYRP_BUTTON_3		(KYRP_SOFT+3) /* {type par label But3 title {headset button 3 - down pressed} range {-8192 8191} unit pp8191 property buttonLevel3}  */
#define KYRP_BUTTON_4		(KYRP_SOFT+4) /* {type par label But4 title {headset button 4 - hey pressed} range {-8192 8191} unit pp8191 property buttonLevel4}  */
#define KYRP_PTT_ENABLE		(KYRP_SOFT+5) /* {type par label PTTReq title {require EXT_PTT to transmit} range {0 1} ignore 1 property externalPTTRequire} */
#define KYRP_IQ_ENABLE		(KYRP_SOFT+6) /* {type par label {IQ mode} title {Mode of IQ sample generation} values KYRV_IQ_* property iqModeSelect} */
#define KYRV_IQ_NONE		0 /* {type val label None title {no IQ} property iqModes value-of KYRP_IQ_ENABLE} */
#define KYRV_IQ_LSB		1 /* {type val label LSB title {IQ for lower sideband} property iqModes value-of KYRP_IQ_ENABLE} */
#define KYRV_IQ_USB		2 /* {type val label USB title {IQ for upper sideband} property iqModes value-of KYRP_IQ_ENABLE} */
#define KYRP_IQ_ADJUST		(KYRP_SOFT+7) /* {type par label IQAdj title {adjustment to IQ phase} range {-8192 8191} unit pp8191 ignore 1 property iqAdjustPhase} */
#define KYRP_TX_ENABLE		(KYRP_SOFT+8) /* {type par label TXEn title {soft enable TX} range {0 1} property txEnable} */
#define KYRP_ST_ENABLE		(KYRP_SOFT+9) /* {type par label STEn title {enable sidetone generation} range {0 1} property sidetoneEnable} */
#define KYRP_IQ_BALANCE		(KYRP_SOFT+10) /* {type par label IQBal title {adjustment to IQ balance} range {-8192 8191} unit pp8191 ignore 1 property iqAdjustBalance} */
#define KYRP_ST_PAN		(KYRP_SOFT+11) /* {type par label STPan title {sidetone pan left or right} range {-8192 8191} unit pp8191 ignore 1 property sidetonePan} */
#define KYRP_OUT_ENABLE		(KYRP_SOFT+12) /* {type par label OutMix title {output mixer enable bits} range {0 4095} property outputEnable} */
#define KYRP_REMOTE_KEY		(KYRP_SOFT+13) /* {type par label Remote title {enable direct remote control of key_out and ptt_out by midi note} property remoteKey} */

#define KYRP_COMM		(KYRP_SOFT+14) /* {type rel title {keyer parameters shared across voices}} */

#define KYRP_DEBOUNCE		(KYRP_COMM+0) /* {type par label Deb title {debounce period} range {0 16383} unit sample property debouncePeriod} */

#define KYRP_PTT		(KYRP_COMM+1) /* {type rel title {PTT timing parameters}} */

#define KYRP_HEAD_TIME		(KYRP_PTT+0) /* {type par label PTTHead title {time ptt should lead key, key delay} range {0 16383} unit sample property pttHeadTime} */
#define KYRP_TAIL_TIME		(KYRP_PTT+1) /* {type par label PTTTail title {time ptt should linger after key} range {0 16383} unit sample property pttTailTime} */
#define KYRP_HANG_TIME		(KYRP_PTT+2) /* {type par label PTTHang title {time in dits ptt should linger after key} range {0 127} unit dit property pttHangTime} */

#define KYRP_RAMP		(KYRP_PTT+3) /* {type rel title {base of the keyer ramp parameters}} */

#define KYRP_RISE_TIME		(KYRP_RAMP+0)	/* {type par title {key rise ramp length} range {0 16383} unit sample property keyerRiseTime} */
#define KYRP_FALL_TIME		(KYRP_RAMP+1)	/* {type par title {key fall ramp length} range {0 16383} unit sample property keyerFallTime} */
#define KYRP_RISE_RAMP		(KYRP_RAMP+2)	/* {type par title {key rise ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN property keyerRiseRamp valuesProperty keyerRamps} */
#define KYRP_FALL_RAMP		(KYRP_RAMP+3)	/* {type par title {key fall ramp} values KYRV_RAMP_* default KYRV_RAMP_HANN property keyerFallRamp valuesProperty keyerRamps} */
#define KYRV_RAMP_HANN		0 /* {type val label Hann title {ramp from Hann window function, raised cosine} value-of {KYRP_*_RAMP} property keyerRamps} */
#define KYRV_RAMP_BLACKMAN_HARRIS 1 /* {type val label {Blackman Harris} title {ramp from Blackman Harris window function} value-of {KYRP_*_RAMP} property keyerRamps} */
#define KYRV_RAMP_LINEAR	2 /* {type val label Linear title {linear ramp, for comparison} value-of {KYRP_*_RAMP} property keyerRamps} */

#define KYRP_PAD		(KYRP_RAMP+4) /* {type rel title {base of paddle keyer parameters}} */

/* keyer parameters - for paddle keyers - this all could fit into one word 2+1+2+1+1+2 == 9 bits  */
#define KYRP_PAD_MODE		(KYRP_PAD+0)	/* {type par title {iambic keyer mode A/B/S} values KYRV_MODE_* default KYRV_MODE_A property paddleMode} */
#define KYRV_MODE_A		0 /* {type val label A title {paddle keyer iambic mode A} value-of {KYRP_PAD_MODE}} */
#define KYRV_MODE_B		1 /* {type val label B title {paddle keyer iambic mode B} value-of {KYRP_PAD_MODE}} */
#define KYRV_MODE_S		2 /* {type val label S title {paddle keyer bug mode} value-of {KYRP_PAD_MODE}} */
#define KYRP_PAD_SWAP		(KYRP_PAD+1) /* {type par title {swap paddles} range {0 1} default 0 property paddleSwapped} */
#define KYRP_PAD_ADAPT		(KYRP_PAD+2) /* {type par title {paddle adapter normal/ultimatic/single lever} values KYRV_ADAPT_* default KYRV_ADAPT_NORMAL property paddleAdapter} */
#define KYRV_ADAPT_NORMAL	0 /* {type val label Normal title {paddle keyer unmodified} value-of {KYRP_PAD_ADAPT}} */
#define KYRV_ADAPT_ULTIMATIC	1 /* {type val label Ultimatic title {paddle keyer modified to produce ultimatic keyer} value-of {KYRP_PAD_ADAPT}} */
#define KYRV_ADAPT_SINGLE	2 /* {type val label Single title {paddle keyer modified to simulate single lever paddle} value-of {KYRP_PAD_ADAPT}} */
#define KYRP_AUTO_ILS		(KYRP_PAD+3) /* {type par title {automatic letter space timing} range {0 1} default 1 property autoLetterSpace} */
#define KYRP_AUTO_IWS		(KYRP_PAD+4) /* {type par title {automatic word space timing} range {0 1} default 0 property autoWordSpace} */
#define KYRP_PAD_KEYER		(KYRP_PAD+5) /* {type par title {paddle keyer implementation} values KYRV_KEYER_* default KYRV_KEYER_VK6PH property paddleKeyer} */
#define KYRV_KEYER_AD5DZ	0 /* {type val label ad5dz title {paddle keyer algorithm by ad5dz} value-of {KYRP_PAD_KEYER} property paddleKeyers}  */
#define KYRV_KEYER_K1EL		1 /* {type val label k1el title {paddle keyer algorithm by k1el} value-of {KYRP_PAD_KEYER} property paddleKeyers} */
#define KYRV_KEYER_ND7PA	2 /* {type val label nd7pa title {paddle keyer algorithm by nd7pa} value-of {KYRP_PAD_KEYER} property paddleKeyers} */
#define KYRV_KEYER_VK6PH	3 /* {type val label vk6ph title {paddle keyer algorithm by vk6ph} value-of {KYRP_PAD_KEYER} property paddleKeyers} */

#define KYRP_CHAN		(KYRP_PAD+6) /* {type rel title {base of midi channels}} */

#define KYRP_CHAN_CC		(KYRP_CHAN+0) /* {type par title {midi channel for sending controls} unit {} range {0 16} property channelCC} */
#define KYRP_CHAN_NOTE		(KYRP_CHAN+1) /* {type par title {midi channel for sending input notes} unit {} range {0 16} property channelNote} */
#define KYRV_CHAN_INVALID	0	       /* {type val title {invalid channel, used to disable midi channel}} */

#define KYRP_NOTE		(KYRP_CHAN+2) /* {type rel title {base of midi notes}} */

#define KYRP_NOTE_L_PAD		(KYRP_NOTE+0) /* {type par title {note for left paddle switch input} unit {} range {0 128}  property noteLeftPaddle} */
#define KYRP_NOTE_R_PAD		(KYRP_NOTE+1) /* {type par title {note for right paddle switch input} unit {} range {0 128} property noteRightPaddle} */
#define KYRP_NOTE_S_KEY		(KYRP_NOTE+2) /* {type par title {note for straight key switch input} unit {} range {0 128} property noteStraightKey} */
#define KYRP_NOTE_EXT_PTT	(KYRP_NOTE+3) /* {type par title {note for external ptt switch input} unit {} range {0 128} property noteExternalPTT} */
#define KYRP_NOTE_KEY_OUT	(KYRP_NOTE+4) /* {type par title {note for key/ptt key output} unit {} range {0 128} property noteKeyOut} */
#define KYRP_NOTE_PTT_OUT	(KYRP_NOTE+5) /* {type par title {note for key/ptt ptt output} unit {} range {0 128} property notePTTOut} */
#define KYRP_NOTE_TUNE		(KYRP_NOTE+6) /* {type par title {note to key tune signal} unit {} range {0 128} property noteTune} */
#define KYRV_NOTE_INVALID	128	       /* {type val title {invalid note, used to disable midi events}} */

#define KYRP_PINS		(KYRP_NOTE+6) /* {type rel title {base of hardware pin assignments}} */

#define KYRP_ADC0_CONTROL	(KYRP_PINS+0) /* {type par title {property for adc0 = A0} unit {} values KYRV_ADC_* property adc0Control valuesProperty adcControls} */
#define KYRP_ADC1_CONTROL	(KYRP_PINS+1) /* {type par title {property for adc1 = A1 (master volume on the CWKeyer)} unit {} values KYRV_ADC_* property adc1Control valuesProperty adcControls} */
#define KYRP_ADC2_CONTROL	(KYRP_PINS+2) /* {type par title {property for adc2 = A2 (sidetone volume on the CWKeyer)} unit {} values KYRV_ADC_* property adc2Control valuesProperty adcControls} */
#define KYRP_ADC3_CONTROL	(KYRP_PINS+3) /* {type par title {property for adc3 = A3 (sidetone frequency on the CWKeyer} unit {} values KYRV_ADC_* property adc4Control valuesProperty adcControls} */
#define KYRP_ADC4_CONTROL	(KYRP_PINS+4) /* {type par title {property for adc4 = A8 (speed on the CWKeyer)} unit {} values KYRV_ADC_* property adc3Control valuesProperty adcControls} */
#define KYRP_ADC_ENABLE		(KYRP_PINS+5) /* {type par title {enable adc channels} label {ADCEn} range {0 1} property adcEnable} */

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

#define KYRP_MIX_USB_L0		(KYRP_MIXER+0)	/* {type par title {i2s_in left to usb_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_L1		(KYRP_MIXER+1)	/* {type par title {sidetone left to usb_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_L2		(KYRP_MIXER+2)	/* {type par title {IQ left to usb_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_L3		(KYRP_MIXER+3)  /* {type par title {usb_in left to usb_out left} range {-128 24} unit dB/10} */

#define KYRP_MIX_USB_R0		(KYRP_MIXER+4)	/* {type par title {i2s_in right to usb_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_R1		(KYRP_MIXER+5)	/* {type par title {sidetone right to usb_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_R2		(KYRP_MIXER+6)	/* {type par title {IQ right to usb_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_USB_R3		(KYRP_MIXER+7)  /* {type par title {usb_in right to usb_out right} range {-128 24} unit dB/10} */

#define KYRP_MIX_I2S_L0		(KYRP_MIXER+8)  /* {type par title {usb_in left to i2s_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_L1		(KYRP_MIXER+9)  /* {type par title {sidetone left to i2s_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_L2		(KYRP_MIXER+10) /* {type par title {IQ left to i2s_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_L3		(KYRP_MIXER+11) /* {type par title {i2s_in right to i2s_out right} range {-128 24} unit dB/10} */

#define KYRP_MIX_I2S_R0		(KYRP_MIXER+12)	/* {type par title {usb_in right to i2s_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_R1		(KYRP_MIXER+13)	/* {type par title {sidetone right to i2s_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_R2		(KYRP_MIXER+14) /* {type par title {IQ right to i2s_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_I2S_R3		(KYRP_MIXER+15) /* {type par title {i2s_in right to i2s_out right} range {-128 24} unit dB/10} */

#define KYRP_MIX_HDW_L0		(KYRP_MIXER+16) /* {type par title {usb_in left to hdw_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_L1		(KYRP_MIXER+17) /* {type par title {sidetone left to hdw_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_L2		(KYRP_MIXER+18) /* {type par title {IQ left to hdw_out left} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_L3		(KYRP_MIXER+19) /* {type par title {i2s_in left to hdw_out left} range {-128 24} unit dB/10} */

#define KYRP_MIX_HDW_R0		(KYRP_MIXER+20) /* {type par title {usb_in right to hdw_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_R1		(KYRP_MIXER+21) /* {type par title {sidetone right to hdw_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_R2		(KYRP_MIXER+22) /* {type par title {IQ right to hdw_out right} range {-128 24} unit dB/10} */
#define KYRP_MIX_HDW_R3		(KYRP_MIXER+23) /* {type par title {i2s_in right to hdw_out right} range {-128 24} unit dB/10} */

#define KYRP_KEYER		(KYRP_MIXER+24) /* {type rel title {base of vox specialized keyer parameters}} */

#define KYRP_TONE		(KYRP_KEYER+0) /* {type par title {sidetone and IQ oscillator frequency} range {0 16383} unit Hz/10 property keyerTone} */
#define KYRP_LEVEL		(KYRP_KEYER+1) /* {type par title {sidetone level} range {-128 24} default 0 unit dB/10 property keyerLevel} */
#define KYRP_SPEED		(KYRP_KEYER+2) /* {type par title {keyer speed control} range {0 16384} unit WPM property keyerSpeed} */
#define KYRP_WEIGHT		(KYRP_KEYER+3) /* {type par title {keyer mark/space weight} range {25 75} unit % default 50 property keyerWeight} */
#define KYRP_RATIO		(KYRP_KEYER+4) /* {type par title {keyer dit/dah ratio} range {25 75} unit % default 50 property keyerRatio} */
#define KYRP_FARNS		(KYRP_KEYER+5) /* {type par title {Farnsworth keying speed} range {0 127} default 0 unit WPM property keyerFarnsworth} */
#define KYRP_COMP		(KYRP_KEYER+6) /* {type par title {keyer compensation} range {-8192 8191} default 0 unit sample property keyerCompensation} */
#define KYRP_SPEED_FRAC		(KYRP_KEYER+7) /* {type par title {keyer speed fraction} range {0 127} default 0 unit WPM/128 property keyerSpeedFraction} */

#define KYRP_KEYER_LAST		(KYRP_KEYER+8) /* {type rel title {end of keyer block}} */

/* seven repetitions of the keyer block for per voice customizations */
#define KYRP_VOX_OFFSET		(KYRP_KEYER_LAST-KYRP_KEYER) /* {type rel title {size of keyer parameter block}} */
#define KYRP_VOX_NONE		(KYRP_KEYER+KYR_VOX_NONE*KYRP_VOX_OFFSET) /* {type rel title {base of default keyer parameters}} */
#define KYRP_VOX_TUNE		(KYRP_KEYER+KYR_VOX_TUNE*KYRP_VOX_OFFSET) /* {type rel title {base of tune keyer parameters}} */
#define KYRP_VOX_S_KEY		(KYRP_KEYER+KYR_VOX_S_KEY*KYRP_VOX_OFFSET) /* {type rel title {base of straight key parameters}} */
#define KYRP_VOX_PAD		(KYRP_KEYER+KYR_VOX_PAD*KYRP_VOX_OFFSET) /* {type rel title {base of paddle keyer parameters}} */
#define KYRP_VOX_WINK		(KYRP_KEYER+KYR_VOX_WINK*KYRP_VOX_OFFSET) /* {type rel title {base of text from winkey parameters}} */
#define KYRP_VOX_KYR		(KYRP_KEYER+KYR_VOX_KYR*KYRP_VOX_OFFSET) /* {type rel title {base of text from hasak parameters}} */
#define KYRP_VOX_BUT		(KYRP_KEYER+KYR_VOX_BUT*KYRP_VOX_OFFSET) /* {type rel title {base of headset button keyer parameters}} */

/* definitions of ADC targets, these had undefined symbols when placed immediately after KYRP_ADC* */
#define KYRV_ADC_NOTHING	(KYRP_NOTHING) /* {type val label None title {pot controls nothing} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_VOLUME		(KYRP_VOLUME) /* {type val label Volume title {pot controls master volume} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_SPEED		(KYRP_SPEED) /* {type val label Speed title {pot controls keyer speed} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_LEVEL		(KYRP_LEVEL) /* {type val label Level title {pot controls sidetone level} value-of KYRP_ADC*_CONTROL property adcControls} */
#define KYRV_ADC_TONE		(KYRP_TONE) /* {type val label Tone title {pot controls sidetone pitch} value-of KYRP_ADC*_CONTROL property adcControls} */

#define KYRP_LAST		(KYRP_VOX_BUT+KYRP_VOX_OFFSET) /* {type rel title {one past end of stored keyer parameters}} */

#define KYRP_XFIRST		(1000) /* {type rel title {base of extended nrpn block}} */

#define KYRP_XKEYER		(KYRP_XFIRST) /* {type rel title {base of extended keyer block}} */

#define KYRP_XPER_DIT		(KYRP_XKEYER+0) /* {type scr title {samples per dit}} */
#define KYRP_XPER_DAH		(KYRP_XKEYER+1) /* {type scr title {samples per dah}} */
#define KYRP_XPER_IES		(KYRP_XKEYER+2) /* {type scr title {samples per inter element space}} */
#define KYRP_XPER_ILS		(KYRP_XKEYER+3) /* {type scr title {samples per inter letter space}} */
#define KYRP_XPER_IWS		(KYRP_XKEYER+4) /* {type scr title {samples per inter word space}} */

#define KYRP_XKEYER_LAST	(KYRP_XKEYER+5) /* {type rel title {one past end of extended keyer block}} */

#define KYRP_XVOX_OFFSET	(KYRP_XKEYER_LAST-KYRP_XKEYER) /* {type rel title {size of extended keyer parameter block}} */
#define KYRP_XVOX_NONE		(KYRP_XKEYER+KYR_VOX_NONE*KYRP_XVOX_OFFSET) /* {type rel title {base of default keyer parameters}} */
#define KYRP_XVOX_TUNE		(KYRP_XKEYER+KYR_VOX_TUNE*KYRP_XVOX_OFFSET) /* {type rel title {base of tune keyer parameters}} */
#define KYRP_XVOX_S_KEY		(KYRP_XKEYER+KYR_VOX_S_KEY*KYRP_XVOX_OFFSET) /* {type rel title {base of straight key parameters}} */
#define KYRP_XVOX_PAD		(KYRP_XKEYER+KYR_VOX_PAD*KYRP_XVOX_OFFSET) /* {type rel title {base of paddle keyer parameters}} */
#define KYRP_XVOX_WINK		(KYRP_XKEYER+KYR_VOX_WINK*KYRP_XVOX_OFFSET) /* {type rel title {base of text from winkey parameters}} */
#define KYRP_XVOX_KYR		(KYRP_XKEYER+KYR_VOX_KYR*KYRP_XVOX_OFFSET) /* {type rel title {base of text from hasak parameters}} */
#define KYRP_XVOX_BUT		(KYRP_XKEYER+KYR_VOX_BUT*KYRP_XVOX_OFFSET) /* {type rel title {base of headset button keyer parameters}} */

#define KYRP_XLAST		(KYRP_XVOX_BUT+KYRP_XVOX_OFFSET) /* {type rel title {end+1 of extended keyer block}} */

#define KYRP_EXEC		(2000) /* {type rel title {base of command nrpns}} */

#define KYRP_WRITE_EEPROM	(KYRP_EXEC+0) /* {type cmd label {Write EEPROM} title {write nrpn+msgs to eeprom}} */
#define KYRP_READ_EEPROM	(KYRP_EXEC+1) /* {type cmd label {Read EEPROM} title {read nrpn+msgs from eeprom}} */
#define KYRP_SET_DEFAULT	(KYRP_EXEC+2) /* {type cmd label {Reset to default} title {load nrpn with default values}} */
#define KYRP_ECHO_ALL		(KYRP_EXEC+3) /* {type cmd label {Echo all} title {echo all set nrpns to midi}} */
#define KYRP_SEND_WINK		(KYRP_EXEC+4) /* {type cmd label {Send to Winkey} title {send character value to wink vox}} */
#define KYRP_SEND_KYR		(KYRP_EXEC+5) /* {type cmd label {Send to Keyer} title {send character value to kyr vox}} */
#define KYRP_MSG_INDEX		(KYRP_EXEC+6) /* {type cmd label {Message byte index} title {set index into msgs}} */
#define KYRP_MSG_WRITE		(KYRP_EXEC+7) /* {type cmd label {Message write byte} title {set msgs[index++] to value}} */
#define KYRP_MSG_READ		(KYRP_EXEC+8) /* {type cmd label {Message reqd byte} title {read msgs[index++] and echo the value}} */

#define KYRP_INFO		(3000) /* {type rel title {base of information nrpns}} */

#define KYRP_NRPN_SIZE		(KYRP_INFO+0) /* {type inf title {size of nrpn array}} */
#define KYRP_MSG_SIZE		(KYRP_INFO+1) /* {type inf title {send the size of msgs array}} */
#define KYRP_SAMPLE_RATE	(KYRP_INFO+2) /* {type inf title {sample rate of audio library} unit sps/100} */
#define KYRP_EEPROM_LENGTH	(KYRP_INFO+3) /* {type inf title {result of EEPROM.length()} unit bytes} */
#define KYRP_ID_CPU		(KYRP_INFO+4) /* {type inf title {which teensy microprocessor are we running}} */
#define KYRP_ID_CODEC		(KYRP_INFO+5) /* {type inf title {which codec are we running}} */

/* end of defined nrpns */
/* end of config.h */
#endif
