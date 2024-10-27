// Basic MD_AD9833 test file
//
// Initialises the device to default conditions
// 
// Connect a pot to A0 to change the frequency by turning the pot
//
#include <MD_AD9833.h>
#include <SPI.h>

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC = 10; ///< SPI Load pin number (FSYNC in AD9833 usage)

// MD_AD9833	AD(PIN_FSYNC);  // Hardware SPI
MD_AD9833	AD(PIN_DATA, PIN_CLK, PIN_FSYNC); // Arbitrary SPI pins

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

void setup(void)
{
  AD.begin();
}

void loop(void)
{
  AD.setFrequency(0, 350.0);
  AD.setMode(MD_AD9833::MODE_SINE);

  int s = 550;
  int e = 1500;
  int t1 = 13;
  int d1 = 1;
  int t2 = 2;
  int d2 = 4;
  int times1 = 13;
  int times2 = 3;

  for(int j = 0; j < times1; j++){
    for(int i = s; i <= e; i += t1){
      AD.setFrequency(0, i);
      delay(d1);
    }
    for(int i = e; i >= s; i -= t1){
      AD.setFrequency(0, i);
      delay(d1);
    }
  }
  for(int j = 0; j < times2; j++){
    for(int i = s; i <= e; i += t2){
      AD.setFrequency(0, i);
      delay(d2);
    }
    for(int i = e; i >= s; i -= t2){
      AD.setFrequency(0, i);
      delay(d2);
    }
  }
}
