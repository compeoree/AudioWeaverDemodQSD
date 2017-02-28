//---------------------------------------------------------------------------------
// File:    AudioWeaverDemod.cpp
//          A ''Teensy'' Audio Object function for Weaver SSB demodulation
//
// Author:  Derek Rowell
//
// Date:    Feb. 19, 2017
//
// Copyright (c) 2017, Derek Rowell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
// 1) The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 2) THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHOR BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//---------------------------------------------------------------------------------
#include "AudioWeaverDemodQSD.h"
extern "C" {extern const int16_t AudioWaveformSine[257];}

//-------------------------------------------------
int16_t Oscillator(uint16_t Phase){
  uint16_t  index;                   // Index into sine table
  uint16_t  delta;                   // Remainder for interpolation,
  int16_t   val1, val2;
  index   = Phase>>8;                              // index into lookup table
  delta   = Phase&0xFF;                            // remainder 
  val1    = AudioWaveformSine[index];
  val2    = AudioWaveformSine[index+1];            // table has 257 entries
  return val1 + (((val2 - val1)*delta)>>8);  // linear interpolation
}

//---------------------------------------------------
void AudioWeaverDemodQSD::update(void){
  int16_t  oscOutI, oscOutQ;
  int16_t  TempI[AUDIO_BLOCK_SAMPLES], TempQ[AUDIO_BLOCK_SAMPLES];
  audio_block_t *blockI, *blockQ, *blockOut;
  blockI = receiveReadOnly(0);
  blockQ = receiveReadOnly(1);
  if (!blockI) {
    if (blockQ) release(blockQ);
    return;
  }
  blockOut = allocate();                           // create new output block
  if (blockOut){
//--- Low-pass filter both blocks
  arm_fir_fast_q15(&filterI, blockI->data, TempI, AUDIO_BLOCK_SAMPLES);
  arm_fir_fast_q15(&filterQ, blockQ->data, TempQ, AUDIO_BLOCK_SAMPLES);
//   
//-- Multiplication stage
  for (uint16_t i=0; i < AUDIO_BLOCK_SAMPLES; i++) { 
    oscOutI = Oscillator(sinPhase);
    oscOutQ = Oscillator(cosPhase);
//
//--- Sum or difference to produce the output.
    if (lowerSB) blockOut->data[i] = ((TempI[i]*oscOutI)>>15) - ((TempQ[i]*oscOutQ)>>15);
    else         blockOut->data[i] = ((TempI[i]*oscOutI)>>15) + ((TempQ[i]*oscOutQ)>>15);
    sinPhase += phaseInc;
    cosPhase += phaseInc;
    }
//--- End of processing...
    transmit(blockOut);
    release(blockOut);
  }
  release(blockI);
  release(blockQ);
	return;
}



