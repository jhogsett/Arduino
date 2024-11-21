#include <Arduino.h>
#include <Wire.h>
#include <Stepper.h>
#include <HT16K33Disp.h>

// https://arduino.stackexchange.com/questions/60764/coin-recognition-using-coin-accepter-in-arduino-uno
// without a 100nF cap between the coin pin and ground, the pulses can be noisy

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

#define MOTOR_SPEED 9
#define MOTOR_PIN1 4
#define MOTOR_PIN2 5
#define MOTOR_PIN3 6
#define MOTOR_PIN4 7

char *display_blank;

#define FIRST_CHAR 0
#define LAST_CHAR 95

char buf[DISP_CNT + 1];
char buf2[DISP_CNT + 1];

#define COIN_GOLD 2
#define COIN_NICKEL 3
#define COIN_DIME 4
#define COIN_QUARTER 5
#define COIN_SPECIAL 6

const int coinIntPin = 2;          //interruptPin 0 is digital pin 2
volatile boolean newCoin = false;                  
volatile int pulseCount;          //counts pulse for 
volatile long timeFPulse = 0;     // to determine when First impulse was interrupted
int coinType;
long interval;           // to determine interval from first and last impulse
const int max_interval = 500;
int credits = 0;
int deposited = 0;

// const int slow_blink_time = 125;
// const int fast_blink_time = 25;
// const int gap_time = slow_blink_time * 2;
// const int totalblink_time = 1000;

const int stepsPerRevolution = 2048;

Stepper myStepper(stepsPerRevolution, MOTOR_PIN1, MOTOR_PIN2, MOTOR_PIN3, MOTOR_PIN4);

HT16K33Disp disp(0x70, 1);

void setup() {
  myStepper.setSpeed(MOTOR_SPEED);
  myStepper.step(stepsPerRevolution);

  byte brightness[1] = {3};
  Wire.begin();
  disp.Init(brightness); 
  disp.clear();

  // pinMode(LED_BUILTIN, OUTPUT);
  pinMode(coinIntPin, INPUT_PULLUP);
  // Serial.begin(9600);              
  attachInterrupt(digitalPinToInterrupt(coinIntPin), coinInserted, RISING);

  // DDRB |= B00111111;
  // PORTB = B000;
  // digitalWrite(DATAPIN, 0);
  // digitalWrite(CLOCKPIN, 1);
  // display_blank = "            ";
  // deposited = 0;

  // blank_display();

  disp.scroll_string("    **** ENJOY CANDY **** ONLY 25 CENTS ****    ");
  disp.show_string("$ .25");

  // scroll_string("            Enjoy Candy  $0.25", 100, 0, false);
  // flash_string("Candy  $0.25", 3000, 500);
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


void coinInserted(){
    newCoin = true; 
    timeFPulse = millis();
    pulseCount++; 
}

// void blink_times(int times, int time){
//     for(int i = 0; i < times; i++){
//       digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
//       delay(time);                      // wait for a second
//       digitalWrite(LED_BUILTIN, LOW);   // turn the LED off by making the voltage LOW
//       delay(time);                      // wait for a second
//     }
// }

// void send_data(int data){
//   digitalWrite(CLOCKPIN, CLOCK_POS);
//   digitalWrite(DATAPIN, data);
//   delayMicroseconds(CLOCK_DEL);
//   digitalWrite(CLOCKPIN, CLOCK_NEG);
//   delayMicroseconds(CLOCK_DEL);
// }

// void send_char(int letter){
//   for(int i = 0; i < SEGMENTS; i++){
//     int data = letter & 0b1;
//     send_data(data);
//     letter = letter >> 1;
//   }
// }

// void blank_display(){
//   write_string(display_blank);
// }

// void blink_string(char * text, int times=DEF_BLINK_TIMES, int on_del=DEF_BLINK_DEL, int off_del=0, bool leave_on=true){
//   for(int i = 0; i < times; i++){
//     flash_string(text, on_del, off_del);
//   }
//   if(leave_on) write_string(text);
// }

void(* resetFunc) (void) = 0;


#define WHEEL_SEGMENTS 3
#define STEPS_PER_SEGMENT 693
int wheel_segment = 0;

void sendData(int coinType){
  if(coinType == COIN_SPECIAL){
    resetFunc();
  }

  if(coinType == COIN_NICKEL) deposited += 5;
  if(coinType == COIN_DIME) deposited += 10;
  if(coinType == COIN_QUARTER) deposited += 25;
  if(coinType == COIN_GOLD) deposited += 25;

  float purse_f = deposited / 100.0;
  dtostrf(purse_f, 4, 2, buf2);
  sprintf(buf, "$%s", buf2);
  disp.show_string(buf);

#define CANDY_COST 25
  if(deposited >= CANDY_COST) {
    deposited -= CANDY_COST;
    
    // scroll_string("Here's Your Candy!", 200, 1000, false, false, 1000);

    wheel_segment = (wheel_segment++) % WHEEL_SEGMENTS;
    int leap_step = wheel_segment == 0 ? 1 : 0;
    myStepper.step(STEPS_PER_SEGMENT - leap_step);

    float purse_f = deposited / 100.0;
    dtostrf(purse_f, 5, 2, buf2);
    sprintf(buf, "Funds $%s", buf2);
    disp.scroll_string(buf);
    
    disp.clear();
    delay(1000);

  disp.scroll_string("    **** ENJOY CANDY **** ONLY 25 CENTS ****    ");
  disp.show_string("$ .25");
  }
    
  // }

  

}

