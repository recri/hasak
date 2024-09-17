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

  static const int N_NOTE = _N_NOTE; // the number of notes

  static const int N_CTRL = _N_CTRL; // the number of control changes (CC)

  static const int N_NRPN = _N_NRPN; // the number of nonregistered parameter numbers (NRPN)

  const uint8_t INPUT_ENABLE = 0x80; // flags[] bit that permits input of value

  const uint8_t OUTPUT_ENABLE = 0x40; // flags[] bit that permits output of value

  const uint8_t ECHO_ENABLE = 0x20; // flags[] bit that permits echo of value

  const uint8_t READ_ONLY = 0x10; // flags[] bit that prevents writing from input

  Midi(usb_midi_class _usbMIDI) : usbMIDI(_usbMIDI) {}

private:

  static const int NOTE = 0;	// note type in generics

  static const int CTRL = 1;	// CC type in generics

  static const int NRPN = 2;	// NRPN type in generics

  static const int NRPN_CC_MSB = 99; // CC used to encode NRPN

  static const int NRPN_CC_LSB = 98; // CC used to encode NRPN

  static const int NRPN_VAL_MSB = 6; // CC used to encode NRPN

  static const int NRPN_VAL_LSB = 38; // CC used to encode NRPN

  usb_midi_class usbMIDI;	// the midi interface

  int8_t notes[N_NOTE];		// the note values

  int8_t ctrls[N_CTRL];		// the CC values

  int16_t nrpns[N_NRPN];	// the NRPN values

  const uint8_t LISTENER_ACTIVE = 0x08; // flags[] bit that marks invoke_listener active on chain

  uint8_t flags[N_NOTE+N_CTRL+N_NRPN]; // flags[] values

  listener_t *listeners[N_NOTE+N_CTRL+N_NRPN]; // listener lists

  //
  // core operations, unguarded, unqualified, undecorated
  //
  
  // get a value
  int _get(int type, int tindex) { 
    switch (type) {
    case NOTE: return notes[tindex];
    case CTRL: return ctrls[tindex];
    case NRPN: return nrpns[tindex];
    default: Serial.printf("midi._get(%d, %d) invalid type %d\n", type, tindex, type); return -1;
    }
  }
  // set a value
  void _set(int type, int tindex, int value) {
    switch (type) {
    case NOTE: notes[tindex] = value; break;
    case CTRL: ctrls[tindex] = value; break;
    case NRPN: nrpns[tindex] = value; break;
    default: Serial.printf("midi._set(%d, %d, %d) invalid type %d\n", type, tindex, value, type); break;
    }
  }
  // increment a value
  void _incr(int type, int tindex, int value) {
    switch (type) {
    case NOTE: notes[tindex] = (notes[tindex]+value) & 0x7f; break;
    case CTRL: ctrls[tindex] = (ctrls[tindex]+value) & 0x7f; break;
    case NRPN: nrpns[tindex] = (nrpns[tindex]+value) & 0x3fff; break;
    default: Serial.printf("midi._incr(%d, %d, %d) invalid type %d\n", type, tindex, value, type); break;
    }
  }
  // send a value
  void _send(int type, int tindex, int value) {
    const int channel = nrpn_get(NRPN_CHANNEL);
    switch (type) {
    case NOTE: usbMIDI.sendNoteOn(tindex, value, channel); break;
    case CTRL: usbMIDI.sendControlChange(tindex, value, channel); break;
    case NRPN: usbMIDI.beginNrpn(tindex, channel); usbMIDI.sendNrpnValue(value, channel); usbMIDI.endNrpn(channel); break;
    default: Serial.printf("midi._send(%d, %d, %d, %d) invalid type %d\n", type, tindex, value, type); break;
    }
  }
  // hoist an index into the unified index
  int _hoist(int type, int tindex) {
    switch (type) {
    case NOTE: return tindex;
    case CTRL: return N_NOTE+tindex;
    case NRPN: return N_NOTE+N_CTRL+tindex;
    default: Serial.printf("midi._hoist(%d, %d) invalid type %d\n", type, tindex, type); return -1;
    }
  }
  // get the flags on a value
  int _flags_get(int type, int tindex) { return flags[_hoist(type,tindex)]; }

  // set the flags on a value
  void _flags_set(int type, int tindex, int value) { 
    const int index = _hoist(type,tindex);
    flags[index] = (flags[index] & ~(INPUT_ENABLE|OUTPUT_ENABLE|ECHO_ENABLE|READ_ONLY)) | (value & (INPUT_ENABLE|OUTPUT_ENABLE|ECHO_ENABLE|READ_ONLY));
  }

  //
  // middle level interface
  //
  // is a value enabled for input
  bool input_enabled(int type, int tindex) { return (_flags_get(type, tindex) & INPUT_ENABLE) != 0; }
  // is a value enabled for output
  bool output_enabled(int type, int tindex) { return (_flags_get(type, tindex) & OUTPUT_ENABLE) != 0; }
  // is a value enabled for echo
  bool echo_enabled(int type, int tindex) { return (_flags_get(type, tindex) & ECHO_ENABLE) != 0; }
  // is a value enabled for remote writing
  bool write_enabled(int type, int tindex) { return (_flags_get(type, tindex) & READ_ONLY) == 0; }
  // listen to a note, ctrl, or nrpn
  void listen(int type, int tindex, void (*f)(int,int)) {
    const int index = _hoist(type, tindex);
    if (listeners[index] == NULL) nrpn_incr(NRPN_LISTENER_LISTS);
    nrpn_incr(NRPN_LISTENER_NODES);
    listener_add(&listeners[index], f);
  }
  // remove a previously installed listener
  void unlisten(int type, int tindex, void (*f)(int,int)) {
    const int index = _hoist(type, tindex);
    if (flags[index]&LISTENER_ACTIVE) // may be able to remove this restriction
      nrpn_incr(NRPN_LISTENER_LOOPS);
    else if (listeners[index] != NULL) {
      if (listener_remove(&listeners[index], f))
	nrpn_incr(NRPN_LISTENER_NODES, -1);
      if (listeners[index] == NULL)
	nrpn_incr(NRPN_LISTENER_LISTS, -1);
    }
  }
  // invoke listeners on a value
  void invoke_listeners(int type, int tindex, int oldvalue) {
    const int index = _hoist(type, tindex);
    if (flags[index]&LISTENER_ACTIVE) {
      nrpn_incr(NRPN_LISTENER_LOOPS); // may not re-enter a listener chain
    } else {
      flags[index] |= LISTENER_ACTIVE;
      int n = listener_invoke(listeners[index], tindex, oldvalue);
      if (n > 0) {
	nrpn_incr(NRPN_LISTENER_FIRES);
	nrpn_incr(NRPN_LISTENER_CALLS, n);
      }
      flags[index] &= ~LISTENER_ACTIVE;
    }
  }
  //
  // guarded, qualified, and decorated versions
  // type and tindex have been guarded
  //
  // get from any source
  int get(int type, int tindex) { return _get(type, tindex); }
  // set from device origin
  void set(int type, int tindex, int value) {
    // Serial.printf("midi.set(%d, %d, %d, %d)\n", index, value, type, tindex);
    const int oldvalue = _get(type, tindex);
    _set(type, tindex, value);
    if (nrpn_get(NRPN_LISTENER_ENABLE)) 
      invoke_listeners(type, tindex, oldvalue);
    if (nrpn_get(NRPN_OUTPUT_ENABLE) && output_enabled(type, tindex)) {
      // Serial.printf("midi.set(type=%d, tindex=%d, old=%d, new=%d) and sending to midi.\n", type, tindex, oldvalue, value);
      _send(type, tindex, _get(type,tindex));
    }
  }
  // unset from device origin, not sure if this is useful
  void unset(int type, int tindex) { _set(type, tindex, -1); }
  // set from input received
  void set_from_input(int type, int tindex, int value) {
    const int oldvalue = _get(type, tindex);
    if (write_enabled(type,tindex)) 
      _set(type, tindex, value);
    if (nrpn_get(NRPN_LISTENER_ENABLE))
      invoke_listeners(type, tindex, oldvalue);
    if (nrpn_get(NRPN_ECHO_ENABLE) && echo_enabled(type,tindex))
      send(type, tindex, _get(type, tindex));
  }
  // send from device origin
  void send(int type, int tindex, int value) {
    if (nrpn_get(NRPN_OUTPUT_ENABLE) && output_enabled(type, tindex) && value != -1) _send(type, tindex, value);
  }
  // definition of initial value and flags from device
  void define(int type, int tindex, int value, int input_enable, int output_enable, int echo_enable, int read_only) {
    _set(type, tindex, value);
    _flags_set(type, tindex, flags_encode(input_enable, output_enable, echo_enable, read_only));
  }
  
 public:			// external interface

  // notes interface

  int note_is_valid(int note) { return (unsigned)note < N_NOTE; }

  int note_get(int note) { return note_is_valid(note) ? get(NOTE, note) : -1; }

  void note_set(int note, int value) { if (note_is_valid(note)) set(NOTE, note, value&0x7f); }

  void note_unset(int note) { if (note_is_valid(note)) unset(NOTE, note); }

  void note_send(int note, int value) { if (note_is_valid(note)) send(NOTE, note, value&0x7f); }

  void note_define(int note, int value, int input_enable, int output_enable, int echo_enable, int read_only) {
    if (note_is_valid(note)) define(NOTE, note, value&0x7f, input_enable, output_enable, echo_enable, read_only);
  }

  int note_flags_get(int note) { return note_is_valid(note) ? _flags_get(NOTE, note) : -1; }

  void note_flags_set(int note, int value) { if (note_is_valid(note)) _flags_set(NOTE, note, value); }

  void note_listen(const int note, void (*f)(int,int)) { if (note_is_valid(note)) listen(NOTE, note, f); }
  
  void note_unlisten(const int note, void (*f)(int,int)) { if (note_is_valid(note)) unlisten(NOTE, note, f); }
  
  // ctrl interface

  bool ctrl_is_valid(int ctrl) { return (unsigned)ctrl < N_CTRL; }

  int ctrl_get(int ctrl) { return ctrl_is_valid(ctrl) ? get(CTRL, ctrl) : -1; }

  void ctrl_set(int ctrl, int value) { if (ctrl_is_valid(ctrl)) set(CTRL, ctrl, value&0x7f); }

  void ctrl_unset(int ctrl) { if (ctrl_is_valid(ctrl)) unset(CTRL, ctrl); }

  void ctrl_send(int ctrl, int value) { 
    Serial.printf("midi.ctrl_send(%d, %d)\n", ctrl, value);
    if (ctrl_is_valid(ctrl)) send(CTRL, ctrl, value&0x7f);
  }

  void ctrl_define(int ctrl, int value, int input_enable, int output_enable, int echo_enable, int read_only) {
    if (ctrl_is_valid(ctrl)) define(CTRL, ctrl, value&0x7f, input_enable, output_enable, echo_enable, read_only);
  }

  int ctrl_flags_get(int ctrl) { return ctrl_is_valid(ctrl) ? _flags_get(CTRL, ctrl) : -1; }

  void ctrl_flags_set(int ctrl, int value) { ctrl_is_valid(ctrl) ? _flags_set(CTRL, ctrl, value) : -1; }

  void ctrl_listen(const int ctrl, void (*f)(int,int)) { if (ctrl_is_valid(ctrl)) listen(CTRL, ctrl, f); }
  
  void ctrl_unlisten(const int ctrl, void (*f)(int,int)) { if (ctrl_is_valid(ctrl)) unlisten(CTRL, ctrl, f); }
  
  // nrpn interface
  
  bool nrpn_is_valid(int nrpn) { return (unsigned)nrpn < N_NRPN; }

  int16_t nrpn_get(int nrpn) { return nrpn_is_valid(nrpn) ? get(NRPN, nrpn) : -1; }

  void nrpn_set(int nrpn, int value) { if (nrpn_is_valid(nrpn)) set(NRPN, nrpn, value&0x3fff); }

  void nrpn_unset(int nrpn) { if (nrpn_is_valid(nrpn)) unset(NRPN, nrpn); }

  void nrpn_send(int nrpn, int value) { 
    Serial.printf("midi.nrpn_send(%d, %d)\n", nrpn, value);
    if (nrpn_is_valid(nrpn)) send(NRPN, nrpn, value&0x3fff);
 }

  void nrpn_incr(int nrpn, int n=1) { if (nrpn_is_valid(nrpn)) nrpn_set(nrpn, nrpn_get(nrpn)+n); }

  void nrpn_define(int nrpn, int value, int input_enable, int output_enable, int echo_enable, int read_only) {
    if (nrpn_is_valid(nrpn)) define(NRPN, nrpn, value&0x3fff, input_enable, output_enable, echo_enable, read_only);
  }

  int nrpn_flags_get(int nrpn) { return nrpn_is_valid(nrpn) ? _flags_get(NRPN, nrpn) : -1; }

  void nrpn_flags_set(int nrpn, int value) { nrpn_is_valid(nrpn) ? _flags_set(NRPN, nrpn, value) : -1; }

  void nrpn_listen(const int nrpn, void (*f)(int,int)) { if (nrpn_is_valid(nrpn)) listen(NRPN, nrpn, f); }

  void nrpn_unlisten(const int nrpn, void (*f)(int,int)) { if (nrpn_is_valid(nrpn)) unlisten(NRPN, nrpn, f); }

  // miscellaneous interface

  int flags_encode(int input_enable, int output_enable, int echo_enable, int read_only) {
    return (input_enable ? INPUT_ENABLE : 0) | (output_enable ? OUTPUT_ENABLE : 0) |
      (echo_enable ? ECHO_ENABLE : 0) | (read_only ? READ_ONLY : 0);
  }

  // flush usbMIDI buffer

  void flush(void) { usbMIDI.send_now(); }
  
  // setup, verify expectations

  void setup(void) {
    // all values, flags, and listeners should be initialized to zeroes
    // check that it's true
    for (int note = 0; note < N_NOTE; note += 1)
      if (note_get(note) != 0 || note_flags_get(note) != 0 || listeners[_hoist(NOTE, note)] != NULL)
	Serial.printf("note %d not initialized\n", note);
    for (int ctrl = 0; ctrl < N_CTRL; ctrl += 1)
      if (ctrl_get(ctrl) != 0 || ctrl_flags_get(ctrl) != 0 || listeners[_hoist(CTRL, ctrl)] != NULL)
	Serial.printf("ctrl %d not initialized\n", ctrl);
    for (int nrpn = 0; nrpn < N_NRPN; nrpn += 1)
      if (nrpn_get(nrpn) != 0 || nrpn_flags_get(nrpn) != 0 || listeners[_hoist(NRPN, nrpn)] != NULL)
	Serial.printf("nrpn %d not initialized\n", nrpn);
  }

  // loop, read and dispatch incoming midi messages

  void loop(void) {
    while (usbMIDI.read(nrpn_get(NRPN_CHANNEL))) {
      if (nrpn_get(NRPN_INPUT_ENABLE)) { // apply global input enable

	const int type = usbMIDI.getType();

	if (type == usbMIDI.NoteOn) {

	  const int note = usbMIDI.getData1();
	  if (note_is_valid(note) && input_enabled(NOTE, note))
	    set_from_input(NOTE, note, usbMIDI.getData2());

	} else if (type == usbMIDI.NoteOff) {

	  const int note = usbMIDI.getData1();
	  if (note_is_valid(note) && input_enabled(NOTE, note))
	    set_from_input(NOTE, note, 0);

	} else if (type == usbMIDI.ControlChange) {

	  const int ctrl = usbMIDI.getData1();
	  if (ctrl_is_valid(ctrl) && input_enabled(CTRL, ctrl)) {
	    set_from_input(CTRL, ctrl, usbMIDI.getData2());

	    if (ctrl == NRPN_VAL_LSB) {
	      const int nrpn = (ctrl_get(NRPN_CC_MSB)<<7)|ctrl_get(NRPN_CC_LSB);
	      if (nrpn_is_valid(nrpn) && input_enabled(NRPN, nrpn))
		set_from_input(NRPN, nrpn, (ctrl_get(NRPN_VAL_MSB)<<7)|ctrl_get(NRPN_VAL_LSB));
	    }
	  }
	}
      }
    }
  }
};

#endif
