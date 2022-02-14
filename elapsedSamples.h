/* Elapsed time types - for easy-to-use measurements of elapsed time
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// class to return the number of elapsed samples
// shamelessly ripped off from Paul Stoffregen's
// elapsedMillis and elapsedMicros classes

class elapsedSamples
{
private:
	unsigned long s;
public:
	elapsedSamples(void) { s = samples(); }
	elapsedSamples(unsigned long val) { s = samples() - val; }
	elapsedSamples(const elapsedSamples &orig) { s = orig.s; }
	operator unsigned long () const { return samples() - s; }
	elapsedSamples & operator = (const elapsedSamples &rhs) { s = rhs.s; return *this; }
	elapsedSamples & operator = (unsigned long val) { s = samples() - val; return *this; }
	elapsedSamples & operator -= (unsigned long val)      { s += val ; return *this; }
	elapsedSamples & operator += (unsigned long val)      { s -= val ; return *this; }
	elapsedSamples operator - (int val) const           { elapsedSamples r(*this); r.s += val; return r; }
	elapsedSamples operator - (unsigned int val) const  { elapsedSamples r(*this); r.s += val; return r; }
	elapsedSamples operator - (long val) const          { elapsedSamples r(*this); r.s += val; return r; }
	elapsedSamples operator - (unsigned long val) const { elapsedSamples r(*this); r.s += val; return r; }
	elapsedSamples operator + (int val) const           { elapsedSamples r(*this); r.s -= val; return r; }
	elapsedSamples operator + (unsigned int val) const  { elapsedSamples r(*this); r.s -= val; return r; }
	elapsedSamples operator + (long val) const          { elapsedSamples r(*this); r.s -= val; return r; }
	elapsedSamples operator + (unsigned long val) const { elapsedSamples r(*this); r.s -= val; return r; }
};
