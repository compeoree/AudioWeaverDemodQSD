//---------------------------------------------------------------------------------
// File:    AudioWeaverQSDSimulator.cpp
//          A ''Teensy'' Audio Object function for Weaver SSB demodulation
//
// Note:    This is not required for operation of AudioWeaverDemodQSD in actual
//          SDR use.   It simply generates a pair of quadrature outputs that can
//          be used for demonstration/testing.
//
// Author:  Derek Rowell
//
// Date:    Feb. 26, 2017
//
// Copyright 2017, Derek Rowell
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
#include "AudioWeaverQSDSimulator.h"
extern "C" {extern const int16_t AudioWaveformSine[257];}
//
int16_t Oscillator1(uint16_t Phase){
  uint16_t  index;                   // Index into sine table
  uint16_t  delta;                   // Remainder for interpolation,
  int16_t   val1, val2;
  index   = Phase>>8;                              // index into lookup table
  delta   = Phase&0xFF;                            // remainder 
  val1    = AudioWaveformSine[index];
  val2    = AudioWaveformSine[index+1];            // table has 257 entries
  return val1 + (((val2 - val1)*delta)>>8);  // linear interpolation
}

void AudioWeaverQSDSimulator::update(void)
{
  audio_block_t *blockIn, *blockOutQ, *blockOutI;
  blockIn = receiveReadOnly(0);
  if (!blockIn) return;
// Main loop:
  blockOutQ = allocate();                           // create new output block
  blockOutI = allocate();                           // create new output block
  if (!blockOutI || !blockOutQ) return;
//
  for (uint16_t i=0; i < AUDIO_BLOCK_SAMPLES; i++) {  
    blockOutI->data[i]  = (blockIn->data[i]*Oscillator1(sinPhase))>>15;
    blockOutQ->data[i]  = (blockIn->data[i]*Oscillator1(cosPhase))>>15;
    sinPhase += phaseInc;
    cosPhase += phaseInc;
  }
//
  transmit(blockOutI, 0);
  transmit(blockOutQ, 1);
  release(blockOutI);
  release(blockOutQ);
  release(blockIn);
  return;
}



