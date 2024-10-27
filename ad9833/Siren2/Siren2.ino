#include <MD_AD9833.h>
#include <SPI.h>

// Pins for SPI comm with the AD9833 IC
const uint8_t PIN_DATA = 11;  ///< SPI Data pin number
const uint8_t PIN_CLK = 13;  	///< SPI Clock pin number
const uint8_t PIN_FSYNC1 = 10; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = 9;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = 8;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC4 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)

// MD_AD9833	AD(PIN_FSYNC);  // Hardware SPI
MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
MD_AD9833	AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

#define SILENT_FREQ 1000000

void setup(void)
{
  Serial.begin(115200);

  AD1.begin();
  AD2.begin();
  AD3.begin();
  AD4.begin();

  AD1.setFrequency(0, SILENT_FREQ);
  AD1.setMode(MD_AD9833::MODE_SINE);

  AD2.setFrequency(0, SILENT_FREQ);
  AD2.setMode(MD_AD9833::MODE_SINE);

  AD3.setFrequency(0, SILENT_FREQ);
  AD3.setMode(MD_AD9833::MODE_SINE);

  AD4.setFrequency(0, SILENT_FREQ);
  AD4.setMode(MD_AD9833::MODE_SINE);

}

void loop(void)
{
  int s = 500;
  int e = 1500;

  int t1 = 21;
  int d1 = 1;

  int t2 = 4;
  int d2a = 3;
  int d2b = 150;
  int d2c = 400;

  int times1 = 19;
  int times2 = 3;

  for(int j = 0; j < times1; j++){
    for(int i = s; i <= e; i += t1){
      AD1.setFrequency(0, i);
      AD2.setFrequency(0, i + 69);

      // AD2.setFrequency(0, i);
      // AD2.setPhase(0, 900);
      delay(d1);
    }
    for(int i = e; i >= s; i -= t1){
      AD1.setFrequency(0, i);
      AD2.setFrequency(0, i + 69);
      // AD2.setFrequency(0, i);
      // AD2.setPhase(0, 900);
      delay(d1);
    }
  }
  
  AD1.setFrequency(0, s);
  delay(d2b);
  
  for(int j = 0; j < times2; j++){

    for(int i = s; i <= e; i += t2){
      AD1.setFrequency(0, i);
      AD2.setFrequency(0, i + 69);
      // AD2.setFrequency(0, i);
      // AD2.setPhase(0, 900);
      delay(d2a);
    }

    delay(d2c);

    for(int i = e; i >= s; i -= t2){
      AD1.setFrequency(0, i);
      AD2.setFrequency(0, i + 69);
      // AD2.setFrequency(0, i);
      // AD2.setPhase(0, 900);
      delay(d2a);
    }

    delay(d2b);
  }
}

