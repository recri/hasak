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
/***************************************************************
** MIDI input handling.
***************************************************************/

static uint32_t kyr_recv_note = 0;
static uint32_t kyr_send_note = 0;
static uint32_t kyr_recv_nrpn = 0;
static uint32_t kyr_send_nrpn = 0;

static int midi_valid_channel(const int channel) { return channel > 0 && channel <= 16; }

static int midi_valid_note(const int note) { return note >= 0 && note <= 127; }

/*
** As written this doesn't work, and it complicated to make it work.
** the values written get immediately overwritten by the value coming
** out of the audio library.  To make it work would require a constant
** source in the audio library that turns on and or's itself with the
** normal source of bits.
*/
/*
** receive a note on event and do something with it.
** only if it's on our channel, which might be
*/
static void midi_note_on(byte channel, byte note, byte velocity) {
  kyr_recv_note += 1;
  if (channel == get_nrpn(KYRP_CHAN_NOTE)) {
    if (note == get_nrpn(KYRP_NOTE_TUNE)) {
      hasak._tune_note_on = (velocity == 0)&get_nrpn(KYRP_REMOTE_KEY);
    }
  }
}

static void midi_note_off(byte channel, byte note, byte velocity) {
  midi_note_on(channel, note, 0);
}

static void midi_receive_nrpn(byte channel, byte control, byte value) {
  /*
  ** handle NRPN format
  ** the canned ctrlr format sends MSB, LSB
  ** for both the NRPN number and the value.
  */
  static uint16_t cc_nrpn, cc_value;

  if (channel == get_nrpn(KYRP_CHAN_CC))
    switch (control) {
    case KYR_CC_MSB:	/* Data Entry (MSB) */
      cc_value = (value&127)<<7;
      break;
    case KYR_CC_LSB:	/* Data Entry (LSB) */
      kyr_recv_nrpn += 1;
      cc_value |= value&127;
      nrpn_set(cc_nrpn, cc_value); 
      kyr_recv_nrpn += 1;
      break;
    case KYR_CC_NRPN_MSB:	/* Non-registered Parameter (MSB) */
      cc_nrpn = (value&127)<<7;
      break;
    case KYR_CC_NRPN_LSB:	/* Non-registered Parameter (LSB) */
      cc_nrpn |= value&127;
      break;
    default:
      break;
    }
}

static void midi_send_nrpn(const int16_t nrpn, const int16_t value) {
  int channel = get_nrpn(KYRP_CHAN_NRPN);
  if (midi_valid_channel(channel)) {
    usbMIDI.sendControlChange(KYR_CC_NRPN_MSB, (nrpn>>7)&127, channel);
    usbMIDI.sendControlChange(KYR_CC_NRPN_LSB, nrpn&127, channel);
    usbMIDI.sendControlChange(KYR_CC_MSB, (value>>7)&127, channel);
    usbMIDI.sendControlChange(KYR_CC_LSB, value&127, channel);
    kyr_send_nrpn += 1;
  }
}

static void midi_setup(void) {
  usbMIDI.setHandleNoteOn(midi_note_on);
  usbMIDI.setHandleNoteOff(midi_note_off);
  usbMIDI.setHandleControlChange(midi_receive_nrpn);
}

static uint16_t midi_send_toggle(const uint16_t pinvalue, const int16_t note_nrpn, int16_t channel) {
  // if the pinvalue was high, then it went low, and is now active, so send velocity 1
  // if the pinvalue was low, then it went high, and is now off, so send velocity 0
  const int note = get_nrpn(note_nrpn);

  if ( ! midi_valid_channel(channel)) return pinvalue ^ 1;
  if ( ! (get_nrpn(KYRP_NOTE_ENABLE)&(1<<(note_nrpn-KYRP_NOTE)))) return pinvalue ^ 1; 
  if ( ! midi_valid_note(note)) return pinvalue ^ 1; 
  /* Serial.printf("midi_send_toggle(%d, %d, %d, %d) to note %d\n", pinvalue, note_nrpn, channel, vox, note); */
  kyr_send_note += 1;
  usbMIDI.sendNoteOn(note, pinvalue ? KYRD_NOTE_EXT_OFF : KYRD_NOTE_EXT_ON, channel);
  usbMIDI.send_now();		// send it now
  return pinvalue ^ 1;		// invert the pin
}

static void midi_loop(void) {
  static uint8_t l_pad, r_pad, s_key, ptt_sw, key_out, ptt_out;
  int channel = get_nrpn(KYRP_CHAN_NOTE);
  if (digitalRead(KYR_L_PAD_PIN) != l_pad) l_pad = midi_send_toggle(l_pad, KYRP_NOTE_L_PAD, channel);
  if (digitalRead(KYR_R_PAD_PIN) != r_pad) r_pad = midi_send_toggle(r_pad, KYRP_NOTE_R_PAD, channel);
  if (digitalRead(KYR_S_KEY_PIN) != s_key) s_key = midi_send_toggle(s_key, KYRP_NOTE_S_KEY, channel);
  if (digitalRead(KYR_EXT_PTT_PIN) != ptt_sw) ptt_sw = midi_send_toggle(ptt_sw, KYRP_NOTE_EXT_PTT, channel);
  if (hasak._key_out != key_out) key_out = midi_send_toggle(key_out, KYRP_NOTE_KEY_OUT, channel);
  if (hasak._ptt_out != ptt_out) ptt_out = midi_send_toggle(ptt_out, KYRP_NOTE_PTT_OUT, channel);
  while (usbMIDI.read());
}

