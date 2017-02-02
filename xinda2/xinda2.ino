#include <PololuLedStrip.h>
#include "IRremote.h"

int receiver = 11; // pin 1 of IR receiver to Arduino digital pin 11

IRrecv irrecv(receiver);           // create instance of 'irrecv'
decode_results results;            // create instance of 'decode_results'

PololuLedStrip<12> ledStrip;

#define LED_COUNT 100
#define MAX_LED (LED_COUNT)

rgb_color colors[LED_COUNT];

#define RESOLUTION 1000000.0

#define MIN_SPEED 0.05
#define MAX_SPEED 0.10
#define SPEED MIN_SPEED

#include "colors.h"

void setup()
{
  Serial.begin(9600);
//  Serial.println("YourDuino IR Receiver Button Decode Test");
//  Serial.println("Questions: terry@yourduino.com");  

  //irrecv.enableIRIn(); // Start the receiver
}

#define DELAY_STEP 100

void loop()
{
  int ncolor = 0;
  rgb_color color;
  color = palette[ncolor];

  rgb_color blank;
  blank.red = 0;
  blank.green = 0;
  blank.blue = 0;

  int ndelay = 0;

  int last = 0;

  while(true){
    delay(ndelay);
    
    for(int j = 0; j < MAX_LED; j++)
    {
      colors[last] = blank;
      colors[j] = color;
      last = j;

      ledStrip.write(colors, LED_COUNT);  
    }

    irrecv.enableIRIn(); // Start the receiver
    delay(300);
    
    if (irrecv.decode(&results)) // have we received an IR signal?
    {
      char command = translateIR(); 
  
      Serial.println(results.value, HEX);
      Serial.println(command);

      //irrecv.resume(); // receive the next value

      switch(command)
      {
        case 'F':
          ndelay += DELAY_STEP;
          break;

        case 'B':
          ndelay -= DELAY_STEP;
          if(ndelay < 0) ndelay = 0;
          break;

        case 'L':
          ncolor--;
          if(ncolor < 0) ncolor = NPALETTE-1;
          color = palette[ncolor];
          break;

        case 'R':
          ncolor = (ncolor + 1) % NPALETTE;
          color = palette[ncolor];
          break;
      }

      irrecv.resume(); // receive the next value
    }  
  }
}

char translateIR()
{
  char result;
  switch(results.value)
  {
    case 0xFF629D:   result = 'F'; break; // forward
    case 0xFF22DD:   result = 'L'; break; // left
    case 0xFF02FD:   result = 'K'; break; // ok
    case 0xFFC23D:   result = 'R'; break; // right
    case 0xFFA857:   result = 'B'; break; // back
    case 0xFF6897:   result = '1'; break; // 1
    case 0xFF9867:   result = '2'; break; // 2
    case 0xFFB04F:   result = '3'; break; // 3
    case 0xFF30CF:   result = '4'; break; // 4
    case 0xFF18E7:   result = '5'; break; // 5
    case 0xFF7A85:   result = '6'; break; // 6
    case 0xFF10EF:   result = '7'; break; // 7
    case 0xFF38C7:   result = '8'; break; // 8
    case 0xFF5AA5:   result = '9'; break; // 9
    case 0xFF42BD:   result = '*'; break; // *
    case 0xFF4AB5:   result = '0'; break; // 0
    case 0xFF52AD:   result = '#'; break; // #
    case 0xFFFFFFFF: result = '&'; break; // repeat
    default:         result = '?'; break; // unknown
  }

//  delay(500); // Do not get immediate repeat

  return result;
}

