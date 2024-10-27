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

#define FREQ_LOW 350
#define FREQ_HI 1050
#define MODE1 (MD_AD9833::MODE_SQUARE2)
#define MODE2 (MD_AD9833::MODE_SQUARE1)
#define MODE3 (MD_AD9833::MODE_SINE)
#define MODE4 (MD_AD9833::MODE_SINE)
#define PHASE1 0
#define PHASE2 1800
#define PHASE3 1800
#define PHASE4 1800
void setup(void)
{
  // Serial.begin(115200);
  AD1.begin();
  AD1.setFrequency(0, SILENT_FREQ);
  AD1.setPhase(0, PHASE1);
  AD1.setMode(MODE1);
  AD2.begin(); 
  AD2.setFrequency(0, SILENT_FREQ);
  AD2.setPhase(0, PHASE2);
  AD2.setMode(MODE2);
  AD3.begin();
  AD3.setFrequency(0, SILENT_FREQ);
  AD3.setPhase(0, PHASE3);
  AD3.setMode(MODE3);
  AD4.begin();
  AD4.setFrequency(0, SILENT_FREQ);
  AD4.setPhase(0, PHASE4);
  AD4.setMode(MODE4);
  delay(100);
  }

void loop(void)
{
  while(true){
    float f = random(FREQ_LOW, FREQ_HI) * 1.0;

    AD1.setFrequency(0, f);
    AD1.setPhase(0, PHASE1);
    AD2.setFrequency(0, f);
    AD2.setPhase(0, PHASE2);
  // AD3.setFrequency(0, f);
    // AD4.setFrequency(0, f);
    // AD2.setFrequency(0, f * 0.5);
    // AD3.setFrequency(0, f * 0.25);
    // AD4.setFrequency(0, f * 0.125);

    delay(150 + int(f / 10.0));
  }
}
