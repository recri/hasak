/* -*- mode: c++; tab-width: 8 -*- */
/*
  Copyright (C) 2021 by Roger E Critchlow Jr, Charlestown, MA, USA.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
*/
#ifndef MORSE_KEYER_H
#define MORSE_KEYER_H

/*
** A morse code keyer base class.
** Handles timing computations for keyers called at sample rate.
** Specifies the clock method
**
** Specify sample rate rather than micros or millis per tick.
** End point is the number of samples per feature.
** Entry points to specify the speed, the sample rate, the weights,
** and all at once.
*/

class morse_keyer {
public:
  static const int wordDits = 50;	// number of dits in a work, PARIS, use 60 for CODEX
  
  // ticks per feature, used to clock keyers
  int _ticksPerDit, _ticksPerDah, _ticksPerIes, _ticksPerIls, _ticksPerIws;
  // input parameters for the computation
  float _sampleRate, _wpm;
  int _weight, _ratio, _compensation, _farnsworth;

  // initialize a keyer
  morse_keyer() { setAll(48000, 20, 50, 50, 0, 0); }

  // set the timing
  void setAll(float sampleRate, float wpm, int weight, int ratio, int compensation, int farnsworth) {
    _sampleRate = sampleRate;
    _wpm = wpm;
    _weight = weight;
    _ratio = ratio;
    _compensation = compensation;
    _farnsworth = farnsworth;
    float ms_per_dit = (1000 * 60) / (wpm * wordDits);
    float r = (ratio-50)/100.0;
    float w = (weight-50)/100.0;
    float c = compensation / ms_per_dit; /* ms / ms_per_dit */
    //Serial.printf("morse_keyer sr %f, word %d, wpm %f, weight %d, ratio %d, comp %d, farns %d\n", sampleRate, wordDits, wpm, weight, ratio, compensation, farnsworth);
    //Serial.printf("morse_keyer r %f, w %f, c %f\n", r, w, c);
    // samples_per_dit = (samples_per_second * second_per_minute) / (words_per_minute * dits_per_word) 
    unsigned ticksPerBaseDit = ((sampleRate * 60) / (wpm * wordDits));
    _ticksPerDit = (1+r+w+c) * ticksPerBaseDit;
    _ticksPerDah = (3-r+w+c) * ticksPerBaseDit;
    _ticksPerIes = (1  -w-c) * ticksPerBaseDit;
    _ticksPerIls = (3  -w-c) * ticksPerBaseDit;
    _ticksPerIws = (7  -w-c) * ticksPerBaseDit;
    
    //
    // Farnsworth timing: stretch inter-character and inter-word pauses
    // to reduce wpm to the specified farnsworth speed.
    // formula from https://morsecode.world/international/timing.html
    //
    if (farnsworth > 5 && farnsworth < wpm) {
      float f = 50.0/19.0 * wpm / farnsworth - 31.0/19.0;
      _ticksPerIls *= f;		// stretched inter-character pause
      _ticksPerIws *= f;		// stretched inter-word pause
    }
    //Serial.printf("morse_keyer base dit %u, dit %u, dah %u, ies %u, ils %u, iws %u\n", ticksPerBaseDit, _ticksPerDit, _ticksPerDah, _ticksPerIes, _ticksPerIls, _ticksPerIws);
  }

  void setWeights(int weight, int ratio, int compensation, int farnsworth) { 
    setAll(_sampleRate, _wpm, weight, ratio, compensation, farnsworth);
  }
  // the sample rate in samples/second
  void setSampleRate(float sampleRate) {
    setAll(sampleRate, _wpm, _weight, _ratio, _compensation, _farnsworth);
  }

  // the speed in words/minute
  void setSpeed(float wpm) {
    setAll(_sampleRate, wpm, _weight, _ratio, _compensation, _farnsworth);
  }

  // the weight between marks and spaces, 50 is half and half
  void setWeight(int weight) {
    setAll(_sampleRate, _wpm, weight, _ratio, _compensation, _farnsworth);
  }

  // the ratio between dits and dahs, 50 is the normal 1 to 3 ratio
  void setRatio(int ratio) {
    setAll(_sampleRate, _wpm, _weight, ratio, _compensation, _farnsworth);
  }

  // compensation in milliseconds per dit
  void setCompensation(int compensation) {
    setAll(_sampleRate, _wpm, _weight, _ratio, compensation, _farnsworth);
  }

  // farnsworth adjustment to interletter and interword spacing
  void setFarnsworth(int farnsworth) {
    setAll(_sampleRate, _wpm, _weight, _ratio, _compensation, farnsworth);
  }

  // the signature for the clock function which is called once per sample to compute the output sample
  int clock(int dit, int dah) { return dit|dah; }
};

#endif
