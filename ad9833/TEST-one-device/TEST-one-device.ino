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
const uint8_t PIN_FSYNC = 8; ///< SPI Load pin number (FSYNC in AD9833 usage)

// MD_AD9833	AD(PIN_FSYNC);  // Hardware SPI
MD_AD9833	AD(PIN_DATA, PIN_CLK, PIN_FSYNC); // Arbitrary SPI pins

void setup(void)
{
  AD.begin();
}

void loop(void)
{
  AD.setFrequency(0, 700.0);
  AD.setMode(MD_AD9833::MODE_SINE);


  while(1);
}
