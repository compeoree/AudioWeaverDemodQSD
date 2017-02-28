//-------------------------------------------------------------------------------------------
// Demonstration sketch for AudioWeaverDemodQSD Teensy Audio object
//
// This is intended as a Weaver SSB (single-sideband) demodulator in SDR (Software-
// Defined-Radio) applications that have a QSD front end.  
//
// Uses the AudioWeaverQSDSimulator object to simulate the existence of a SDR QSD receiver.
//
// D. Rowell     2/26/2017
//============================================================================================
#include <Audio.h>
#include "AudioWeaverDemodQSD.h"
#include "AudioWeaverQSDSimulator.h"
#include <Wire.h>
#include <SPI.h>
#include <SerialFlash.h>
//
float WeaverFilter_cutoff = 1500;                  // Defined in AudioWEaverDemodQSD
//
AudioSynthWaveformSine  RFsignal;        
AudioWeaverQSDSimulator SimulatedSDR;         
AudioWeaverDemodQSD     Demod;         
AudioOutputI2S          Output;
//
AudioConnection c1(RFsignal,0,     SimulatedSDR,0);
AudioConnection c2(SimulatedSDR,0, Demod,0);
AudioConnection c3(SimulatedSDR,1, Demod,1);
AudioConnection c4(Demod,0,        Output,0);    // Display output on 'scope
AudioConnection c5(RFsignal,0,     Output,1);    // Display input on 'scope
AudioControlSGTL5000 audioShield;

//----------------------------------------------------------------------------
void setup() {
  delay(2000);
  audioShield.enable();
  AudioMemory(20);
  audioShield.volume(1.0);
}

//----------------------------------------------------------------------------
void loop() {
// LSB demodulation for 5 seconds
  float CarrierFrequency = 18000.;
  float AudioFrequency   = 2000.;
  AudioNoInterrupts();
    RFsignal.frequency(CarrierFrequency - AudioFrequency);   //For LSB
    RFsignal.amplitude(1.0);
    Demod.LSB(); 
    SimulatedSDR.TuningFrequency(CarrierFrequency - WeaverFilter_cutoff);   // For Weaver LSB
  AudioInterrupts();
  delay(5000);
//
// USB Demodulation for 5 seconds
//
  CarrierFrequency = 8000.;
  AudioFrequency   = 1000.;
  AudioNoInterrupts();
    RFsignal.frequency(CarrierFrequency + AudioFrequency);   //For USB
    RFsignal.amplitude(1.0);
    Demod.USB(); 
    SimulatedSDR.TuningFrequency(CarrierFrequency + WeaverFilter_cutoff);   // For Weaver USB
  AudioInterrupts();
  delay(5000);
}


