#include <MD_AD9833.h>
#include <SPI.h>
#include <random_seed.h>

#define RANDOM_SEED_PIN A1            // floating pin for seeding the RNG

// for generating higher-quality random number seeds
static RandomSeed<RANDOM_SEED_PIN> randomizer;

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC1 = 10; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = 9;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = 8;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC4 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)

MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
MD_AD9833	AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

#define SILENT_FREQ 0

void setup(void)
{
  randomizer.randomize();
    
  AD1.begin();
  AD1.setMode(MD_AD9833::MODE_SINE);
  AD1.setFrequency(0, SILENT_FREQ);

  AD2.begin();
  AD2.setMode(MD_AD9833::MODE_SINE);
  AD2.setFrequency(0, SILENT_FREQ);

  AD3.begin();
  AD3.setMode(MD_AD9833::MODE_SINE);
  AD3.setFrequency(0, SILENT_FREQ);

  // AD4.begin();
  // AD4.setFrequency(0, SILENT_FREQ);
  // AD4.setMode(MD_AD9833::MODE_SINE);
}

void silence(){
  AD1.setFrequency(0, SILENT_FREQ);
  AD2.setFrequency(0, SILENT_FREQ);
  AD3.setFrequency(0, SILENT_FREQ);
  // AD4.setFrequency(0, SILENT_FREQ);
}

void loop(void){
  AD1.setMode(MD_AD9833::MODE_SINE);
  AD2.setMode(MD_AD9833::MODE_SINE);
  AD3.setMode(MD_AD9833::MODE_SINE);

  AD1.setFrequency(0, 30.0);
  AD2.setFrequency(0, 32.0);
  AD3.setFrequency(0, 2.0);

  // AD3.setFrequency(0, 1000.2);
  // AD4.setFrequency(0, 1000.3);

  while(true);
}
