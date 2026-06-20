#include <SPI.h>
#include <MD_AD9833.h>
#include "types.h"

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC1 = 8; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = 6;  ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins

#define SILENT_FREQ 1000000

bool abort_sequence(uint32_t data){
  if(Serial.available()){
    while(Serial.available() && Serial.read());
    return true;
  }
  return false;
}


void sound_off(){
  AD1.setFrequency(0, SILENT_FREQ);
  AD2.setFrequency(0, SILENT_FREQ);
}

void busy_on(){
  AD1.setFrequency(0, 480.0);
  AD2.setFrequency(0, 620.0);
}

void ring_on(){
  AD1.setFrequency(0, 480.0);
  AD2.setFrequency(0, 440.0);
}

void error_tone1_on(){
  AD1.setFrequency(0, 913.8);
}

void error_tone2_on(){
  AD1.setFrequency(0, 1428.5);
}

void error_tone3_on(){
  AD1.setFrequency(0, 1776.7);
}

NonBlockingAction ring_actions[2] = { ring_on, sound_off};
int ring_times[2] = { 2000, 4000 };
NonBlockingSequence ring_sequence(ring_actions, ring_times, 2, abort_sequence);

NonBlockingAction busy_actions[2] = { busy_on, sound_off};
int busy_times[2] = { 500, 500 };
NonBlockingSequence busy_sequence(busy_actions, busy_times, 2, abort_sequence);

NonBlockingAction reorder_actions[2] = { busy_on, sound_off};
int reorder_times[2] = { 250, 250 };
NonBlockingSequence reorder_sequence(reorder_actions, reorder_times, 2, abort_sequence);

NonBlockingAction error_actions[4] = { error_tone1_on, error_tone2_on, error_tone3_on, sound_off};
int error_times[4] = { 380, 276, 380, 0 };
NonBlockingSequence error_sequence(error_actions, error_times, 4, abort_sequence);

void setup() {
  Serial.begin(115200);

  AD1.begin();
  AD2.begin();
  AD3.begin();

  AD1.setFrequency(0, SILENT_FREQ);
  AD1.setMode(MD_AD9833::MODE_SINE);

  AD2.setFrequency(0, SILENT_FREQ);
  AD2.setMode(MD_AD9833::MODE_SINE);

  AD3.setFrequency(0, SILENT_FREQ);
  AD3.setMode(MD_AD9833::MODE_SINE);

  // sequence.start(5, NULL);
  ring_sequence.start(4);
  busy_sequence.start(4);
  reorder_sequence.start(8);
  error_sequence.start(1);
}

void loop() {
  while(ring_sequence.step());
  if(ring_sequence.aborted()){
    sound_off();
  }
  ring_sequence.restart();

  while(busy_sequence.step());
  if(busy_sequence.aborted()){
    sound_off();
  }
  busy_sequence.restart();

  while(reorder_sequence.step());
  if(reorder_sequence.aborted()){
    sound_off();
  }
  reorder_sequence.restart();

  while(error_sequence.step());
  if(error_sequence.aborted()){
    sound_off();
  }
  error_sequence.restart();

  delay(2000);
}
