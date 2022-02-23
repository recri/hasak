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
** listen to the elements from cwdetime, decode the sequences into ascii
** transcoded into C from the javascript of https://keyer.elf.org
*/

static void cwdecode_setup(void) {
}

static void cwdecode_loop(void) {
}

/* the decode part still in javascript, using a different table */
#if 0
// Local Variables: 
// mode: JavaScript
// js-indent-level: 2
// End:
//
// keyer.js - a progressive web app for morse code
// Copyright (c) 2020 Roger E Critchlow Jr, Charlestown, MA, USA
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// 
import { KeyerDetime } from './KeyerDetime.js';

// translate dit dah strings to text
export class KeyerDecode extends KeyerDetime {
  constructor(context) {
    super(context);
    this.table = null;
    this.elements = [];
    this.elementTimeout = null;

    this.on('element', (elt, timeEnded) => this.onelement(elt, timeEnded));
  }

  elementTimeoutFun() {
    this.elementTimeout = null;
    if (this.elements.length > 0) {
      const code = this.elements.join('');
      const ltr = this.table.decode(code) || '\u25a1';
      // console.log('decode.emit.letter timeout', ltr, code);
      this.emit('letter', ltr, code);
      this.elements = [];
    }
  }

  onelement(elt, timeEnded) {
    // console.log('decode.onelement("'+elt+'", '+timeEnded+')');
    if (this.elementTimeout) {
      clearTimeout(this.elementTimeout);
      this.elementTimeout = null;
    }
    if (elt === '') {
      return;
    }
    if (elt === '.' || elt === '-') {
      this.elements.push(elt);
      this.elementTimeout = setTimeout((...args) => this.elementTimeoutFun(...args), 1000 * (timeEnded - this.context.currentTime) + 250 );
      return;
    }
    if (this.elements.length > 0) {
      const code = this.elements.join('');
      const ltr = this.table.decode(code) || '\u25a1';
      // console.log('decode.emit.letter space', ltr, code);
      this.emit('letter', ltr, code);
      this.elements = [];
    }
    if (elt === '\t') {
      this.emit('letter', ' ', elt);
    }
  }
}
// Local Variables: 
// mode: JavaScript
// js-indent-level: 2
// End:
#endif
