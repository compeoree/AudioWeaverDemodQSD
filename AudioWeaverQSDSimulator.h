//---------------------------------------------------------------------------------
// File:    AudioWeaverQSDSimulator.h
//          Helper file for ''Teensy'' Audio Object AudioWeaverDemod for Weaver
//          SSB demodulation.
//
// Note:    This is not required for operation of AudioWeaverDemodQSD in actual
//          SDR use.   IT simply generates a pair of quadrature outputs that can
//          be used for demonstration/testing.
//
// Author:  Derek Rowell
//
// Date:    Feb. 26, 2017
//
// Copyright (c) 2017, Derek Rowell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//  1) The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 2) THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------
//
#ifndef weaver_qsd_in_h_
#define weaver_qsd_in_h_
#include "Arduino.h"
#include "AudioStream.h"

class AudioWeaverQSDSimulator : public AudioStream {
  public:
    AudioWeaverQSDSimulator() : AudioStream(1,inputQueueArray) {
      sinPhase = 0;            // Initial phase for oscillators
      cosPhase = 16384;        // Corresponds to phase shift of pi/2 for cosine
    }
//
    virtual void update(void);
    
//--- Set internal oscillator frequency
    void TuningFrequency(float oscFreq) {
      if (oscFreq < 0.0) oscFreq = 0.0;
      else if (oscFreq > AUDIO_SAMPLE_RATE_EXACT/2) oscFreq = AUDIO_SAMPLE_RATE_EXACT/2;
      phaseInc = int16_t(oscFreq*(65536.0/AUDIO_SAMPLE_RATE_EXACT)); 
    }
//
  private:
    audio_block_t *inputQueueArray[1];
    uint16_t sinPhase;
    uint16_t cosPhase;
    uint16_t phaseInc;
};
#endif
