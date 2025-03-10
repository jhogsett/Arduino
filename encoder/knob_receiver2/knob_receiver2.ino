
#define RED_PIN 9
#define GREEN_PIN 10
#define BLUE_PIN 11

#define RED_ID 1
#define GREEN_ID 2
#define BLUE_ID 3

class LEDFader
{
public:
  LEDFader(byte led_pin){
    _led_pin = led_pin;
    pinMode(_led_pin, OUTPUT);
    analogWrite(_led_pin, 0);
    _value = 1;
    _active = false;
  }

  void show(){
    analogWrite(_led_pin, _active ? _value : 0);
  }

  void activate(bool active=true){
    _active = active;
    show();
  }

  void toggle(){
    activate(!_active);
  }

  void set(int value){
    if(value < 1)
      value = 1;
    else if(value > 255)
      value = 255;
    _value = value;
    show();
  }
  
  void up(int steps=1){
    set(_value + steps);    
  }

  void down(int steps=1){
    set(_value - steps);    
  }
  
  void on(){
    set(255);
    activate();
  }

private:
  byte _led_pin;
  byte _value;
  bool _active;
};

LEDFader red_fader(RED_PIN);
LEDFader green_fader(GREEN_PIN);
LEDFader blue_fader(BLUE_PIN);

void setup() {
  Serial.begin(115200);
}

void handle_fader(LEDFader &fader, int data){
  switch(data){
    case 0:
      // decrement
      fader.down();
      break;
    case 1:
      // button press
      fader.toggle();
      break;
    case 2:
      // increment
      fader.up();
      break;
    case 3:
      // button repeat
      fader.on();
      break;
  }
}

void loop() {
  red_fader.show();
  green_fader.show();
  blue_fader.show();
  
  char buffer[10];
  byte read = 0;
  if((read = Serial.readBytesUntil('\n', buffer, 9)) != 0){
    buffer[read] = '\0';
    // Serial.println(buffer);

    int id = buffer[0] - '0';
    int data = (buffer[1] - '0');
    // Serial.println(id);
    // Serial.println(data);

    if(id > 0 && id < 4 && data >= 0 and data <= 3){
      switch(id){
        case RED_ID:
          handle_fader(red_fader, data);
          break;
        case GREEN_ID:
          handle_fader(green_fader, data);
          break;
        case BLUE_ID:
          handle_fader(blue_fader, data);
          break;
      }
    }
  }
}
