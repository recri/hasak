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

/*
** hasak midi interface with listeners
** a template to supply the number of nrpns to implement
** so instantiote with HasakMidi<NNRPN> midi(usbMidi,channel)
*/
template <int _N_NOTE, int _N_CTRL, int _N_NRPN> class Midi {

 public:

  static const int N_NOTE = _N_NOTE;

  static const int N_CTRL = _N_CTRL;

  static const int N_NRPN = _N_NRPN;

private:
  
  static const int NOTE = 0;

  static const int CTRL = 1;

  static const int NRPN = 2;

  static const int NRPN_CC_MSB = 99;

  static const int NRPN_CC_LSB = 98;

  static const int NRPN_VAL_MSB = 6;

  static const int NRPN_VAL_LSB = 38;

 public:

  Midi(usb_midi_class _usbMIDI, int _channel=1) : usbMIDI(_usbMIDI), channel(_channel) { }

 private:

  int8_t channel;
  
  usb_midi_class usbMIDI;
  
 public:

  bool channel_is_valid(int channel) { return channel >= 1 && channel <= 16; }

  int8_t channel_get(void) { return channel; }

  void channel_set(int _channel) { if (channel_is_valid(_channel)) channel = _channel; }

 private:

  int16_t values[N_NOTE+N_CTRL+N_NRPN];

  int get(int index) { return values[index]; }

  void set(int index, int value) {
    if ( ! (flags[value]&READ_ONLY)) values[index] = value;
    invoke_listeners(index);
    send(index, values[index]);
  }

  void set_from_input(int index, int value) {
    if ( ! (flags[index]&READ_ONLY)) values[index] = value;
    invoke_listeners(index);
    if (flags[index]&ECHO_ENABLE) send(index, values[index]);
  }
  
  void send(int index, int value) {
    if (value == -1) return;
    if ( ! (flags[index]&OUTPUT_ENABLE)) return;
    if (note_lower(index) >= 0)
      usbMIDI.sendNoteOn(note_lower(index), value, channel);
    else if (ctrl_lower(index) >= 0)
      usbMIDI.sendControlChange(ctrl_lower(index), value, channel);
    else {
      usbMIDI.beginNrpn(nrpn_lower(index), channel);
      usbMIDI.sendNrpnValue(value, channel);
    }
  }

  int note_hoist(int note) { return note; }

  int note_lower(int index) { return note_is_valid(index) ? index : -1; }

  int ctrl_hoist(int ctrl) { return N_NOTE+ctrl; }

  int ctrl_lower(int index) { return ctrl_is_valid(index-N_NOTE) ? index-N_NOTE : -1; }

  int nrpn_hoist(int nrpn) { return N_NOTE+N_CTRL+nrpn; }

  int nrpn_lower(int index) { return nrpn_is_valid(index-(N_NOTE+N_CTRL)) ? index-(N_NOTE+N_CTRL) : -1; }

  int lower(int index) {
    return (unsigned)index < N_NOTE ? index : (unsigned)index < N_NOTE+N_CTRL ? index-N_NOTE : index - (N_NOTE+N_CTRL);
  }

  int lower_type(int index) {
    return (unsigned)index < N_NOTE ? NOTE : (unsigned)index < N_NOTE+N_CTRL ? CTRL : NRPN;
  }
  
 public:

  int note_is_valid(int note) { return (unsigned)note < N_NOTE; }

  int note_get(int note) { return note_is_valid(note) ? get(note_hoist(note)) : -1; }

  void note_set(int note, int value) { if (note_is_valid(note)) set(note_hoist(note), value); }

  bool ctrl_is_valid(int ctrl) { return (unsigned)ctrl < N_CTRL; }

  int ctrl_get(int ctrl) { return ctrl_is_valid(ctrl) ? get(ctrl_hoist(ctrl)) : -1; }

  void ctrl_set(int ctrl, int value) { if (ctrl_is_valid(ctrl)) set(ctrl_hoist(ctrl), value); }

  bool nrpn_is_valid(int nrpn) { return (unsigned)nrpn < N_NRPN; }

  int16_t nrpn_get(int nrpn) { return nrpn_is_valid(nrpn) ? get(nrpn_hoist(nrpn)) : -1; }

  void nrpn_set(int nrpn, int value) { if (nrpn_is_valid(nrpn)) set(nrpn_hoist(nrpn), value); }

 private:

  uint8_t flags[N_NOTE+N_CTRL+N_NRPN];

  int flags_get(int index) { return flags[index]; }

  void flags_set(int index, int value) { 
    flags[index] &= ~(INPUT_ENABLE|OUTPUT_ENABLE|ECHO_ENABLE|READ_ONLY);
    flags[index] |= value & (INPUT_ENABLE|OUTPUT_ENABLE|ECHO_ENABLE|READ_ONLY);
  }
  
 public:

  const uint8_t INPUT_ENABLE = 0x80;

  const uint8_t OUTPUT_ENABLE = 0x40;

  const uint8_t ECHO_ENABLE = 0x20;

  const uint8_t READ_ONLY = 0x10;

 private: 

  const uint8_t LISTENER_ACTIVE = 0x80;

 public:

  int note_flags_get(int note) { return note_is_valid(note) ? flags_get(note_hoist(note)) : -1; }

  void note_flags_set(int note, int value) { if (note_is_valid(note)) flags_set(note_hoist(note), value); }

  int ctrl_flags_get(int ctrl) { return ctrl_is_valid(ctrl) ? flags_get(ctrl_hoist(ctrl)) : -1; }

  void ctrl_flags_set(int ctrl, int value) { ctrl_is_valid(ctrl) ? flags_set(ctrl_hoist(ctrl), value) : -1; }

  int nrpn_flags_get(int nrpn) { return nrpn_is_valid(nrpn) ? flags_get(nrpn_hoist(nrpn)) : -1; }

  void nrpn_flags_set(int nrpn, int value) { nrpn_is_valid(nrpn) ? flags_set(nrpn_hoist(nrpn), value) : -1; }

  int flags_encode(const int input_enable, const int output_enable, const int echo_enable, const int read_only) {
    return (input_enable ? INPUT_ENABLE : 0) | 
      (output_enable ? OUTPUT_ENABLE : 0) |
      (echo_enable ? ECHO_ENABLE : 0) |
      (read_only ? READ_ONLY : 0);
  }

 private:

  class listener_t {
  listener_t(int index, void (*_listener)(int t)) : listener(_listener) {
      if (index != -1) {
	next = listeners[index];
	listeners[index] = this;
      }
    }
    listener_t *next;
    void (*listener)(int t);
  };

  /*
  ** could do this with a 1 byte listener chain id
  ** per index rather than a full pointer per index
  */
  listener_t *listeners[N_NOTE+N_CTRL+N_NRPN];

 public:

  class note_listener_t : listener_t {
   note_listener_t(int note, int (*listener)(int t)) {
     super(note_is_valid(note) ? note_hoist(note) : -1, listener);
   }
 };

 class ctrl_listener_t : listener_t {
   ctrl_listener_t(int ctrl, int (*listener)(int t)) {
     super(ctrl_is_valid(ctrl) ? ctrl_hoist(ctrl) : -1, listener);
   }
 };

 class nrpn_listener_t : listener_t {
   nrpn_listener_t(int nrpn, int (*listener)(int t)) {
     super(nrpn_is_valid(nrpn) ? nrpn_hoist(nrpn) : -1, listener);
   }
 };

  int listeners_installed(void) {
    int count = 0;
    for (int index = note_hoist(0); index < nrpn_hoist(N_NRPN); index += 1)
      count += listeners[index] != NULL;
    return count;
  }

 private:

  void invoke_listeners(int index) {
    if (flags[index]&LISTENER_ACTIVE) {
      switch (lower_type(index)) {
      case NOTE: Serial.printf("listener loop at note %d\n", note_lower(index)); break;
      case CTRL: Serial.printf("listener loop at ctrl %d\n", ctrl_lower(index)); break;
      case NRPN: Serial.printf("listener loop at nrpn %d\n", nrpn_lower(index)); break;
      default: Serial.printf("listener loop at unknown index %d\n", index); break;
      }
      return;
    }
    const int lowered = lower(index);
    flags[index] |= LISTENER_ACTIVE;
    for (listener_t *lp = listeners[index]; *lp != NULL; lp = lp->next)
      lp->listener(lowered);
    flags[index] &= ~LISTENER_ACTIVE;
  }
  
 public:

  void setup(void) {
    for (int i = note_hoist(0); i < nrpn_hoist(N_NRPN); i += 1) {
      values[i] = -1;
      flags[i] = 0;
      listeners[i] = NULL;
    }
  }

  void loop(void) {
    /* dispatch incoming midi */
    while (usbMIDI.read(channel)) {
      const type = usbMIDI.getType();
      if (type == case usbMIDI.ControlChange) {
	const int ctrl = usbMIDI.getData1();
	if ( ! ctrl_is_valid(ctrl)) continue;
	const int index = ctrl_hoist(ctrl);
	if ( ! (flags[index]&INPUT_ENABLE)) continue;
	set_from_input(index, usbMIDI.getData2());
	/* 
	** recognize nrpns, could do from a listener,
	** but I can't figure out how to set one up
	** from inside the class.
	*/
	if (ctrl == NRPN_VAL_LSB) {
	  const int nrpn = (nrpn_get(NRPN_CC_MSB)<<7)|nrpn_get(NRPN_CC_LSB);
	  if (nrpn_is_valid(nrpn))
	    set_from_input(nrpn_hoist(nrpn), (nrpn_get(NRPN_VAL_MSB)<<7)|nrpn_get(NRPN_VAL_LSB));
	}
      } else if (type ==  usbMIDI.NoteOn) {
	const int note = usbMIDI.getData1();
	if ( ! note_is_valid(note)) continue;
	const int index = note_hoist(ctrl);
	if ( ! (flags[index]&INPUT_ENABLE)) continue;
	set_from_input(index, usbMIDI.getData2());
	continue;
      } else if (type == usbMIDI.NoteOff) {
	const int note = usbMIDI.getData1();
	if ( ! note_is_valid(note)) continue;
	const int index = note_hoist(ctrl);
	if ( ! (flags[index]&INPUT_ENABLE)) continue;
	set_from_input(note, 0);
      }
    }
    /* dispatch every timeouts? */
    /* dispatch after timeouts? */
  }
};

static Midi<128,128,256> midi(usbMIDI, 1);

#ifdef __cplusplus
extern "C" {
#endif

static void midi_setup(void) { midi.setup(); }

static void midi_loop(void) { midi.loop(); }
    
static int midi_flags_encode(const int input_enable, const int output_enable, const int echo_enable, const int read_only) {
  return midi.flags_encode(input_enable, output_enable, echo_enable, read_only
}

#ifdef __cplusplus
}
#endif
