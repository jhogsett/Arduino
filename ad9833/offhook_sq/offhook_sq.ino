#include <MD_AD9833.h>
#include <SPI.h>

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC1 = 8; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = A0;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = A1;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC4 = A2;  ///< SPI Load pin number (FSYNC in AD9833 usage)

// MD_AD9833	AD(PIN_FSYNC);  // Hardware SPI
MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
MD_AD9833	AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

#define SILENT_FREQ 1000000

void setup(void)
{
  AD1.begin();
  AD2.begin();
  AD3.begin();
  AD4.begin();

  AD1.setFrequency(0, 1400.0);
  AD1.setMode(MD_AD9833::MODE_SQUARE1);

  AD2.setFrequency(0, 2060.0);
  AD2.setMode(MD_AD9833::MODE_SQUARE1);

  AD3.setFrequency(0, 2450.0);
  AD3.setMode(MD_AD9833::MODE_SQUARE1);

  AD4.setFrequency(0, 2600.0);
  AD4.setMode(MD_AD9833::MODE_SQUARE1);
}

void loop(void)
{
  AD1.setFrequency(0, 1400.0);
  AD2.setFrequency(0, 2060.0);
  AD3.setFrequency(0, 2450.0);
  AD4.setFrequency(0, 2600.0);
  delay(100);

  AD1.setFrequency(0, SILENT_FREQ);
  AD2.setFrequency(0, SILENT_FREQ);
  AD3.setFrequency(0, SILENT_FREQ);
  AD4.setFrequency(0, SILENT_FREQ);

  delay(100);
}
