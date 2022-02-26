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

#include "src/midi.h"

static Midi<KYR_N_NOTE,KYR_N_CTRL,KYR_N_NRPN> midi(usbMIDI, 1);

// not until I'm sure it's needed
// #ifdef __cplusplus
// extern "C" {
// #endif

#define midi_listener_t Midi<KYR_N_NOTE,KYR_N_CTRL,KYR_N_NRPN>::listener_t
#define note_listen(n, f) {static midi_listener_t t(f); midi.note_listen(n,&t);}
#define ctrl_listen(c, f) {static midi_listener_t t(f); midi.ctrl_listen(c,&t);}
#define nrpn_listen(p, f) {static midi_listener_t t(f); midi.nrpn_listen(p,&t);}

static int midi_flags_encode(const int input_enable, const int output_enable, const int echo_enable, const int read_only) {
  return midi.flags_encode(input_enable, output_enable, echo_enable, read_only);
}

/*
** Notes
*/
static int note_invalid(const int note, const char *who) {
  Serial.printf("invalid note %d in %s\n", note, who);
  return -1;
}

static int note_is_valid(int note) { return midi.note_is_valid(note); }
  
static int note_get(int note) { 
  return note_is_valid(note) ? midi.note_get(note) : note_invalid(note, "note_get");
}

static void note_set(int note, int value) { 
  if (note_is_valid(note)) {
    Serial.printf("note_set(%d, %d)\n", note, value);
    midi.note_set(note, value);
  } else
    note_invalid(note, "note_set");
}

static void note_echo(int note, int value) {
  if (note_is_valid(note))
    midi.note_echo(note, value);
  else
    note_invalid(note, "note_echo");
}

/* define a note */
static void note_define(const int note, const int value, 
			const int input_enable, const int output_enable, 
			const int echo_enable, const int read_only) {
  if (note_is_valid(note))
    midi.note_define(note, value, input_enable, output_enable, echo_enable, read_only);
  else
    note_invalid(note, "note_define");
}

static void note_toggle(const int note) { note_set(note, note_get(note) ^ 1); } 

static void note_on(const int note) { note_set(note, 1); } 

static void note_off(const int note) { note_set(note, 0); } 
    
/*
** Control changes.
*/

static int ctrl_invalid(const int ctrl, const char *who) {
  Serial.printf("invalid ctrl %d in %s\n", ctrl, who);
  return -1;
}

static int ctrl_is_valid(int ctrl) { return midi.ctrl_is_valid(ctrl); }
  
static int ctrl_get(int ctrl) { 
  return ctrl_is_valid(ctrl) ? midi.ctrl_get(ctrl) : ctrl_invalid(ctrl, "ctrl_get");
}

static void ctrl_set(int ctrl, int value) { 
  if (ctrl_is_valid(ctrl))
    midi.ctrl_set(ctrl, value);
  else
    ctrl_invalid(ctrl, "ctrl_set");
}

static void ctrl_echo(int ctrl, int value) { 
  if (ctrl_is_valid(ctrl))
    midi.ctrl_echo(ctrl, value);
  else
    ctrl_invalid(ctrl, "ctrl_echo");
}

/* define a ctrl */
static void ctrl_define(const int ctrl, const int value, 
			const int input_enable, const int output_enable, 
			const int echo_enable, const int read_only) {
  if (ctrl_is_valid(ctrl))
    midi.ctrl_define(ctrl, value, input_enable, output_enable, echo_enable, read_only);
  else
    ctrl_invalid(ctrl, "ctrl_define");
}

/*
** Nonregistered parameter numbers.
** xnrpn refers to a pair of nrpn taken as 2 14 bit values assembled into a 28 bit integer
** nrpns and xnrpns can be signed or unsigned
*/

/* report invalid access */
static int nrpn_invalid(const int nrpn, const char *who) {
  Serial.printf("invalid nrpn %d in %s\n", nrpn, who);
  return -1;
}

/* report invalid access */
static int xnrpn_invalid(const int nrpn, const char *who) {
  Serial.printf("invalid xnrpn %d in %s\n", nrpn, who);
  return -1;
}

/* valid nrpn test */
static int nrpn_is_valid(const int nrpn) {
  return midi.nrpn_is_valid(nrpn);
}

/* fetch a nrpn */
static int nrpn_get(const int nrpn) { 
  return nrpn_is_valid(nrpn) ? midi.nrpn_get(nrpn) : nrpn_invalid(nrpn, "nrpn_get");
}

/* fetch an xnrpn */  
static int xnrpn_get(const int nrpn) {
  return nrpn_is_valid(nrpn+1) ? (midi.nrpn_get(nrpn)<<14)|midi.nrpn_get(nrpn+1) : xnrpn_invalid(nrpn, "xnrpn_get");
}

/* set a nrpn */
static inline void nrpn_set(const int nrpn, const int value) {
  if (nrpn_is_valid(nrpn))
    midi.nrpn_set(nrpn, value);
  else
    nrpn_invalid(nrpn, "nrpn_set");
}

static inline void nrpn_echo(const int nrpn, const int value) {
  if (nrpn_is_valid(nrpn))
    midi.nrpn_echo(nrpn, value);
  else
    nrpn_invalid(nrpn, "nrpn_echo");
}

/* set an xnrpn */
static inline void xnrpn_set(const int nrpn, const int value) {
  if (nrpn_is_valid(nrpn+1)) {
    midi.nrpn_set(nrpn, (value>>14)&0x3fff);
    midi.nrpn_set(nrpn+1, value&0x3fff);
  } else
    xnrpn_invalid(nrpn, "xnrpn_set");
}

static inline void xnrpn_echo(const int nrpn, const int value) {
  if (nrpn_is_valid(nrpn+1)) {
    midi.nrpn_echo(nrpn, (value>>14)&0x3fff);
    midi.nrpn_echo(nrpn+1, value&0x3fff);
  } else
    xnrpn_invalid(nrpn, "xnrpn_echo");
}

/* define a nrpn */
static void nrpn_define(const int nrpn, const int value, 
			const int input_enable, const int output_enable, 
			const int echo_enable, const int read_only) {
  if (nrpn_is_valid(nrpn))
    midi.nrpn_define(nrpn, value, input_enable, output_enable, echo_enable, read_only);
  else
    nrpn_invalid(nrpn, "nrpn_define");
}

/*
** external linkage.
*/
int get_note(int note) { return note_get(note); }
int get_ctrl(int ctrl) { return ctrl_get(ctrl); }
int get_nrpn(int nrpn) { return nrpn_get(nrpn); }
int get_xnrpn(int xnrpn) { return xnrpn_get(xnrpn); }

/*
** setup and loop.
*/

static void midi_setup(void) { midi.setup(); }

static void midi_loop(void) { midi.loop(); }
    
