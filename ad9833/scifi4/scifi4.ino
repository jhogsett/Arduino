#include <MD_AD9833.h>
#include <SPI.h>

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

#define SILENT_FREQ 250000

void setup(void)
{
  // Serial.begin(115200);
  AD1.begin();
  AD1.setFrequency(0, SILENT_FREQ);
  AD2.begin(); 
  AD2.setFrequency(0, SILENT_FREQ);
  AD3.begin();
  AD3.setFrequency(0, SILENT_FREQ);
  AD4.begin();
  AD4.setFrequency(0, SILENT_FREQ);
  }

void loop(void)
{
  AD1.setFrequency(0, 367.0);
  AD2.setFrequency(0, 367.5);
  AD3.setFrequency(0, 557.2);
  AD4.setFrequency(0, 558.3);

  while(true);
}





