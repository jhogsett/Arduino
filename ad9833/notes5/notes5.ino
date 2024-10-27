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
int scale_major[8];
int scale_minor[8];

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
  scale_major[7] = 12;

  scale_minor[0] = 0;
  scale_minor[1] = 2;
  scale_minor[2] = 3;
  scale_minor[3] = 5;
  scale_minor[4] = 7;
  scale_minor[5] = 8;
  scale_minor[6] = 10;
  scale_minor[7] = 12;

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
  AD1.setMode(MD_AD9833::MODE_SQUARE2);

  AD2.setFrequency(0, SILENT_FREQ);
  AD2.setMode(MD_AD9833::MODE_SQUARE2);

  AD3.setFrequency(0, SILENT_FREQ);
  AD3.setMode(MD_AD9833::MODE_SQUARE2);

  AD4.setFrequency(0, SILENT_FREQ);
  AD4.setMode(MD_AD9833::MODE_SQUARE2);

}


void major_chord(int note, int times=1, int on=500, int off=100){
  for(int i = 0; i < times; i++){
    AD1.setFrequency(0, notes[note]);
    AD2.setFrequency(0, notes[note+4]);
    AD3.setFrequency(0, notes[note+7]);
    AD4.setFrequency(0, notes[note+12]);
    delay(on);

    AD1.setFrequency(0, SILENT_FREQ);
    AD2.setFrequency(0, SILENT_FREQ);
    AD3.setFrequency(0, SILENT_FREQ);
    AD4.setFrequency(0, SILENT_FREQ);
    delay(off);
  }
}

void minor_chord(int note, int on=500, int off=100){
  AD1.setFrequency(0, notes[note]);
  AD2.setFrequency(0, notes[note+3]);
  AD3.setFrequency(0, notes[note+7]);
  AD4.setFrequency(0, notes[note+12]);
  delay(on);

  AD1.setFrequency(0, SILENT_FREQ);
  AD2.setFrequency(0, SILENT_FREQ);
  AD3.setFrequency(0, SILENT_FREQ);
  AD4.setFrequency(0, SILENT_FREQ);
  delay(off);
}

void loop(void)
{
  major_chord(12, 1, 150, 10);
  major_chord(16, 1, 150, 10);
  major_chord(19, 1, 150, 10);
  major_chord(24, 1, 400, 10);
  major_chord(22, 1, 150, 10);
  major_chord(24, 1, 600, 1000);

}

















