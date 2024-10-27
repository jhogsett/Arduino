// https://arduino.stackexchange.com/questions/60764/coin-recognition-using-coin-accepter-in-arduino-uno

// pennies and dimes get confused (also they are harder because they're easier to give too much of a shove)
// without a 100nF cap between the coin pin and ground, the pulses can be noisy

#include  <Arduino.h>

const int coinIntPin = 2;          //interruptPin 0 is digital pin 2
volatile boolean newCoin = false;                  
volatile int pulseCount;          //counts pulse for 
volatile long timeFPulse = 0;     // to determine when First impulse was interrupted
int coinType;
long interval;           // to determine interval from first and last impulse
const int max_interval = 500;
int credits = 0;
int deposited = 0;
const int slow_blink_time = 125;
const int fast_blink_time = 25;
const int gap_time = slow_blink_time * 2;
const int totalblink_time = 1000;

void coinInserted(){
    newCoin = true; 
    timeFPulse = millis();
    pulseCount++; 
}




void blink_times(int times, int time){
    for(int i = 0; i < times; i++){
      digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
      delay(time);                      // wait for a second
      digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
      delay(time);                      // wait for a second
    }
}


/*
 *  Project     Segmented LED Display - ASCII Library
 *  @author     David Madison
 *  @link       github.com/dmadison/Segmented-LED-Display-ASCII
 *  @license    MIT - Copyright (c) 2017 David Madison
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

// 5/26/24 bit positions 12 and 14 swapped for LTM8495G display
// otherwise lower two star diagonal segments are backwards

const uint16_t FourteenSegmentASCII[96] = {
	0b000000000000000, /* (space) */
	0b100000000000110, /* ! */
	0b000001000000010, /* " */
	0b001001011001110, /* # */
	0b001001011101101, /* $ */
	0b011111111100100, /* % */
	0b000101101011001, /* & */
	0b000001000000000, /* ' */
	0b000110000000000, /* ( */
	0b010000100000000, /* ) */
	0b011111111000000, /* * */
	0b001001011000000, /* + */
	0b010000000000000, /* , */
	0b000000011000000, /* - */
  0b001000000000000, /* i */ //  0b000000000001000, // /* _ */ 0b100000000000000, /* . */
	0b010010000000000, /* / */
	0b010010000111111, /* 0 */
	0b000010000000110, /* 1 */
	0b000000011011011, /* 2 */
	0b000000010001111, /* 3 */
	0b000000011100110, /* 4 */
	0b000100001101001, /* 5 */
	0b000000011111101, /* 6 */
	0b000000000000111, /* 7 */
	0b000000011111111, /* 8 */
	0b000000011101111, /* 9 */
	0b001001000000000, /* : */
	0b010001000000000, /* ; */
	0b000110001000000, /* < */
	0b000000011001000, /* = */
	0b010000110000000, /* > */
	0b101000010000011, /* ? */
	0b000001010111011, /* @ */
	0b000000011110111, /* A */
	0b001001010001111, /* B */
	0b000000000111001, /* C */
	0b001001000001111, /* D */
	0b000000001111001, /* E */
	0b000000001110001, /* F */
	0b000000010111101, /* G */
	0b000000011110110, /* H */
	0b001001000001001, /* I */
	0b000000000011110, /* J */
	0b000110001110000, /* K */
	0b000000000111000, /* L */
	0b000010100110110, /* M */
	0b000100100110110, /* N */
	0b000000000111111, /* O */
	0b000000011110011, /* P */
	0b000100000111111, /* Q */
	0b000100011110011, /* R */
	0b000000011101101, /* S */
	0b001001000000001, /* T */
	0b000000000111110, /* U */
	0b010010000110000, /* V */
	0b010100000110110, /* W */
	0b010110100000000, /* X */
	0b000000011101110, /* Y */
	0b010010000001001, /* Z */
	0b000000000111001, /* [ */
	0b000100100000000, /* \ */
	0b000000000001111, /* ] */
	0b010100000000000, /* ^ */
	0b000000000001000, /* _ */
	0b000000100000000, /* ` */
	0b001000001011000, /* a */
	0b000100001111000, /* b */
	0b000000011011000, /* c */
	0b010000010001110, /* d */
	0b010000001011000, /* e */
	0b001010011000000, /* f */
	0b000010010001110, /* g */
	0b001000001110000, /* h */
	0b001000000000000, /* i */
	0b010001000010000, /* j */
	0b001111000000000, /* k */
	0b000000000110000, /* l */
	0b001000011010100, /* m */
	0b001000001010000, /* n */
	0b000000011011100, /* o */
	0b000000101110000, /* p */
	0b000010010000110, /* q */
	0b000000001010000, /* r */
	0b000100010001000, /* s */
	0b000000001111000, /* t */
	0b000000000011100, /* u */
	0b010000000010000, /* v */
	0b010100000010100, /* w */
	0b010110100000000, /* x */
	0b000001010001110, /* y */
	0b010000001001000, /* z */
	0b010000101001001, /* { */
	0b001001000000000, /* | */
	0b000110010001001, /* } */
	0b010010011000000, /* ~ */
	0b000000000000000, /* (del) */
};

#define CHAR_OFFSET 32
#define DEFAULT_CHAR 32

#define SEGMENTS 14
#define ADDR_MASK 0b00000111
#define DATAPIN 11
#define CLOCKPIN 12
#define LED_CNT 6
#define CLOCK_DEL 0
#define CLOCK_POS 0
#define CLOCK_NEG 1
#define EXTRA_PULSES 8
#define VIEW_DEL 1000
#define DISP_CNT (LED_CNT * 2)
#define DEF_BLINK_DEL 400
#define DEF_BLINK_TIMES 1
#define DEF_FLASH_ON 2000
#define DEF_FLASH_OFF 500
#define DEF_SCROLL_DEL 100
#define DEF_SCROLL_WAIT 1000
#define CREDIT_COST 69
#define WIN_CREDITS 69

char *display_blank;

#define FIRST_CHAR 0
#define LAST_CHAR 95

void send_data(int data){
  digitalWrite(CLOCKPIN, CLOCK_POS);
  digitalWrite(DATAPIN, data);
  delayMicroseconds(CLOCK_DEL);
  digitalWrite(CLOCKPIN, CLOCK_NEG);
  delayMicroseconds(CLOCK_DEL);
}

void send_char(int letter){
  for(int i = 0; i < SEGMENTS; i++){
    int data = letter & 0b1;
    send_data(data);
    letter = letter >> 1;
  }
}

void write_string(char * text, int offset=0){
  int length = strlen(text);

  text += offset;
  int blocks = (length + 1) / 2;
  if(blocks > LED_CNT) blocks = LED_CNT;

  bool abort = false;
  for(int block = 0; !abort && (block < blocks); block++){
    PORTB = block & ADDR_MASK;

    // first bit must be a 1
    send_data(1);

    int letter = 0;

    letter = text[(block * 2) + 1];
    if(letter < CHAR_OFFSET){
      letter = DEFAULT_CHAR;
      if(letter == 0) abort = true;
    }
    letter = FourteenSegmentASCII[letter - CHAR_OFFSET];

    send_char(letter);

    letter = text[(block * 2) + 0];
    if(letter < CHAR_OFFSET){
      letter = DEFAULT_CHAR;
      abort = true;
    }
    letter = FourteenSegmentASCII[letter - CHAR_OFFSET];
    send_char(letter);

    for(int j = 0; j < EXTRA_PULSES; j++){
      send_data(0);
    }

    // put the clock back to high
    digitalWrite(CLOCKPIN, CLOCK_POS);
  }
}

void blank_display(){
  write_string(display_blank);
}

void flash_string(char * text, int on_del=1000, int off_del=0, bool leave_on=true){
  write_string(text);
  delay(on_del);
  if(!leave_on){
    blank_display();
    delay(off_del ? off_del : on_del);
  }
}

void blink_string(char * text, int times=DEF_BLINK_TIMES, int on_del=DEF_BLINK_DEL, int off_del=0, bool leave_on=true){
  for(int i = 0; i < times; i++){
    flash_string(text, on_del, off_del);
  }
  if(leave_on) write_string(text);
}

// add off delay
void scroll_string(char * text, int del=DEF_SCROLL_DEL, int first_del=DEF_SCROLL_WAIT, bool scroll_out=true, bool scroll_in=false, int off_del=0){
  int length = strlen(text);
  int frames = length - DISP_CNT;
  if(frames < 1) frames = 1;

  int offset;
  for(offset = 0; offset < frames; offset++){
    write_string(text, offset);

    if(offset == 0)
      delay(first_del);
    else
      delay(del);
  }

  delay(off_del);
}

void setup() {
  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(coinIntPin, INPUT_PULLUP);
  // Serial.begin(9600);              
  attachInterrupt(digitalPinToInterrupt(coinIntPin), coinInserted, RISING);

  DDRB |= B00111111;
  PORTB = B000;
  digitalWrite(DATAPIN, 0);
  digitalWrite(CLOCKPIN, 1);
  display_blank = "            ";
  deposited = CREDIT_COST;

  blank_display();
  scroll_string("            Are You Ready To Pay", 100, 0, false);
  flash_string("Ready To Pay", 3000, 500);
}

void loop()
{
     while (newCoin == true)
    {
            interval = millis() - timeFPulse;
            if(interval > max_interval) 
            {                                                                  
              coinType = pulseCount;
              sendData(coinType);
              pulseCount = 0;
              newCoin = false;
             }  
    }
}

char buf[DISP_CNT + 1];
char buf2[DISP_CNT + 1];

#define COIN_GOLD 2
#define COIN_NICKEL 3
#define COIN_DIME 4
#define COIN_QUARTER 5
#define COIN_SPECIAL 6

void(* resetFunc) (void) = 0;

void sendData(int coinType){
  if(coinType == COIN_SPECIAL){
    resetFunc();
  }

  if(coinType == COIN_GOLD){
    credits = credits + 1;
  }
  // else{
  //   credits = 0;
  // }

  if(coinType == COIN_GOLD) {
    if(deposited >= CREDIT_COST){
      if(deposited == CREDIT_COST){
        blink_string("* YOU  WIN *", 5, 350);        

        for(int i = 0; i < WIN_CREDITS; i++){
          credits += 1;
          sprintf(buf, "Credits %4d", credits);
          flash_string(buf, 250, 0, true);
        }

        deposited = CREDIT_COST;
        float purse_f = deposited / 100.0;
        dtostrf(purse_f, 5, 2, buf2);
        sprintf(buf, "Purse $%s", buf2);
        flash_string(buf, 1000, 0, true);
      } else {
        while(deposited >= CREDIT_COST){
          credits += 1;
          deposited -= CREDIT_COST;

          scroll_string("* Buying One Credit for $0.69 ", 200, 1000, false, false, 1000);

          float purse_f = deposited / 100.0;
          dtostrf(purse_f, 5, 2, buf2);
          sprintf(buf, "Purse $%s", buf2);
          flash_string(buf, 1000, 0, true);

          sprintf(buf, "Credits %4d", credits);
          flash_string(buf, 1000, 0, true);
        }
      }
    } else
      sprintf(buf, "Credits %4d", credits);
  }
  else {
    if(coinType == COIN_NICKEL) deposited += 5;
    if(coinType == COIN_DIME) deposited += 10;
    if(coinType == COIN_QUARTER) deposited += 25;

    float purse_f = deposited / 100.0;
    dtostrf(purse_f, 5, 2, buf2);
    sprintf(buf, "Purse $%s", buf2);
  }

  
  flash_string(buf);



  // if(coinType == 1){
  //   flash_string("GOLD COIN");
  // }
  // if(coinType == 2){
  //   flash_string("1 Cent");
  // }

  // if(coinType == 3){
  //   flash_string("5 Cents");
  // }

  // if(coinType == 4){
  //   flash_string("10 Cents");
  // }

  // if(coinType == 5){
  //   flash_string("25 Cents");
  // }

}



// void loop() {
//   blink_string(" N O T I C E");
//   scroll_string("NO SALES PEOPLE --- NO SOLAR INQUIRIES --- You are WARNED to LEAVE AT ONCE --- YOU ARE TRESSPASSING ---                       ", 100, 1000);
// }


