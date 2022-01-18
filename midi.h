/***************************************************************
** MIDI input handling.
***************************************************************/
/*
** this could be a NRPN, too, but the penalty for screwing it up
** could be pretty fierce.  it could also be split into different
** channels for receive and send.
*/
static uint8_t kyr_cc_in_channel = KYR_CC_IN_CHANNEL;
static uint8_t kyr_cc_out_channel = KYR_CC_OUT_CHANNEL;
static uint8_t kyr_note_in_channel = KYR_NOTE_IN_CHANNEL;
static uint8_t kyr_note_out_channel = KYR_NOTE_OUT_CHANNEL;

static uint32_t kyr_recv_note = 0;
static uint32_t kyr_send_note = 0;
static uint32_t kyr_recv_nrpn = 0;
static uint32_t kyr_send_nrpn = 0;

#ifdef KYRP_RECV_MIDI
/*
** As written this doesn't work, and it complicated to make it work.
*/
/*
** receive a note on event and do something with it.
** only if KYRP_RECV_MIDI is enabled and it's on our channel.
*/
static void myNoteOn(byte channel, byte note, byte velocity) {
  kyr_recv_note += 1;
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
  myNoteOn(channel, note, 0);
}
#endif

static void myControlChange(byte channel, byte control, byte value) {
  /*
  ** handle NRPN format
  ** the canned ctrlr format sends MSB, LSB
  ** for both the NRPN number and the value.
  */
  static uint16_t cc_nrpn, cc_value;

  if (channel == kyr_cc_in_channel)
    switch (control) {
    case KYR_CC_MSB:	/* Data Entry (MSB) */
      cc_value = (value&127)<<7;
      break;
    case KYR_CC_LSB:	/* Data Entry (LSB) */
      kyr_recv_nrpn += 1;
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
#ifdef KYRP_RECV_MIDI
  usbMIDI.setHandleNoteOn(myNoteOn);
  usbMIDI.setHandleNoteOff(myNoteOff);
#endif
  usbMIDI.setHandleControlChange(myControlChange);
}

static uint8_t midi_send_toggle(uint8_t pin, uint8_t note, uint8_t send) {
  // if the pin was high, then it went low, and is now active, so send velocity 1
  // if the pin was low, then it went high, and is now off, so send velocity 0
  if (send) {
    kyr_send_note += 1;
    usbMIDI.sendNoteOn(note, pin, kyr_note_out_channel);
    usbMIDI.send_now();		// send it now
  }
  return pin ^ 1;		// invert the pin
}

static void midi_loop(void) {
  int send = get_send_midi();
  if (send) {
    static uint8_t l_pad, r_pad, s_key, ptt_sw, key_out, ptt_out;
    int send_input = send == KYRV_SM_INPUT || send == KYRV_SM_BOTH;
    int send_output = send == KYRV_SM_OUTPUT || send == KYRV_SM_BOTH;
    if (digitalRead(KYR_L_PAD_PIN) != l_pad) l_pad = midi_send_toggle(l_pad, KYR_L_PAD_NOTE, send_input);
    if (digitalRead(KYR_R_PAD_PIN) != r_pad) r_pad = midi_send_toggle(r_pad, KYR_R_PAD_NOTE, send_input);
    if (digitalRead(KYR_S_KEY_PIN) != s_key) s_key = midi_send_toggle(s_key, KYR_S_KEY_NOTE, send_input);
    if (digitalRead(KYR_PTT_SW_PIN) != ptt_sw) ptt_sw = midi_send_toggle(ptt_sw, KYR_PTT_SW_NOTE, send_input);
    if (_key_out != key_out) key_out = midi_send_toggle(key_out, KYR_KEY_OUT_NOTE, send_output);
    if (_ptt_out != ptt_out) ptt_out = midi_send_toggle(ptt_out, KYR_PTT_OUT_NOTE, send_output);
  }
  while (usbMIDI.read());
}

