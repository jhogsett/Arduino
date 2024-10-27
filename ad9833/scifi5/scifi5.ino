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

#define FREQ1 338.0
#define FREQ2 566.0
#define FREQ_OFFSET1 0.1
#define FREQ_OFFSET2 0.1
#define FREQ_STEP1 0.1
#define FREQ_STEP2 0.3
#define STEP1 1
#define STEPS1 100
#define STEP2 3
#define STEPS2 100
// #define DEL 0

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
  delay(100);
  }

void loop(void)
{
  double cf1 = 0;
  double cf2 = 0;
  int step1 = 0;
  int step2 = 0;
  int dir1 = 1;
  int dir2 = 1;
  
  while(true){
    
    step1 += (STEP1 * dir1);
    step2 += (STEP2 * dir2);
    
    if(step1 > STEPS1 || step1 < 0){
      dir1 *= -1;
    }
    if(step2 > STEPS2 || step2 < 0){
      dir2 *= -1;
    }
    
    
    // if(step1 > STEPS1 || step1 < 0){
    //   step1 -= STEPS1;
    //   dir1 *= -1;
    // }      

    // step2 += (STEP2 * dir2);
    // if(cf2 > STEPS2 || step2 < 0){
    //   cf2 -= STEPS2;
    //   dir2 *= -1;
    // }      

    cf1 = FREQ1 + (step1 * FREQ_STEP1);
    cf2 = FREQ2 + (step2 * FREQ_STEP2);

    AD1.setFrequency(0, cf1);
    AD2.setFrequency(0, cf2);
    AD3.setFrequency(0, cf1 + FREQ_OFFSET1);
    AD4.setFrequency(0, cf2 + FREQ_OFFSET2);
    
    // delay(DEL);        
  }
}
