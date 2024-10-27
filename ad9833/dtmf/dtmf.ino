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

// DTMF keypad frequencies (with sound clips)[10]
// 1209 Hz	1336 Hz	1477 Hz	1633 Hz
// 697 Hz	1ⓘ	2ⓘ	3ⓘ	Aⓘ
// 770 Hz	4ⓘ	5ⓘ	6ⓘ	Bⓘ
// 852 Hz	7ⓘ	8ⓘ	9ⓘ	Cⓘ
// 941 Hz	*ⓘ	0ⓘ	#ⓘ	Dⓘ

#define ROW1 697.0
#define ROW2 770.0
#define ROW3 852.0
#define ROW4 941.0
#define COL1 1209.0
#define COL2 1336.0
#define COL3 1477.0
#define COL4 1633.0

int rows[4];
int cols[4];

int mapr[16];
int mapc[16];

void setup(void)
{
  Serial.begin(115200);

  rows[0] = ROW1;
  rows[1] = ROW2;
  rows[2] = ROW3;
  rows[3] = ROW4;

  cols[0] = COL1;
  cols[1] = COL2;
  cols[2] = COL3;
  cols[3] = COL4;

  mapr[ 0] = 3;
  mapr[ 1] = 0;
  mapr[ 2] = 0;
  mapr[ 3] = 0;
  mapr[ 4] = 1;
  mapr[ 5] = 1;
  mapr[ 6] = 1;
  mapr[ 7] = 2;
  mapr[ 8] = 2;
  mapr[ 9] = 2;
  mapr[10] = 3;
  mapr[11] = 3;
  mapr[12] = 0;
  mapr[13] = 1;
  mapr[14] = 2;
  mapr[15] = 3;

  mapc[ 0] = 1;
  mapc[ 1] = 0;
  mapc[ 2] = 1;
  mapc[ 3] = 2;
  mapc[ 4] = 0;
  mapc[ 5] = 1;
  mapc[ 6] = 2;
  mapc[ 7] = 0;
  mapc[ 8] = 1;
  mapc[ 9] = 2;
  mapc[10] = 0;
  mapc[11] = 2;
  mapc[12] = 3;
  mapc[13] = 3;
  mapc[14] = 3;
  mapc[15] = 3;

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
  for(int i = 0; i < 4; i++)
    for(int j = 0; j < 3; j++){

      AD1.setFrequency(0, rows[i]);
      AD2.setFrequency(0, cols[j]);

      delay(250);

      AD1.setFrequency(0, SILENT_FREQ);
      AD2.setFrequency(0, SILENT_FREQ);
      delay(125);
    }

  for(int i = 0; i < 4; i++)
    for(int j = 3; j < 4; j++){

      AD1.setFrequency(0, rows[i]);
      AD2.setFrequency(0, cols[j]);

      delay(250);

      AD1.setFrequency(0, SILENT_FREQ);
      AD2.setFrequency(0, SILENT_FREQ);
      delay(125);
    }

}


//   int j;
//   for(int i = 0; i < 16; i++){

//     if(i == 0){
//       j = 9;
//     } else if(i == 9){
//       j = 0;
//     } else {
//       j = i;
//     }

//     AD1.setFrequency(0, rows[mapr[j]]);
//     AD2.setFrequency(0, cols[mapc[j]]);

//     delay(250);

//     AD1.setFrequency(0, SILENT_FREQ);
//     AD2.setFrequency(0, SILENT_FREQ);
//     delay(500);
//   }

// }






