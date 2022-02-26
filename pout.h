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
 * digital pin output
 */

static void pout_key_hw_key_out(int note) {
  digitalWriteFast(KYR_KEY_OUT_PIN,  nrpn_get(KYRP_POUT_LOGIC) ? note_get(KYRN_HW_KEY_OUT) : ! note_get(KYRN_HW_KEY_OUT));
}

static void pout_key_hw_ptt_out(int note) {
  digitalWriteFast(KYR_PTT_OUT_PIN, nrpn_get(KYRP_POUT_LOGIC) ? note_get(KYRN_HW_PTT_OUT) : ! note_get(KYRN_HW_PTT_OUT));
}

static void pout_setup(void) {
  pinMode(KYR_PTT_OUT_PIN, OUTPUT); 
  digitalWrite(KYR_PTT_OUT_PIN, ! nrpn_get(KYRP_POUT_LOGIC));
  note_listen(KYRN_HW_KEY_OUT, pout_key_hw_key_out);

  pinMode(KYR_KEY_OUT_PIN, OUTPUT);
  digitalWrite(KYR_PTT_OUT_PIN, ! nrpn_get(KYRP_POUT_LOGIC));
  note_listen(KYRN_HW_PTT_OUT, pout_key_hw_ptt_out);

  // FIX.ME additional input configuration awaiting listener_node_t free list
}

static void pout_loop(void) {
}
