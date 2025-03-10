#ifndef __ENCODER_HANDLER_H__
#define __ENCODER_HANDLER_H__

#include <Arduino.h>
#include <limits.h>

#define UNPRESSED 0
#define PRESSED 1
#define NOTIFIED 2

class EncoderHandler
{
public:
  EncoderHandler(byte id, int clock_pin, int data_pin, int button_pin, byte pulses_per_detent=1){
    _id = id;
    _button_pin = button_pin;
    _pulses_per_detent = pulses_per_detent;

    old_dial_position = LONG_MIN;
    old_encoded_position = LONG_MIN;
    button_state = UNPRESSED;
    valid_time = 0;

    pinMode(_button_pin, INPUT_PULLUP);

    pencoder = new Encoder(clock_pin, data_pin);
  }

  void step(){
    switch(button_state){
      case UNPRESSED:
        if(digitalRead(_button_pin) == LOW){
          valid_time = millis() + DEBOUNCE_TIME;
          button_state = PRESSED;        
        }        
        break;
      case PRESSED:
        if(digitalRead(_button_pin) == HIGH){
          button_state = UNPRESSED;
        } else {
          if(millis() >= valid_time){
            send(0);
            button_state = NOTIFIED;          
          }
        }
        break;
      case NOTIFIED:
        if(digitalRead(_button_pin) == HIGH){
          button_state = UNPRESSED;
        }
        break;
    }
    
    long new_dial_position = pencoder->read();
    if (new_dial_position != old_dial_position) {
      int diff = new_dial_position - old_dial_position;
      old_dial_position = new_dial_position;
      
      if(diff != 0){
        long new_encoded_position = new_dial_position / _pulses_per_detent;
        if(new_encoded_position != old_encoded_position){
          old_encoded_position = new_encoded_position;
          send(diff);      
        }
      }
    }
  }
  
  // diff is -1 for CCW, 1 for CW, 0 for button press
  // sent is: 0 for CCW, 2 for CW, 1 for button press
  void send(int diff){
    char buffer[10];
    sprintf(buffer, "%d,%d", _id, diff + 1);
    Serial.println(buffer);
  }

  const int DEBOUNCE_TIME = 50;

private:  
  byte _id;
  byte _button_pin;
  byte _pulses_per_detent;
  Encoder * pencoder;
  long old_dial_position;
  long old_encoded_position;

  byte button_state;  
  unsigned long valid_time;

};


#endif
