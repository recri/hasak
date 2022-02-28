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
*/
typedef struct {
  char *location;
  int local, size, index;
} string_endpoint_t

static void string_begin_listener(int nrpn) {
}

static void string_end_listener(int nrpn) {
}

static void string_byte_listener(int nrpn) {
}

static void string_setup(void) {
  note_listen(KYRP_STRING_BEGIN, string_begin_listener);
  nrpn_listen(KYRP_STRING_END, string_end_listener);
  nrpn_listen(KYRP_STRING_BYTE, string_byte_listener);
}

// static void cwstptt_loop(void) {}
