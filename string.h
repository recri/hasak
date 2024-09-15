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
** string transfers
*/

/*
The string NRPNs all transfer a string endpoint identifier in the high
byte of the value so that multiple strings can be transferred without
interference. The low bytes of NRPN_STRING_START and NRPN_STRING_END
specify the string target. The low byte of NRPN_STRING_BYTE specifies
a 7 bit ASCII value. 

To accomodate WinKey encapsulation, we could restrict the endpoint
identifier to 6 bits and send the 8th bit of 8 bit ASCII in the high
byte. Hmm, that would also encode UNICODE [Basic Latin (ASCII)], and
[Latin-1 Supplement], which gives us all the common european latin
characters. 

The string endpoints on the keyer are max 128 byte ring buffers. A
successful NRPN_STRING_BYTE sent to the either side echoes the
endpoint identifier in the high byte of the response and the
min(number of remaining writeable bytes in the ring buffer, 127) in
the low byte. If there is no room for the byte received, it will be
discarded and there will be no response echoed. 

The string target byte is used to identify different producers and
consumers of strings. But we can leave this undefined, the JSON for
the keyer should enumerate the implemented targets, the number of
endpoints, the maximum size of the endpoints, and so on. The
NRPN_ID_JASON request will echo back with a string endpoint in the
high byte and a string target in the low byte, or not respond if the
keyer doesn't know its JSON descriptor. 

The string targets on the keyer side are text keyers and winkey
emulators, so they will usually be draining the endpoint ring buffer
as fast as keyer speed allows. Those endpoints may remain open for the
length of the session.
*/

/*
** endpoints
**  some are local sources of bytes
**	a static string compiled into the code image, like the json
**	a stream source, like the decoded text
**	a dynamic string stored in memory, like a saved message
**  some are local sinks of bytes
**	a stream sink, like the text keyers
**	a dynamic string, like a saved message
** 
*/
typedef struct {
  int open, source, id, length, sent, rcvd, last, stalled;
  const char *string;
} string_endpoint_t;

static string_endpoint_t string_endpoint[KYR_N_ENDP];

static elapsedMicros string_timeout;;

static void string_sample(int _, int __, int ___) {
  if ((int)string_timeout < 0) return;
  string_timeout = -nrpn_get(NRPN_STRING_THROTTLE);
  for (int i = 0; i < KYR_N_ENDP; i += 1) {
    string_endpoint_t *p = &string_endpoint[i];
    if ( ! p->open)
      continue;				// endpoint active
    if (i == ENDP_JSON_TO_HOST) {
      if (p->sent == p->length) {		// all sent
	nrpn_send(NRPN_STRING_END, (p->id<<9)); // end string
	p->open = 0;				// close endpoint
	// nrpn_set(NRPN_PADC_ENABLE, string_adc_enable_save);
      } else {					// more to send
	if ((p->last&0xff) > 0) {		// last ack said go
	  // midi_flush();
	  if (p->string[p->sent] & 0x80) Serial.printf("8 bit character at index %d\n", p->sent);
	  nrpn_send(NRPN_STRING_BYTE, (p->id << 9) | (p->string[p->sent++]&0xFF));
	  // midi_flush();
	}
			 			// waiting for xon
      }
    } else {
      Serial.printf("string endpoint %d not implemented\n", i);
    }
  }
}

#include "json.h"

static void string_start(string_endpoint_t *p) {
  int i = p-&string_endpoint[0];
  if (i == ENDP_JSON_TO_HOST) {
    if ( ! p->open) {
      p->open = 1;
      p->id = 0;			// json end point
      p->length = sizeof(json_string)-1; // string bytes
      p->string = json_string;	// string length
      p->sent = 0;		// number sent
      p->last = 1;		// last acknowledgement
      nrpn_send(NRPN_STRING_START, (p->id << 9));
      Serial.printf("string_start JSON %d bytes\n", p->length);
      //string_adc_enable_save = nrpn_get(NRPN_PADC_ENABLE);
      //nrpn_set(NRPN_PADC_ENABLE, 0);
    }
  } else {
    Serial.printf("string_start %d not implemented\n", i);
  }
}

static void string_byte(string_endpoint_t *p, const int payload) {
  int id = payload>>9;
  if (id == ENDP_JSON_TO_HOST) {
    string_endpoint[0].last = payload;
  }
}	

static void string_end(string_endpoint_t *p) {
}

static void string_start_listener(int _, int __, int e) {
  if (e >= 0 && e < KYR_N_ENDP)
    string_start(&string_endpoint[e]);
}

static void string_end_listener(int _, int __, int e) {
  if (e >= 0 && e < KYR_N_ENDP)
    string_end(&string_endpoint[e]);
}

static void string_byte_listener(int _, int __, int b) {
  int e = b >> 9;
  if (e >= 0 && e < KYR_N_ENDP) 
    string_byte(&string_endpoint[e], b);
}

static void string_setup(void) {
  nrpn_listen(NRPN_STRING_START, string_start_listener);
  nrpn_listen(NRPN_STRING_END, string_end_listener);
  nrpn_listen(NRPN_STRING_BYTE, string_byte_listener);
  nrpn_listen(NRPN_SAMPLE, string_sample);
}
