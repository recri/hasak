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

#include <Arduino.h>
#include "config.h"		// configuration
#include "linkage.h"		// forward references, linkage, and conversions

#include "src/Audio/responsive.h"

int16_t values[16] = {
  1,
  0,
  32,
  KYRV_RAMP_HANN
};

int get_nrpn(int nrpn) {
  switch (nrpn) {
  case NRPN_MIX_EN_USB_L0: return values[0];
  case NRPN_MIX_USB_L0: return values[1];
  case NRPN_MIX_SLEW_TIME: return values[2];
  case NRPN_MIX_SLEW_RAMP: return values[3];;
  default: return 0;
  }
}

PlainRamp pramp(NRPN_MIX_SLEW_RAMP, NRPN_MIX_SLEW_TIME);
ResponsiveRamp rramp(NRPN_MIX_SLEW_RAMP, NRPN_MIX_SLEW_TIME, 0);
ResponsiveGainRamp gramp(NRPN_MIX_USB_L0, NRPN_MIX_EN_USB_L0, NRPN_MIX_SLEW_RAMP, NRPN_MIX_SLEW_TIME);

void setup(void) {
  Serial.begin(115200);
  Serial.printf("hello responsive tester\n");
}

void preport(const char *msg, int n) {
  Serial.printf("%s %d: ", msg, n);
  for (int i = 0; i < n; i += 1) Serial.printf(" %04x%c", pramp.ramp_value(), pramp.in_slew() ? '^' : ' ');
  Serial.printf("\n");
}
void rreport(const char *msg, int n) {
  Serial.printf("%s %d: ", msg, n);
  for (int i = 0; i < n; i += 1) Serial.printf(" %02x%c", rramp.ramp_value(), rramp.in_slew() ? '^' : ' ');
  Serial.printf("\n");
}

void loop(void) {
  if (Serial.available()) {
    char buffer[256];
    for (int i = 0; Serial.available() && i < (int)(sizeof(buffer)-1); i += 1) {
      buffer[i] = Serial.read();
      buffer[i+1] = 0;
    }
    switch (buffer[0]) {
    case '0': pramp.slew(0); preport("prmap.slew(0)", 32); break;
    case '1': pramp.slew(1); preport("prmap.slew(1)", 32); break;
    case '2': preport("pramp no slew", 32); break;

    case '3': rramp.slew(0); rreport("rrmap.slew(0)", 32); break;
    case '4': rramp.slew(255); rreport("prmap.slew(256)", 32); break;
    case '5': rreport("rramp no slew", 32); break;
    }
  }
}
