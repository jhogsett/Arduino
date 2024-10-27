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
const uint8_t PIN_FSYNC1 = 10; ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC2 = 9;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC3 = 8;  ///< SPI Load pin number (FSYNC in AD9833 usage)
const uint8_t PIN_FSYNC4 = 7;  ///< SPI Load pin number (FSYNC in AD9833 usage)

// MD_AD9833	AD(PIN_FSYNC);  // Hardware SPI
MD_AD9833	AD1(PIN_DATA, PIN_CLK, PIN_FSYNC1); // Arbitrary SPI pins
MD_AD9833	AD2(PIN_DATA, PIN_CLK, PIN_FSYNC2); // Arbitrary SPI pins
MD_AD9833	AD3(PIN_DATA, PIN_CLK, PIN_FSYNC3); // Arbitrary SPI pins
MD_AD9833	AD4(PIN_DATA, PIN_CLK, PIN_FSYNC4); // Arbitrary SPI pins

// https://github.com/RodrigoDornelles/arduino-tone-pitch
#define STARTING_NOTE 261.63 // C4
#define NOTE_COUNT (5 * 12)
#define SILENT_FREQ 50000

double notes[NOTE_COUNT];
int scale_major[7];
int scale_minor[7];

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

void setup(void)
{
  Serial.begin(115200);

// https://www.reddit.com/r/musictheory/comments/mvhlsk/octave_8_or_12/
// For example C D E F G A B C is a C major scale it has 7 unique notes and the 8th note is the same as a first.
// All 12 notes written using sharps are
// C C# D D# E F F# G G# A A# B C

// The Minor Scales
// musictheory.net
// https://www.musictheory.net › lessons
// C natural minor is: C, D, Eb, F, G, Ab, Bb.

  scale_major[0] = 0;
  scale_major[1] = 2;
  scale_major[2] = 4;
  scale_major[3] = 5;
  scale_major[4] = 7;
  scale_major[5] = 9;
  scale_major[6] = 11;

  scale_minor[0] = 0;
  scale_minor[1] = 2;
  scale_minor[2] = 3;
  scale_minor[3] = 5;
  scale_minor[4] = 7;
  scale_minor[5] = 8;
  scale_minor[6] = 10;

  // >>> a=[(starting * (2**(1/12))**i) for i in range(0, 9*12)]
  // double note_power = pow(2.0, 1.0/12.0);
  // Serial.println(note_power);
  for(int i = 0; i < NOTE_COUNT; i++){
    // Serial.println(pow(note_power, i));
    notes[i] = STARTING_NOTE * pow(pow(2.0, 1.0/12.0), i);
    Serial.println(notes[i]);
  }

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

/*
  AD.setMode(MD_AD9833::MODE_TRIANGLE);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now low
  
  AD.setMode(MD_AD9833::MODE_SINE);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now low

  AD.setMode(MD_AD9833::MODE_SQUARE1);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now high

  AD.setMode(MD_AD9833::MODE_SQUARE2);
  delay(2000);
  AD.setMode(MD_AD9833::MODE_OFF);
  delay(2000);
  //output is now low
*/
}

void loop(void)
{
  // major
  for(int i = 5; i < NOTE_COUNT-24; i++){
    Serial.println(notes[i]);
    AD1.setFrequency(0, notes[i]);
    AD2.setFrequency(0, notes[i+4]);
    AD3.setFrequency(0, notes[i+7]);
    AD4.setFrequency(0, notes[i+12]);
    delay(200);
  }

  // minor
  for(int i = NOTE_COUNT-23; i >= 5 ; i--){
    Serial.println(notes[i]);
    AD1.setFrequency(0, notes[i]);
    AD2.setFrequency(0, notes[i+3]);
    AD3.setFrequency(0, notes[i+7]);
    AD4.setFrequency(0, notes[i+12]);
    delay(200);
  }

  // AD1.setFrequency(0, 1400.0);
  // AD2.setFrequency(0, 2060.0);
  // AD3.setFrequency(0, 2450.0);
  // AD4.setFrequency(0, 2600.0);
  // delay(100);

  // AD1.setFrequency(0, 140000.0);
  // AD2.setFrequency(0, 206000.0);
  // AD3.setFrequency(0, 245000.0);
  // AD4.setFrequency(0, 260000.0);
  // delay(100);






  // the use of off mode creates a click
  // AD1.setMode(MD_AD9833::MODE_SINE);
  // AD2.setMode(MD_AD9833::MODE_SINE);
  // AD3.setMode(MD_AD9833::MODE_SINE);
  // AD4.setMode(MD_AD9833::MODE_SINE);
  // delay(100);

  // AD1.setMode(MD_AD9833::MODE_OFF);
  // AD2.setMode(MD_AD9833::MODE_OFF);
  // AD3.setMode(MD_AD9833::MODE_OFF);
  // AD4.setMode(MD_AD9833::MODE_OFF);
  // delay(100);

  // while(true){
  // AD1.setFrequency(0, 1400.0);
  // AD1.setMode(MD_AD9833::MODE_SINE);
  // }
  // AD2.setFrequency(0, 2060.0);
  // AD2.setMode(MD_AD9833::MODE_SINE);

  // AD3.setFrequency(0, 2450.0);
  // AD3.setMode(MD_AD9833::MODE_SINE);

  // AD4.setFrequency(0, 2600.0);
  // AD4.setMode(MD_AD9833::MODE_SINE);

  // delay(1000);

  // delay(1);

  // AD1.setMode(MD_AD9833::MODE_OFF);
  // AD2.setMode(MD_AD9833::MODE_OFF);
  // AD3.setMode(MD_AD9833::MODE_OFF);
  // AD4.setMode(MD_AD9833::MODE_OFF);

  // delay(1);

  // int s = 550;
  // int e = 1500;
  // int t1 = 13;
  // int d1 = 1;
  // int t2 = 2;
  // int d2 = 4;
  // int times1 = 15;
  // int times2 = 5;

  // for(int j = 0; j < times1; j++){
  //   for(int i = s; i <= e; i += t1){
  //     AD1.setFrequency(0, i);
  //     AD2.setFrequency(0, i + 60);
  //     delay(d1);
  //   }
  //   for(int i = e; i >= s; i -= t1){
  //     AD1.setFrequency(0, i);
  //     AD2.setFrequency(0, i + 60);
  //     delay(d1);
  //   }
  // }
  // for(int j = 0; j < times2; j++){
  //   for(int i = s; i <= e; i += t2){
  //     AD1.setFrequency(0, i);
  //     AD2.setFrequency(0, i + 60);
  //     delay(d2);
  //   }
  //   for(int i = e; i >= s; i -= t2){
  //     AD1.setFrequency(0, i);
  //     AD2.setFrequency(0, i + 60);
  //     delay(d2);
  //   }
  // }

  // delay(2000);
  // AD.setMode(MD_AD9833::MODE_OFF);


  // static bool bOff = false;
  // static uint8_t m = 0;
  // static MD_AD9833::mode_t modes[] =
  // {
  //   MD_AD9833::MODE_TRIANGLE,
  //   MD_AD9833::MODE_SQUARE2,
  //   MD_AD9833::MODE_SINE,
  //   MD_AD9833::MODE_SQUARE1
  // };

  // if (bOff)
  //   AD.setMode(MD_AD9833::MODE_OFF);
  // else
  // {
  //   AD.setMode(modes[m]);
  //   m++;
  //   if (m == ARRAY_SIZE(modes)) m = 0;
  // }
  // bOff = !bOff;
  // delay(2000);
}
