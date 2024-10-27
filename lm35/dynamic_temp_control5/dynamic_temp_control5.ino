// #include <trend_detector.h>
#include "value_controller.h"

#define COOLER_SURGE_PIN 2
#define HEATER_SURGE_PIN 4
#define COOLING_PIN 5
#define HEATING_PIN 3
#define COOLER_PIN 6
#define HEATER_PIN 10
#define SENSOR_PIN A7

#define COOLER_MAX 255
#define COOLER_MIN 63
#define COOLER_SPEED_STEPS 16
#define COOLER_BOOST_TIME 20
#define COOLER_EXP_BOOST 10000

#define HEATER_MAX 255
#define HEATER_MIN 63
#define HEATER_OFF 0
#define HEATER_SPEED_STEPS 16
#define HEATER_BOOST_TIME 20
#define HEATER_EXP_BOOST 10000

#define FAN_OPEN_START 2000
#define FAN_OPEN_DELAY 100
#define FAN_OPEN_STEP 16

#define COOLER_FAST_WINDOW 500
#define COOLER_SLOW_WINDOW 5000
#define COOLER_TREND_WINDOW 500
#define COOLER_TREND_SENSE 0.25
#define COOLER_SETTLED_WINDOW 0.25
#define COOLER_TREND_BOOST 3

#define HEATER_FAST_WINDOW 500
#define HEATER_SLOW_WINDOW 5000
#define HEATER_TREND_WINDOW 500
#define HEATER_TREND_SENSE 0.25
#define HEATER_SETTLED_WINDOW 0.25
#define HEATER_TREND_BOOST 3

#define COOLER_START_TEMP 155.0
#define COOLER_MAX_TEMP 175.0

#define HEATER_START_TEMP 155.0
#define HEATER_MAX_TEMP 135.0

#define REPORTING_FREQ 1000

ValueController *cooler_controller;
ValueController *heater_controller;

void fan_opening(){
  Serial.println("opening");

  digitalWrite(COOLER_SURGE_PIN, HIGH);
  digitalWrite(HEATER_SURGE_PIN, HIGH);
  digitalWrite(COOLING_PIN, HIGH);
  digitalWrite(HEATING_PIN, HIGH);

  analogWrite(COOLER_PIN, COOLER_MAX);
  delay(FAN_OPEN_START);
  for(int i = COOLER_MAX; i >= COOLER_MIN; i -= FAN_OPEN_STEP){
    Serial.println(i);
    analogWrite(COOLER_PIN, i);
    delay(FAN_OPEN_DELAY);
  }  

  digitalWrite(COOLER_SURGE_PIN, LOW);
  digitalWrite(HEATER_SURGE_PIN, LOW);
  digitalWrite(COOLING_PIN, LOW);
  digitalWrite(HEATING_PIN, LOW);
  digitalWrite(COOLER_PIN, LOW);
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

  pinMode(COOLER_SURGE_PIN, OUTPUT);
  pinMode(COOLING_PIN, OUTPUT);
  pinMode(HEATING_PIN, OUTPUT);
  pinMode(COOLER_PIN, OUTPUT);
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  fan_opening();
  
  float initial_sample = sample_temp();

  cooler_controller = new ValueController(
    ValueController::DOWN_CONTROLLER, 
    COOLER_PIN, 
    COOLER_MAX, 
    COOLER_MIN, 
    COOLER_SPEED_STEPS, 
    COOLER_BOOST_TIME, 
    COOLER_START_TEMP, 
    COOLER_MAX_TEMP, 
    COOLER_FAST_WINDOW, 
    COOLER_SLOW_WINDOW, 
    COOLER_TREND_WINDOW, 
    COOLER_TREND_SENSE, 
    COOLER_SETTLED_WINDOW, 
    COOLER_TREND_BOOST,
    COOLER_EXP_BOOST,
    initial_sample);

  heater_controller = new ValueController(
    ValueController::UP_CONTROLLER, 
    HEATER_PIN, 
    HEATER_MAX, 
    HEATER_MIN, 
    HEATER_SPEED_STEPS, 
    HEATER_BOOST_TIME, 
    HEATER_START_TEMP, 
    HEATER_MAX_TEMP, 
    HEATER_FAST_WINDOW, 
    HEATER_SLOW_WINDOW, 
    HEATER_TREND_WINDOW, 
    HEATER_TREND_SENSE, 
    HEATER_SETTLED_WINDOW, 
    HEATER_TREND_BOOST,
    HEATER_EXP_BOOST,
    initial_sample);
}

int reporting_counter = 0;

void loop() {
  float sample = sample_temp();
  cooler_controller->control(sample);
  heater_controller->control(sample);
  
  if(cooler_controller->surging() == -1){
    digitalWrite(COOLER_SURGE_PIN, HIGH);
  } else {
    digitalWrite(COOLER_SURGE_PIN, LOW);
  }

  if(heater_controller->surging() == 1){
    digitalWrite(HEATER_SURGE_PIN, HIGH);
  } else {
    digitalWrite(HEATER_SURGE_PIN, LOW);
  }
  
  analogWrite(COOLING_PIN, int(cooler_controller->_current_controller_speed_step * (256 / COOLER_SPEED_STEPS)) / 2);
  analogWrite(HEATING_PIN, int(heater_controller->_current_controller_speed_step * (256 / HEATER_SPEED_STEPS)) / 2);

  if(++reporting_counter > REPORTING_FREQ){
    reporting_counter = 0;

    Serial.print(cooler_controller->_trend_detector->fast_mean());
    Serial.print(" ");
    Serial.print(cooler_controller->_trend_detector->slow_mean());
    Serial.print(" ");
    Serial.print(cooler_controller->_trend_detector->mean());
    Serial.print(" ");
    Serial.print(cooler_controller->_current_controller_speed_step);
    Serial.print("    ");
    Serial.print(heater_controller->_trend_detector->fast_mean());
    Serial.print(" ");
    Serial.print(heater_controller->_trend_detector->slow_mean());
    Serial.print(" ");
    Serial.print(heater_controller->_trend_detector->mean());
    Serial.print(" ");
    Serial.print(heater_controller->_current_controller_speed_step);
    Serial.println();
  }
}
