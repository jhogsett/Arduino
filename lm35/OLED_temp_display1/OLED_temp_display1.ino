// pulsing the heater should be secondary to calculating the heater speed
//   on sample(), process the heater pulsing
//     when the current speed is zero, don't immediately turn off the heater
//       when time to pulse the heater on, don't turn on if the speed is zero
//     when the heater is on, the speed based time could change, but that change won't interfere 


// reset heater on/off counters when switching from heating to cooling /etc

// choose to expedite if the speed hasn't increased (not that the barrier has been reached)
// if the speed decreases, cancel or reduce the expedite

// might be good to reduce the expedite boost if the value starts picking up

#include "value_controller.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define COOLER_SURGE_PIN 2
#define HEATER_SURGE_PIN 4
#define COOLING_PIN 5
#define HEATING_PIN 3
#define COOLER_PIN 6
#define HEATER_PIN 10
#define SENSOR_PIN A7

#define COOLER_MAX 255 // maximum fan on voltage
#define COOLER_MIN 31 // minimum fan on voltage
#define COOLER_SPEED_STEPS 10
#define COOLER_BOOST_TIME 20 // when speeding up, set to max speed briefly
#define COOLER_EXP_BOOST 40000 // boost speed to expedite cooling

#define HEATER_MAX 30000 // maximum on time per power window
#define HEATER_MIN 500 // minimum on time per power window
#define HEATER_SPEED_STEPS 10
#define HEATER_OFF_TIME 1000 // mandatory off time
#define HEATER_EXP_BOOST 90000

#define FAN_OPEN_START 1000
#define FAN_OPEN_DELAY 100
#define FAN_OPEN_STEP 16

#define COOLER_FAST_WINDOW 200
#define COOLER_SLOW_WINDOW 2000
#define COOLER_TREND_WINDOW 200
#define COOLER_TREND_SENSE 0.25
#define COOLER_SETTLED_WINDOW 0.25
#define COOLER_TREND_BOOST 2

#define HEATER_FAST_WINDOW 25
#define HEATER_SLOW_WINDOW 250
#define HEATER_TREND_WINDOW 25
#define HEATER_TREND_SENSE 0.25
#define HEATER_SETTLED_WINDOW 0.25
#define HEATER_TREND_BOOST 1

#define TARGET_TEMP 112.0
#define TARGET_RANGE 3.0

#define COOLER_START_TEMP TARGET_TEMP
#define COOLER_MAX_TEMP (TARGET_TEMP + TARGET_RANGE)

#define HEATER_START_TEMP TARGET_TEMP
#define HEATER_MAX_TEMP (TARGET_TEMP - TARGET_RANGE)

#define REPORTING_FREQ 2000

ValueController *cooler_controller;
ValueController *heater_controller;

void fan_opening(){
  digitalWrite(COOLER_SURGE_PIN, HIGH);
  digitalWrite(HEATER_SURGE_PIN, HIGH);
  digitalWrite(COOLING_PIN, HIGH);
  digitalWrite(HEATING_PIN, HIGH);

  analogWrite(COOLER_PIN, COOLER_MAX);
  delay(FAN_OPEN_START);
  for(int i = COOLER_MAX; i >= COOLER_MIN; i -= FAN_OPEN_STEP){
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

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();

  // Draw a single pixel in white
  // display.drawPixel(10, 10, SSD1306_WHITE);

  // Show the display buffer on the screen. You MUST call display() after
  // drawing commands to make them visible on screen!
  display.display();
  // delay(2000);

  // pinMode(COOLER_SURGE_PIN, OUTPUT);
  // pinMode(COOLING_PIN, OUTPUT);
  // pinMode(HEATING_PIN, OUTPUT);
  // pinMode(COOLER_PIN, OUTPUT);
  // pinMode(HEATER_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  // fan_opening();
  
  float initial_sample = sample_temp();

  // cooler_controller = new ValueController(
  //   ValueController::DOWN_CONTROLLER, 
  //   COOLER_PIN, 
  //   COOLER_MAX, 
  //   COOLER_MIN, 
  //   COOLER_SPEED_STEPS, 
  //   COOLER_BOOST_TIME, 
  //   COOLER_START_TEMP, 
  //   COOLER_MAX_TEMP, 
  //   COOLER_FAST_WINDOW, 
  //   COOLER_SLOW_WINDOW, 
  //   COOLER_TREND_WINDOW, 
  //   COOLER_TREND_SENSE, 
  //   COOLER_SETTLED_WINDOW, 
  //   COOLER_TREND_BOOST,
  //   COOLER_EXP_BOOST,
  //   initial_sample);

//   heater_controller = new ValueController(
//     ValueController::UP_CONTROLLER, 
//     HEATER_PIN, 
//     HEATER_MAX, 
//     HEATER_MIN, 
//     HEATER_SPEED_STEPS, 
//     HEATER_OFF_TIME, 
//     HEATER_START_TEMP, 
//     HEATER_MAX_TEMP, 
//     HEATER_FAST_WINDOW, 
//     HEATER_SLOW_WINDOW, 
//     HEATER_TREND_WINDOW, 
//     HEATER_TREND_SENSE, 
//     HEATER_SETTLED_WINDOW, 
//     HEATER_TREND_BOOST,
//     HEATER_EXP_BOOST,
//     initial_sample);
}

// int reporting_counter = 0;

void loop() {
  float sample = sample_temp();
  Serial.println(sample);

  display.clearDisplay();
  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  
  display.print(sample, 1);
  display.display();

  // cooler_controller->control(sample);
  // heater_controller->control(sample);
  
  // if(cooler_controller->surging() == -1){
  //   digitalWrite(COOLER_SURGE_PIN, HIGH);
  // } else {
  //   digitalWrite(COOLER_SURGE_PIN, LOW);
  // }

  // if(heater_controller->surging() == 1){
  //   digitalWrite(HEATER_SURGE_PIN, HIGH);
  // } else {
  //   digitalWrite(HEATER_SURGE_PIN, LOW);
  // }
  
  // analogWrite(COOLING_PIN, int(cooler_controller->_current_controller_speed_step * (256 / COOLER_SPEED_STEPS)) / 2);
  // analogWrite(HEATING_PIN, int(heater_controller->_current_controller_speed_step * (256 / HEATER_SPEED_STEPS)) / 2);

  // if(++reporting_counter > REPORTING_FREQ){
  //   reporting_counter = 0;

  //   Serial.print("heater_speed:");
  //   Serial.print(heater_controller->_current_controller_speed_step * 10);
  //   Serial.print(",");
  //   Serial.print("heater_temp:");
  //   Serial.print(heater_controller->_trend_detector->fast_mean());
  //   Serial.print(",");
  //   Serial.print("cooler_speed:");
  //   Serial.print(cooler_controller->_current_controller_speed_step * 10);
  //   Serial.print(",");
  //   Serial.print("cooler_temp:");
  //   Serial.println(cooler_controller->_trend_detector->fast_mean());
  // }
}
