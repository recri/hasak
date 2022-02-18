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

#ifdef __cplusplus
extern "C" {
#endif

static int ctrl_invalid(const int ctrl, const char *who) {
  Serial.printf("invalid ctrl %d in %s\n", ctrl, who);
  return -1;
}

static int ctrl_is_valid(int ctrl) { return midi.ctrl_is_valid(ctrl); }
  
static void ctrl_get(int ctrl) { 
  return ctrl_is_valid(ctrl) ? midi.ctrl_get(ctrl) : ctrl_invalid(ctrl, "ctrl_get");
}

static void ctrl_set(int ctrl, int value) { 
  if (ctrl_is_valid(ctrl))
    midi.ctrl_set(ctrl, value);
  else
    ctrl_invalid(ctrl, "ctrl_set");
}

/* define a ctrl */
static void ctrl_define(const int ctrl, const int value, 
			const int input_enable, const int output_enable, 
			const int echo_enable, const int read_only) {
  if (ctrl_is_valid(ctrl)) {
    midi.ctrl_flags_set(ctrl, midi_flags_encode(input_enable, output_enable, echo_enable, read_only));
    nprn_set(ctrl, value);
  } else
    ctrl_invalid(ctrl, "nrpn_define");
}

static void ctrl_setup(void) {
  /* need to catch incoming ctrls and translate to nrpn */
#if 0
    MIDI_NRPN_CC_MSB              = 99,
    MIDI_NRPN_CC_LSB              = 98,
    MIDI_NRPN_VAL_MSB             = 6,
    MIDI_NRPN_VAL_LSB             = 38,

    MIDI_MASTER_VOLUME            = 7,      // set master volume
    MIDI_MASTER_BALANCE           = 8,      //TODO: stereo balance
    MIDI_MASTER_PAN               = 10,     //TODO: stereo position of CW tone

    MIDI_SIDETONE_VOLUME          = 12,     // set sidetone volume
    MIDI_SIDETONE_FREQUENCY       = 13,     // set sidetone frequency

    MIDI_INPUT_LEVEL              = 16,     //TODO:

    MIDI_ENABLE_POTS              = 64,     // enable/disable potentiometers
    MIDI_KEYER_AUTOPTT            = 65,     // enable/disable auto-PTT from CW keyer
    MIDI_RESPONSE                 = 66,     // enable/disable reporting back to SDR and MIDI controller
    MIDI_MUTE_CWPTT               = 67,     // enable/disable muting of RX audio during auto-PTT
    MIDI_MICPTT_HWPTT             = 68,     // enable/disable that MICIN triggers the hardware PTT output
    MIDI_CWPTT_HWPTT              = 69,     // enable/disable that CWPTT triggers the hardware PTT output

    MIDI_KEYER_HANG               = 72,     // set Keyer hang time (if auto-PTT active)
    MIDI_KEYER_LEADIN             = 73,     // set Keyer lead-in time (if auto-PTT active)
    MIDI_CW_SPEED                 = 74,     // set CW speed
    MIDI_INPUT_SELECT             = 75,     //TODO:
    MIDI_SET_CHANNEL              = 119     // Change the default channel to use
#endif
}
static void ctrl_loop(void) {
}

#ifdef __cplusplus
}
#endif
