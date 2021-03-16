/***************************************************************
** MIDI input handling.
***************************************************************/
/*
** this could be a NRPN, too, but the penalty for screwing it up
** could be pretty fierce.  it could also be split into different
** channels for receive and send.
*/
static uint8_t kyr_in_channel = KYR_IN_CHANNEL;
static uint8_t kyr_out_channel = KYR_OUT_CHANNEL;

/*
** receive a note on event and do something with it.
** only if KYRP_RECV_MIDI is enabled and it's on our channel.
*/
static void myNoteOn(byte channel, byte note, byte velocity) {
  if (get_recv_midi() && (channel == kyr_in_channel)) {
    switch (note) {
    case KYR_L_PAD_NOTE:      /* note used to report raw left paddle switch */
      digitalWrite(KYR_L_PAD_PIN, velocity == 0); break;
    case KYR_R_PAD_NOTE:      /* note used to report raw right paddle switch */
      digitalWrite(KYR_R_PAD_PIN, velocity == 0); break;
    case KYR_S_KEY_NOTE:      /* note used to report raw straight key switch */
      digitalWrite(KYR_S_KEY_PIN, velocity == 0); break;
    case KYR_PTT_SW_NOTE:      /* note used to report raw ptt switch */
      digitalWrite(KYR_PTT_SW_PIN, velocity == 0); break;
    case KYR_KEY_OUT_NOTE:      /* note used to send external key signal */
      digitalWrite(KYR_KEY_OUT_PIN, velocity == 0); break;
    case KYR_PTT_OUT_NOTE:      /* note used to send external ptt signal */
      digitalWrite(KYR_PTT_OUT_PIN, velocity == 0); break;
    default:
      break;
    }
  }
}

static void myNoteOff(byte channel, byte note, byte velocity) {
  if (get_recv_midi() && (channel == kyr_in_channel)) {
    switch (note) {
    case KYR_L_PAD_NOTE:      /* note used to report raw left paddle switch */
      digitalWrite(KYR_L_PAD_PIN, 1); break;
    case KYR_R_PAD_NOTE:      /* note used to report raw right paddle switch */
      digitalWrite(KYR_R_PAD_PIN, 1); break;
    case KYR_S_KEY_NOTE:      /* note used to report raw straight key switch */
      digitalWrite(KYR_S_KEY_PIN, 1); break;
    case KYR_PTT_SW_NOTE:      /* note used to report raw ptt switch */
      digitalWrite(KYR_PTT_SW_PIN, 1); break;
    case KYR_KEY_OUT_NOTE:      /* note used to send external key signal */
      digitalWrite(KYR_KEY_OUT_PIN, 1); break;
    case KYR_PTT_OUT_NOTE:      /* note used to send external ptt signal */
      digitalWrite(KYR_PTT_OUT_PIN, 1); break;
    default:
      break;
    }
  }
}

static void myControlChange(byte channel, byte control, byte value) {
  /*
  ** handle NRPN format
  ** the canned ctrlr format sends MSB, LSB
  ** for both the NRPN number and the value.
  */
  static uint16_t cc_nrpn, cc_value;

  if (channel == kyr_in_channel)
    switch (control) {
    case KYR_CC_MSB:	/* Data Entry (MSB) */
      cc_value = (value&127)<<7;
      break;
    case KYR_CC_LSB:	/* Data Entry (LSB) */
      cc_value |= value&127;
      nrpn_set(cc_nrpn, cc_value); 
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

static void midi_setup(void) {
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
  usbMIDI.setHandleControlChange(myControlChange);
}

static uint8_t midi_send_toggle(uint8_t pin, uint8_t note) {
  // if the pin was high, then it went low, and is now active, so send velocity 1
  // if the pin was low, then it went high, and is now off, so send velocity 0
  usbMIDI.sendNoteOn(note, pin, kyr_out_channel);
  usbMIDI.send_now();		// send it now
  return pin ^ 1;		// invert the pin
}
static void midi_loop(void) {
  while (usbMIDI.read());
  if (get_send_midi()) {
    static uint8_t l_pad, r_pad, s_key, ptt_sw, key_out, ptt_out;
    if (digitalRead(KYR_L_PAD_PIN) != l_pad) l_pad = midi_send_toggle(l_pad, KYR_L_PAD_NOTE);
    if (digitalRead(KYR_R_PAD_PIN) != r_pad) r_pad = midi_send_toggle(r_pad, KYR_R_PAD_NOTE);
    if (digitalRead(KYR_S_KEY_PIN) != s_key) s_key = midi_send_toggle(s_key, KYR_S_KEY_NOTE);
    if (digitalRead(KYR_PTT_SW_PIN) != ptt_sw) ptt_sw = midi_send_toggle(ptt_sw, KYR_PTT_SW_NOTE);
    if (_key_out != key_out) key_out = midi_send_toggle(key_out, KYR_KEY_OUT_NOTE);
    if (_ptt_out != ptt_out) ptt_out = midi_send_toggle(ptt_out, KYR_PTT_OUT_NOTE);
  }
}

