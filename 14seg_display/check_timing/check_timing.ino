
#define ADDR_MASK 0b00000111

#define DATAPIN 11
#define CLOCKPIN 12
#define LED_CNT 6
#define CLOCK_DEL 100
#define CLOCK_POS 0
#define CLOCK_NEG 1
#define PULSE_CNT 29

void setup() {
  // put your setup code here, to run once:
  DDRB |= B00111111;
  PORTB = B000;
  digitalWrite(DATAPIN, 0);
  digitalWrite(CLOCKPIN, 1);
}

int a = 0;

void loop() {
  // put your main code here, to run repeatedly:
  a++;
  for(int i = 0; i < LED_CNT; i++){
    PORTB = i & ADDR_MASK;
    // digitalWrite(13, 1);
    // delay(150);
    // digitalWrite(13, 0);
    // delay(150);

    digitalWrite(CLOCKPIN, CLOCK_POS);
    delayMicroseconds(CLOCK_DEL);
    digitalWrite(DATAPIN, 1);
    digitalWrite(CLOCKPIN, CLOCK_NEG);
    delayMicroseconds(CLOCK_DEL);

    if(++a & 0x01){
      for(int j = 0; j <= PULSE_CNT; j++){
          digitalWrite(CLOCKPIN, CLOCK_POS);
          digitalWrite(DATAPIN, (j & 0x01));
          delayMicroseconds(CLOCK_DEL);
          digitalWrite(CLOCKPIN, CLOCK_NEG);
          delayMicroseconds(CLOCK_DEL);
        }
    } else {
      for(int j = 0; j <= PULSE_CNT; j++){
          digitalWrite(CLOCKPIN, CLOCK_POS);
          digitalWrite(DATAPIN, !(j & 0x01));
          delayMicroseconds(CLOCK_DEL);
          digitalWrite(CLOCKPIN, CLOCK_NEG);
          delayMicroseconds(CLOCK_DEL);
        }
    }

  }
}
