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

#define SILENT_FREQ 100000

#define AFSKS_LOW 1070
#define AFSKS_HIGH 1270
#define AFSKA_LOW 2025
#define AFSKA_HIGH 2225

#define AFSK2_LOW AFSKA_LOW
#define AFSK2_HIGH AFSKA_HIGH
#define AFSK1_LOW AFSKS_LOW
#define AFSK1_HIGH AFSKS_HIGH

#define BAUD_RATE 110.0
#define INTER_BIT_DELAY (1000000.0 / BAUD_RATE)

void setup(void)
{
  randomizer.randomize();
    
  AD1.begin();
  AD1.setFrequency(0, SILENT_FREQ);
  AD2.begin();
  AD2.setFrequency(0, SILENT_FREQ);
  AD3.begin();
  AD3.setFrequency(0, SILENT_FREQ);
  AD4.begin();
  AD4.setFrequency(0, SILENT_FREQ);

  AD1.setMode(MD_AD9833::MODE_SINE);
  AD2.setMode(MD_AD9833::MODE_SINE);
  AD3.setMode(MD_AD9833::MODE_SINE);
  AD4.setMode(MD_AD9833::MODE_SINE);
  }

void silence(){
    AD1.setFrequency(0, SILENT_FREQ);
    AD2.setFrequency(0, SILENT_FREQ);
    AD3.setFrequency(0, SILENT_FREQ);
    AD4.setFrequency(0, SILENT_FREQ);
}

void loop(void){
  delay(1000);  

  AD2.setFrequency(0, AFSK2_LOW);
  delay(1000);  

  AD1.setFrequency(0, AFSK1_LOW);
  delay(500);  

  for(int i = 0; i < BAUD_RATE; i++){
    int r1 = random(0, 2);
    AD1.setFrequency(0, r1 ? AFSK1_LOW : AFSK1_HIGH);
    AD2.setFrequency(0, AFSK2_LOW);
    delayMicroseconds(INTER_BIT_DELAY);
  }

  // for(int i = 0; i < BAUD_RATE / 2; i++){
  //   int r1 = random(0, 2);
  //   AD1.setFrequency(0, r1 ? AFSK1_LOW : AFSK1_HIGH);
  //   AD2.setFrequency(0, AFSK2_LOW);
  //   delayMicroseconds(INTER_BIT_DELAY);
  // }

  while(1){
    int r1 = random(0, 2);
    int r2 = random(0, 2);

    AD1.setFrequency(0, r1 ? AFSK1_LOW : AFSK1_HIGH);
    AD2.setFrequency(0, r2 ? AFSK2_LOW : AFSK2_HIGH);

    delayMicroseconds(INTER_BIT_DELAY);
  }
}
