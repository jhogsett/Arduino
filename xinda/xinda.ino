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
//  Serial.begin(9600);
//  Serial.println("YourDuino IR Receiver Button Decode Test");
//  Serial.println("Questions: terry@yourduino.com");  

  irrecv.enableIRIn(); // Start the receiver
}


void loop()
{
  int ncolor = 0;
  rgb_color color;
  color = palette[ncolor];

  rgb_color blank;
  blank.red = 0;
  blank.green = 0;
  blank.blue = 0;

  int last = 0;

  while(true){
    for(int j = 0; j < MAX_LED; j++)
    {
      colors[last] = blank;
      colors[j] = color;
      last = j;
      
      ledStrip.write(colors, LED_COUNT);  
    }

    if (irrecv.decode(&results)) // have we received an IR signal?
    {
  //    Serial.println(results.value, HEX);  UN Comment to see raw values
  //    translateIR(); 
      irrecv.resume(); // receive the next value
  
      ncolor = (ncolor + 1) % NPALETTE;
      color = palette[ncolor];
    }  
  }
}

void translateIR() // takes action based on IR code received
{
  switch(results.value)
  {
    case 0xFF629D: Serial.println(" FORWARD"); break;
    case 0xFF22DD: Serial.println(" LEFT");    break;
    case 0xFF02FD: Serial.println(" -OK-");    break;
    case 0xFFC23D: Serial.println(" RIGHT");   break;
    case 0xFFA857: Serial.println(" REVERSE"); break;
    case 0xFF6897: Serial.println(" 1");    break;
    case 0xFF9867: Serial.println(" 2");    break;
    case 0xFFB04F: Serial.println(" 3");    break;
    case 0xFF30CF: Serial.println(" 4");    break;
    case 0xFF18E7: Serial.println(" 5");    break;
    case 0xFF7A85: Serial.println(" 6");    break;
    case 0xFF10EF: Serial.println(" 7");    break;
    case 0xFF38C7: Serial.println(" 8");    break;
    case 0xFF5AA5: Serial.println(" 9");    break;
    case 0xFF42BD: Serial.println(" *");    break;
    case 0xFF4AB5: Serial.println(" 0");    break;
    case 0xFF52AD: Serial.println(" #");    break;
    case 0xFFFFFFFF: Serial.println(" REPEAT");break;  

    default: 
      Serial.println(" other button   ");
  }

  delay(500); // Do not get immediate repeat
}

