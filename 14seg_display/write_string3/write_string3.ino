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
	0b100000000000000, /* . */
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

#define ADDR_MASK 0b00000111
#define DATAPIN 11
#define CLOCKPIN 12
#define LED_CNT 6
#define CLOCK_DEL 10
#define CLOCK_POS 0
#define CLOCK_NEG 1
#define EXTRA_PULSES 8
#define VIEW_DEL 1000
#define DISP_CNT (LED_CNT * 2)

void setup() {
  // put your setup code here, to run once:
  DDRB |= B00111111;
  PORTB = B000;
  digitalWrite(DATAPIN, 0);
  digitalWrite(CLOCKPIN, 1);
}

#define FIRST_CHAR 0
#define LAST_CHAR 95




void write_string(char * text, int offset=0){
  int length = strlen(text);

  text += offset;
  int blocks = (length + 1) / 2;
  if(blocks > LED_CNT) blocks = LED_CNT;

  bool abort = false;
  for(int block = 0; !abort && (block < blocks); block++){
    PORTB = block & ADDR_MASK;
    digitalWrite(CLOCKPIN, CLOCK_POS);
    delayMicroseconds(CLOCK_DEL);
    digitalWrite(DATAPIN, 1);
    digitalWrite(CLOCKPIN, CLOCK_NEG);
    delayMicroseconds(CLOCK_DEL);

    int letter = 0;
    letter = text[(block * 2) + 1];
    if(letter < CHAR_OFFSET){
      letter = DEFAULT_CHAR;
      abort = true;
    }
    letter = FourteenSegmentASCII[letter - CHAR_OFFSET];

    for(int j = 0; j < 14; j++){
      int data = letter & 0x01;
      letter = letter >> 1;

      digitalWrite(CLOCKPIN, CLOCK_POS);
      digitalWrite(DATAPIN, data);
      delayMicroseconds(CLOCK_DEL);
      digitalWrite(CLOCKPIN, CLOCK_NEG);
      delayMicroseconds(CLOCK_DEL);

    }

    letter = text[(block * 2) + 0];
    if(letter < CHAR_OFFSET){
      letter = DEFAULT_CHAR;
      abort = true;
    }
    letter = FourteenSegmentASCII[letter - CHAR_OFFSET];

    for(int j = 0; j < 14; j++){
      int data = letter & 0x01;
      letter = letter >> 1;

      digitalWrite(CLOCKPIN, CLOCK_POS);
      digitalWrite(DATAPIN, data);
      delayMicroseconds(CLOCK_DEL);
      digitalWrite(CLOCKPIN, CLOCK_NEG);
      delayMicroseconds(CLOCK_DEL);

    }

    for(int j = 0; j < EXTRA_PULSES; j++){
      digitalWrite(CLOCKPIN, CLOCK_POS);
      digitalWrite(DATAPIN, 0);
      delayMicroseconds(CLOCK_DEL);
      digitalWrite(CLOCKPIN, CLOCK_NEG);
      delayMicroseconds(CLOCK_DEL);
    }

    digitalWrite(CLOCKPIN, CLOCK_POS);
  }
}

void blank_display(){
  write_string("            ");
}

void blink_string(char * text, int times, int on_del, int off_del=0, bool leave_on=true){
  for(int i = 0; i < times; i++){
    write_string(text);
    delay(on_del);
    blank_display();
    delay(off_del ? off_del : on_del);
  }
  if(leave_on) write_string(text);
}

void scroll_string(char * text, int del, int first_del){
  int length = strlen(text);
  int frames = length - DISP_CNT;
  if(frames < 1) frames = 1;

  for(int i = 0; i < frames; i++){
    write_string(text, i);

    if(i == 0)
      delay(first_del);
    else
      delay(del);
  }
}


void loop() {

  blink_string("** WARNING *", 3, 500);

  scroll_string("** WARNING * NO SOLAR Sales People OR SOLAR Inquiries *** YOU ARE TRESSPASSING ***                       ", 100, 1000);

}
