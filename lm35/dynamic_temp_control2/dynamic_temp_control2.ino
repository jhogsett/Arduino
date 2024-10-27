// #include <trend_detector.h>
#include "value_controller.h"

#define SETTLED_PIN 2
#define COOLING_PIN 5
#define HEATING_PIN 3
#define FAN_PIN 6
#define SENSOR_PIN A7

#define FAN_MAX 255
#define FAN_MIN 63
#define FAN_OFF 0
#define FAN_SPEED_STEPS 16
#define FAN_BOOST_TIME 20

#define FAN_OPEN_START 2000
#define FAN_OPEN_DELAY 200
#define FAN_OPEN_STEP 16

#define FAST_WINDOW 500
#define SLOW_WINDOW 5000
#define TREND_WINDOW 500
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25

#define OVERHEAT_TEMP 85.0
#define OVERHEAT_MAX 100.0
#define OVERHEAT_RANGE (OVERHEAT_MAX - OVERHEAT_TEMP)
#define TREND_BOOST 3

#define REPORTING_FREQ 1000

ValueController *value_controller;

void fan_opening(){
  Serial.println("opening");

  digitalWrite(SETTLED_PIN, HIGH);
  digitalWrite(COOLING_PIN, HIGH);
  digitalWrite(HEATING_PIN, HIGH);

  analogWrite(FAN_PIN, FAN_MAX);
  delay(FAN_OPEN_START);
  for(int i = FAN_MAX; i >= FAN_MIN; i -= FAN_OPEN_STEP){
    Serial.println(i);
    analogWrite(FAN_PIN, i);
    delay(FAN_OPEN_DELAY);
  }  

  digitalWrite(SETTLED_PIN, LOW);
  digitalWrite(COOLING_PIN, LOW);
  digitalWrite(HEATING_PIN, LOW);
  digitalWrite(FAN_PIN, LOW);
}

float sample_temp(){
  int temp_adc_val;
  float temp_val, temp_c, temp_f;

  temp_adc_val = analogRead(SENSOR_PIN);

  temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  temp_c = (temp_val / 10.0);	/* LM35 gives output of 10mv/°C */
  temp_f = temp_c * (9.0 / 5.0) + 32.0;

  return temp_f;
}

void setup() {
  Serial.begin(115200);

  pinMode(SETTLED_PIN, OUTPUT);
  pinMode(COOLING_PIN, OUTPUT);
  pinMode(HEATING_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  fan_opening();

  value_controller = new ValueController(
    ValueController::DOWN_CONTROLLER, 
    FAN_PIN, 
    FAN_MAX, 
    FAN_MIN, 
    FAN_SPEED_STEPS, 
    FAN_BOOST_TIME, 
    OVERHEAT_TEMP, 
    OVERHEAT_MAX, 
    FAST_WINDOW, 
    SLOW_WINDOW, 
    TREND_WINDOW, 
    TREND_SENSE, 
    SETTLED_WINDOW, 
    TREND_BOOST,
    sample_temp());
  }

int reporting_counter = 0;

void loop() {
  value_controller->control(sample_temp());
  
  if(value_controller->surging()){
    digitalWrite(SETTLED_PIN, HIGH);
  } else {
    digitalWrite(SETTLED_PIN, LOW);
  }
  
  analogWrite(COOLING_PIN, int(value_controller->_current_controller_speed_step * (256 / FAN_SPEED_STEPS)) / 2);

  if(++reporting_counter > REPORTING_FREQ){
    reporting_counter = 0;

    Serial.print(value_controller->_trend_detector->fast_mean());
    Serial.print(" ");
    Serial.print(value_controller->_trend_detector->slow_mean());
    Serial.print(" ");
    Serial.print(value_controller->_trend_detector->mean());
    Serial.print(" ");
    Serial.print(value_controller->_current_controller_speed_step);
    Serial.println();
  }
}
