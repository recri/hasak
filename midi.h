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
    /* 
       none of these work, they need a constant source 
       to be keyed on and off and or'ed into the normal
       stream
    */
    if (note == get_nrpn(KYRP_NOTE_L_PAD)) {
      /* note used to report raw left paddle switch */
      digitalWrite(KYR_L_PAD_PIN, velocity == 0);
    } else if (note == get_nrpn(KYRP_NOTE_R_PAD)) {
      /* note used to report raw right paddle switch */
      digitalWrite(KYR_R_PAD_PIN, velocity == 0);
    } else if (note == get_nrpn(KYRP_NOTE_S_KEY)) {
      /* note used to report raw straight key switch */
      digitalWrite(KYR_S_KEY_PIN, velocity == 0);
    } else if (note == get_nrpn(KYRP_NOTE_EXT_PTT)) {
      /* note used to report raw ptt switch */
      digitalWrite(KYR_EXT_PTT_PIN, velocity == 0);
    } else if (note == get_nrpn(KYRP_NOTE_KEY_OUT)) {
      /* note used to send external key signal */
      digitalWrite(KYR_KEY_OUT_PIN, velocity == 0);
    } else if (note == get_nrpn(KYRP_NOTE_PTT_OUT)) {
      /* note used to send external ptt signal */
      digitalWrite(KYR_PTT_OUT_PIN, velocity == 0);
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
  int channel = get_nrpn(KYRP_CHAN_CC);
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

static uint16_t midi_send_toggle(const uint16_t pinvalue, const int16_t note_nrpn, int16_t channel, int16_t vox) {
  // if the pinvalue was high, then it went low, and is now active, so send velocity 1
  // if the pinvalue was low, then it went high, and is now off, so send velocity 0

  if (midi_valid_channel(channel)) {
    const int note = get_nrpn(note_nrpn);
    if ( midi_valid_note(note)) {
      kyr_send_note += 1;
      usbMIDI.sendNoteOn(note, pinvalue ? pinvalue+vox : pinvalue, channel);
      usbMIDI.send_now();		// send it now
    }
  }
  return pinvalue ^ 1;		// invert the pin
}

static void midi_loop(void) {
  static uint8_t l_pad, r_pad, s_key, ptt_sw, key_out, ptt_out;
  int channel = get_nrpn(KYRP_CHAN_NOTE);
  if (digitalRead(KYR_L_PAD_PIN) != l_pad) l_pad = midi_send_toggle(l_pad, KYRP_NOTE_L_PAD, channel, 0);
  if (digitalRead(KYR_R_PAD_PIN) != r_pad) r_pad = midi_send_toggle(r_pad, KYRP_NOTE_R_PAD, channel, 0);
  if (digitalRead(KYR_S_KEY_PIN) != s_key) s_key = midi_send_toggle(s_key, KYRP_NOTE_S_KEY, channel, 0);
  if (digitalRead(KYR_EXT_PTT_PIN) != ptt_sw) ptt_sw = midi_send_toggle(ptt_sw, KYRP_NOTE_EXT_PTT, channel, 0);
  if (_key_out != key_out) key_out = midi_send_toggle(key_out, KYRP_NOTE_KEY_OUT, channel, get_active_vox());
  if (_ptt_out != ptt_out) ptt_out = midi_send_toggle(ptt_out, KYRP_NOTE_PTT_OUT, channel, get_active_vox());
  while (usbMIDI.read());
}

