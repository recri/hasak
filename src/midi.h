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

#ifndef _midi_h_
#define _midi_h_

#include "Arduino.h"
#include "../config.h"
#include "../linkage.h"

/*
** midi interface with listeners
** a template to supply the number of notes, controls, and nrpns to implement
** instantiote with Midi<NNOTE,NCTRL,NNRPN> Midi(usbMidi,channel)
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

  Midi(usb_midi_class _usbMIDI) : usbMIDI(_usbMIDI) {
  }

 private:

  usb_midi_class usbMIDI;

  int8_t notes[N_NOTE];
  int8_t ctrls[N_CTRL];
  int16_t nrpns[N_NRPN];

  int get(int index, int type, int tindex) { 
    return type == NOTE ? notes[tindex] : type == CTRL ? ctrls[tindex] : nrpns[tindex]; 
  }

  void _set(int index, int value, int type, int tindex) {
    switch (type) {
    case NOTE: notes[tindex] = value; break;
    case CTRL: ctrls[tindex] = value; break;
    case NRPN: nrpns[tindex] = value; break;
    default:
      Serial.printf("midi._set(%d, %d, %d, %d) invalid type %d\n", index, value, type, tindex, type);
    }
  }

  // no direct references to values[] beyond this point
  
  void set(int index, int value, int type, int tindex) {
    // Serial.printf("midi.set(%d, %d, %d, %d)\n", index, value, type, tindex);
    _set(index, value, type, tindex);
    invoke_listeners(index, type, tindex);
    send(index, get(index,type,tindex), type, tindex);
  }

  void set_from_input(int index, int value, int type, int tindex) {
    if ( ! (flags[index]&READ_ONLY)) _set(index, value, type, tindex);
    invoke_listeners(index, type, tindex);
    if (flags[index]&ECHO_ENABLE) send(index, get(index, type, tindex), type, tindex);
  }

  void unset(int index, int type, int tindex) {
    _set(index, -1, type, tindex);
  }

  void send(int index, int value, int type, int tindex) {
    if (value == -1) return;
    if ( ! (flags[index]&OUTPUT_ENABLE)) return;
    switch (type) {
    case NOTE: 
      usbMIDI.sendNoteOn(tindex, value, nrpn_get(KYRP_CHANNEL));
      break;
    case CTRL:
      usbMIDI.sendControlChange(tindex, value, nrpn_get(KYRP_CHANNEL));
      break;
    case NRPN:
      usbMIDI.beginNrpn(tindex, nrpn_get(KYRP_CHANNEL));
      usbMIDI.sendNrpnValue(value, nrpn_get(KYRP_CHANNEL));
      break;
    default:
      Serial.printf("midi.send(%d, %d, %d, %d) invalid type %d\n", index, value, type, tindex, type);
      break;
    }
  }

  void define(int index, int value, int input_enable, int output_enable, int echo_enable, int read_only, int type, int tindex) {
    set(index, value, type, tindex);
    flags[index] = flags_encode(input_enable, output_enable, echo_enable, read_only);
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

  int note_get(int note) { return note_is_valid(note) ? get(note_hoist(note), NOTE, note) : -1; }

  void note_set(int note, int value) { if (note_is_valid(note)) set(note_hoist(note), value&0x7f, NOTE, note); }

  void note_unset(int note) { if (note_is_valid(note)) unset(note_hoist(note), NOTE, note); }

  void note_send(int note, int value) { if (note_is_valid(note)) send(note_hoist(note), value&0x7f, NOTE, note); }

  void note_define(int note, int value,
		   int input_enable, int output_enable, 
		   int echo_enable, int read_only) {
    if (note_is_valid(note))
      define(note_hoist(note), value&0x7f, input_enable, output_enable, echo_enable, read_only, NOTE, note);
  }

  bool ctrl_is_valid(int ctrl) { return (unsigned)ctrl < N_CTRL; }

  int ctrl_get(int ctrl) { return ctrl_is_valid(ctrl) ? get(ctrl_hoist(ctrl), CTRL, ctrl) : -1; }

  void ctrl_set(int ctrl, int value) { if (ctrl_is_valid(ctrl)) set(ctrl_hoist(ctrl), value&0x7f, CTRL, ctrl); }

  void ctrl_unset(int ctrl) { if (ctrl_is_valid(ctrl)) unset(ctrl_hoist(ctrl), CTRL, ctrl); }

  void ctrl_send(int ctrl, int value) { if (ctrl_is_valid(ctrl)) send(ctrl_hoist(ctrl), value&0x7f, CTRL, ctrl); }

  void ctrl_define(int ctrl, int value,
		   int input_enable, int output_enable, 
		   int echo_enable, int read_only) {
    if (ctrl_is_valid(ctrl))
      define(ctrl_hoist(ctrl), value&0x7f, input_enable, output_enable, echo_enable, read_only, CTRL, ctrl);
  }

  bool nrpn_is_valid(int nrpn) { return (unsigned)nrpn < N_NRPN; }

  int16_t nrpn_get(int nrpn) { return nrpn_is_valid(nrpn) ? get(nrpn_hoist(nrpn), NRPN, nrpn) : -1; }

  void nrpn_set(int nrpn, int value) { if (nrpn_is_valid(nrpn)) set(nrpn_hoist(nrpn), value&0x3fff, NRPN, nrpn); }

  void nrpn_unset(int nrpn) { if (nrpn_is_valid(nrpn)) unset(nrpn_hoist(nrpn), NRPN, nrpn); }

  void nrpn_send(int nrpn, int value) { if (nrpn_is_valid(nrpn)) send(nrpn_hoist(nrpn), value&0x3fff, NRPN, nrpn); }

  void nrpn_incr(int nrpn, int n=1) { if (nrpn_is_valid(nrpn)) nrpn_set(nrpn, nrpn_get(nrpn)+n); }

  void nrpn_define(int nrpn, int value,
		   int input_enable, int output_enable, 
		   int echo_enable, int read_only) {
    if (nrpn_is_valid(nrpn))
      define(nrpn_hoist(nrpn), value&0x3fff, input_enable, output_enable, echo_enable, read_only, NRPN, nrpn);
  }

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

  const uint8_t LISTENER_ACTIVE = 0x08;

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

public:

  void index_listen(const int index, void (*f)(int)) {
    if (listeners[index] == NULL) nrpn_incr(KYRP_LISTENER_LISTS);
    nrpn_incr(KYRP_LISTENER_NODES);
    listener_add(&listeners[index], f);
  }

private:
  /*
  ** could do this with a 1 byte listener chain id
  ** per index rather than a full pointer per index
  */
  listener_t *listeners[N_NOTE+N_CTRL+N_NRPN];

public:

  void note_listen(const int note, void (*f)(int)) {
    if (note_is_valid(note)) index_listen(note_hoist(note), f);
  }
  
  void ctrl_listen(const int ctrl, void (*f)(int)) {
    if (ctrl_is_valid(ctrl)) index_listen(ctrl_hoist(ctrl), f);
  }
  
  void nrpn_listen(const int nrpn, void (*f)(int)) {
    if (nrpn_is_valid(nrpn)) index_listen(nrpn_hoist(nrpn), f);
  }
  
 private:

  void invoke_listeners(int index, int type, int tindex) {
    if (flags[index]&LISTENER_ACTIVE) {
      nrpn_incr(KYRP_LISTENER_LOOPS);
    } else {
      flags[index] |= LISTENER_ACTIVE;
      int n = listener_invoke(listeners[index], tindex);
      if (n > 0) {
	nrpn_incr(KYRP_LISTENER_FIRES);
	nrpn_incr(KYRP_LISTENER_CALLS, n);
      }
      flags[index] &= ~LISTENER_ACTIVE;
    }
  }
  
 public:

  void setup(void) {
    for (int i = note_hoist(0); i < nrpn_hoist(N_NRPN); i += 1) {
      _set(i, -1, lower_type(i), lower(i));
      flags[i] = 0;
      listeners[i] = NULL;
    }
  }

  void loop(void) {
    /* dispatch incoming midi */
    while (usbMIDI.read(nrpn_get(KYRP_CHANNEL))) {
      const int type = usbMIDI.getType();
      if (type == usbMIDI.ControlChange) {
	const int ctrl = usbMIDI.getData1();
	if ( ! ctrl_is_valid(ctrl)) continue;
	const int index = ctrl_hoist(ctrl);
	if ( ! (flags[index]&INPUT_ENABLE)) continue;
	set_from_input(index, usbMIDI.getData2(), CTRL, ctrl);
	/* 
	** recognize nrpns, could do from a listener,
	** but I can't figure out how to set one up
	** from inside the class.
	*/
	if (ctrl == NRPN_VAL_LSB) {
	  const int nrpn = (nrpn_get(NRPN_CC_MSB)<<7)|nrpn_get(NRPN_CC_LSB);
	  if (nrpn_is_valid(nrpn))
	    set_from_input(nrpn_hoist(nrpn), (nrpn_get(NRPN_VAL_MSB)<<7)|nrpn_get(NRPN_VAL_LSB), NRPN, nrpn);
	}
      } else if (type == usbMIDI.NoteOn) {
	const int note = usbMIDI.getData1();
	if ( ! note_is_valid(note)) continue;
	const int index = note_hoist(note);
	if ( ! (flags[index]&INPUT_ENABLE)) continue;
	set_from_input(index, usbMIDI.getData2(), NOTE, note);
	continue;
      } else if (type == usbMIDI.NoteOff) {
	const int note = usbMIDI.getData1();
	if ( ! note_is_valid(note)) continue;
	const int index = note_hoist(note);
	if ( ! (flags[index]&INPUT_ENABLE)) continue;
	set_from_input(note, 0, NOTE, note);
      }
    }
  }
};

#endif
