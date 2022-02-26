/* -*- mode: c++; tab-width: 8 -*- */
/*
 * hasak (ham and swiss army knife) keyer for Teensy 4.X, 3.X
 * Copyright (c) 2022 by Roger Critchlow, Charlestown, MA, USA
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


/*
** defines - define notes, ctrls, nrpns, xnrpns
*/
  
static void define_define(void) {
  /*
  ** unnecessary switches used as a compile time check for duplicate
  ** and missed values.
  */
  for (int note = 0; note < KYR_N_NOTE; note += 1)
    switch (note) {		//          note, v, i, o, e, ro
    case KYRN_MIDI_OUT_KEY:	note_define(note, 0, 0, 1, 0, 0); break; // output
    case KYRN_MIDI_OUT_PTT:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_MIDI_IN_TUNE:	note_define(note, 0, 1, 0, 0, 0); break; // input
    case KYRN_MIDI_OUT_ST:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_MIDI_VOL:		note_define(note, 0, 1, 1, 1, 0); break;
    case KYRN_MIDI_ST_VOL:	note_define(note, 0, 1, 1, 1, 0); break;
    case KYRN_MIDI_ST_FREQ:	note_define(note, 0, 1, 1, 1, 0); break;
    case KYRN_MIDI_SPEED:	note_define(note, 0, 1, 1, 1, 0); break;
    case KYRN_HW_L_PAD:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_R_PAD:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_S_KEY:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_EXT_PTT:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_TUNE:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_L_PAD2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_R_PAD2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_S_KEY2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_EXT_PTT2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_L_PAD3:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_R_PAD3:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_S_KEY3:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_KEY_OUT:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_PTT_OUT:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_KEY_OUT2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_HW_PTT_OUT2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_AU_ST_KEY:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_AU_IQ_KEY:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_AU_DECODE:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_S_KEY:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_S_KEY2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_S_KEY3:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_PAD:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_PAD2:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_PAD3:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_TUNE:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_TEXT:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_TEXT2:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ST_NONE:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_KEY_ST:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PTT_ST:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_KEY_OUT:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PTT_OUT:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_TXT_TEXT:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_TXT_TEXT2:	note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_ELT_DEC:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_TXT_DEC:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN0:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN1:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN2:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN3:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN4:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN5:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN6:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_PIN7:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT0:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT1:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT2:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT3:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT4:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT5:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT6:		note_define(note, 0, 0, 0, 0, 0); break;
    case KYRN_POUT7:		note_define(note, 0, 0, 0, 0, 0); break;
    default: Serial.printf("default_setup: overlooked note %d\n", note); break;
  }
  for (int ctrl = 0; ctrl < KYR_N_CTRL; ctrl += 1)
    switch (ctrl) {		//          ctrl, value,                i, o, e, ro
    case 0: case 1: case 2: case 3: case 4: case 5: 
				ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_MSB:		ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case KYRC_MASTER_VOLUME:	ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case KYRC_MASTER_BALANCE:	ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case 9:			ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_MASTER_PAN:	ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case KYRC_SIDETONE_VOLUME:	ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case 11:			ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_SIDETONE_FREQUENCY:ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case 14: case 15: 		ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_INPUT_LEVEL:	ctrl_define(ctrl, 1, 1, 0, 0, 0); break;
    case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
    case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36:
    case 37:			ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_LSB:		ctrl_define(ctrl, 0, 1, 1, 0, 0); break;
    case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: 
    case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: 
    case 59: case 60: case 61: case 62: case 63: 
				ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_ENABLE_POTS:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_KEYER_AUTOPTT:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_RESPONSE:		ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_MUTE_CWPTT:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_MICPTT_HWPTT:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_CWPTT_HWPTT:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case 70: case 71:		ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_KEYER_HANG:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_KEYER_LEADIN:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_CW_SPEED:		ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case KYRC_INPUT_SELECT:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    case 76: case 77: case 78: case 79: case 80: case 81: case 82: case 83: case 84: case 85:
    case 86: case 87: case 88: case 89: case 90: case 91: case 92: case 93: case 94: case 95:
    case 96: case 97:		ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_NRPN_LSB:		ctrl_define(ctrl, 0, 1, 1, 0, 0); break;
    case KYRC_NRPN_MSB:		ctrl_define(ctrl, 0, 1, 1, 0, 0); break;
    case 100: case 101: case 102: case 103: case 104: case 105: case 106: case 107: case 108: case 109:
    case 110: case 111: case 112: case 113: case 114: case 115: case 116: case 117: case 118:
				ctrl_define(ctrl, 0, 0, 0, 0, 0); break;
    case KYRC_SET_CHANNEL:	ctrl_define(ctrl, 0, 1, 1, 1, 0); break;
    default: 
      Serial.printf("default_setup: overlooked ctrl %d\n", ctrl); break;
    }
  for (int nrpn = 0; nrpn < KYR_N_NRPN; nrpn += 1)
    switch (nrpn) {		//          nrpn, v, i, o, e, ro
    case KYRP_NOTHING:		nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_ID_JSON:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break; // readonly, echo response
    case KYRP_STRING_BEGIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_STRING_END:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_STRING_BYTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIDI_INPUTS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break; // readonly, statistics
    case KYRP_MIDI_OUTPUTS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_MIDI_ECHOES:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_MIDI_SENDS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_MIDI_NOTES:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_MIDI_CTRLS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_MIDI_NRPNS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_LISTENER_NODES:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_LISTENER_LISTS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_LISTENER_CALLS:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_LISTENER_FIRES:	nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_STATS_RESET:	nrpn_define(nrpn, 0, 1, 1, 0, 0); break; // command
    case KYRP_LOOP:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break; // counters
    case KYRP_SAMPLE:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_UPDATE:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_MILLI:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_10MILLI:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_100MILLI:		nrpn_define(nrpn, 0, 1, 1, 0, 1); break;
    case KYRP_CHANNEL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_INPUT_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_OUTPUT_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ECHO_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_LISTENER_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_IN_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_OUT_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ST_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_TX_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_IQ_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PTT_REQUIRE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_RKEY_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_CW_AUTOPTT:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_RX_MUTE:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIC_HWPTT:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_CW_HWPTT:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_HDW_IN_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_HDW_OUT_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_VOLUME:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_LEVEL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_IQ_LEVEL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_I2S_LEVEL:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_HDW_LEVEL:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ST_BALANCE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_IQ_BALANCE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_TONE:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_SPEED:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WEIGHT:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_RATIO:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_FARNS:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_COMP:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_SPEED_FRAC:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_HEAD_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_TAIL_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_HANG_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_RISE_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_FALL_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_RISE_RAMP:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_FALL_RAMP:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_PAD_MODE:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PAD_SWAP:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PAD_ADAPT:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_AUTO_ILS:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_AUTO_IWS:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PAD_KEYER:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_ACTIVE_ST:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break; // readonly
    case KYRP_MIXER_SLEW_RAMP:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIXER_SLEW_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_FREQ_SLEW_RAMP:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_FREQ_SLEW_TIME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_ENABLE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_ENABLE_L:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_ENABLE_R:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN_DEBOUNCE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT_LOGIC:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC_RATE:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_XIQ_FREQ:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_XIQ_FREQ+1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_IQ_USB:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_PIN0_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN0_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN1_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN1_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN2_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN2_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN3_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN3_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN4_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN4_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN5_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN5_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN6_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN6_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN7_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_PIN7_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_POUT0_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT0_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT1_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT1_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT2_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT2_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT3_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT3_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT4_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT4_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT5_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT5_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT6_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT6_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT7_PIN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_POUT7_NOTE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_ADC0_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC0_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC0_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC1_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC1_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC1_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC2_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC2_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC2_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC3_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC3_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC3_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC4_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC4_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC4_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC5_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC5_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC5_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC6_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC6_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC6_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC7_PIN:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC7_VAL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_ADC7_NRPN:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

      /* morse code block */
#define M(N) (KYRP_MORSE+(N))
    case M( 0+0): case M( 0+1): case M( 0+2): case M( 0+3): case M( 0+4): case M( 0+5): case M( 0+6): case M( 0+7):
    case M( 8+0): case M( 8+1): case M( 8+2): case M( 8+3): case M( 8+4): case M( 8+5): case M( 8+6): case M( 8+7):
    case M(16+0): case M(16+1): case M(16+2): case M(16+3): case M(16+4): case M(16+5): case M(16+6): case M(16+7):
    case M(24+0): case M(24+1): case M(24+2): case M(24+3): case M(24+4): case M(24+5): case M(24+6): case M(24+7):
    case M(32+0): case M(32+1): case M(32+2): case M(32+3): case M(32+4): case M(32+5): case M(32+6): case M(32+7):
    case M(40+0): case M(40+1): case M(40+2): case M(40+3): case M(40+4): case M(40+5): case M(40+6): case M(40+7):
    case M(48+0): case M(48+1): case M(48+2): case M(48+3): case M(48+4): case M(48+5): case M(48+6): case M(48+7):
    case M(56+0): case M(56+1): case M(56+2): case M(56+3): case M(56+4): case M(56+5): case M(56+6): case M(56+7):
#undef M
				nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_L0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_L1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_L2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_L3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_R0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_R1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_R2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_USB_R3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_L0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_L1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_L2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_L3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_R0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_R1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_R2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_I2S_R3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_L0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_L1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_L2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_L3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_R0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_R1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_R2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_HDW_R3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_MIX_EN_USB_L0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_L1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_L2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_L3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_R0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_R1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_R2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_USB_R3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_L0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_L1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_L2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_L3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_R0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_R1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_R2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_I2S_R3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_L0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_L1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_L2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_L3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_R0:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_R1:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_R2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MIX_EN_HDW_R3:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_CODEC_VOLUME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_INPUT_SELECT:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_INPUT_LEVEL:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_WM8960_ENABLE:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_INPUT_LEVEL:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_INPUT_SELECT:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_VOLUME:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_HEADPHONE_VOLUME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_HEADPHONE_POWER:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_SPEAKER_VOLUME:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_SPEAKER_POWER:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_DISABLE_ADCHPF:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_ENABLE_MICBIAS:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_ENABLE_ALC:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_MIC_POWER:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_LINEIN_POWER:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_RAW_MASK:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_RAW_DATA:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_WM8960_RAW_WRITE:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_WRITE_EEPROM:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_READ_EEPROM:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_SET_DEFAULT:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_ECHO_ALL:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_SEND_TEXT:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_SEND_TEXT2:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MSG_INDEX:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MSG_WRITE:	nrpn_define(nrpn, 0, 1, 1, 1, 0); break;
    case KYRP_MSG_READ:		nrpn_define(nrpn, 0, 1, 1, 1, 0); break;

    case KYRP_NRPN_SIZE:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_MSG_SIZE:		nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_SAMPLE_RATE:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_EEPROM_LENGTH:	nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_ID_CPU:		nrpn_define(nrpn, 0, 1, 1, 1, 1); break;
    case KYRP_ID_CODEC:		nrpn_define(nrpn, 0, 1, 1, 1, 1); break;

    case KYRP_XPER_DIT:		nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_DIT+1:	nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_DAH:		nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_DAH+1:	nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_IES:		nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_IES+1:	nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_ILS:		nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_ILS+1:	nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_IWS:		nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained
    case KYRP_XPER_IWS+1:	nrpn_define(nrpn, 0, 0, 0, 0, 0); break; // internally maintained

    default: 
      Serial.printf("default_setup: overlooked nrpn %d\n", nrpn); break;
    }
}

static void define_setup(void) { define_define(); }

static void define_loop(void) {}