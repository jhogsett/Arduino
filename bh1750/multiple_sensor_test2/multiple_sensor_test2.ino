/*

Example of BH1750 library usage.

This example initialises the BH1750 object using the default high resolution
continuous mode and then makes a light level reading every second.

Connections

  - VCC to 3V3 or 5V
  - GND to GND
  - SCL to SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due, on
    esp8266 free selectable)
  - SDA to SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due, on
    esp8266 free selectable)
  - ADD to (not connected) or GND

ADD pin is used to set sensor I2C address. If it has voltage greater or equal
to 0.7VCC voltage (e.g. you've connected it to VCC) the sensor address will be
0x5C. In other case (if ADD voltage less than 0.7 * VCC) the sensor address
will be 0x23 (by default).

*/

#include <BH1750.h>
#include <Wire.h>
#include <trend_detector.h>

BH1750 lightMeter1;
BH1750 lightMeter2;
BH1750 lightMeter3;
// BH1750 lightMeter4;


#define FAST_WINDOW 200
#define SLOW_WINDOW 2000
#define TREND_WINDOW 200
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25
// #define TREND_BOOST 2

// Nov 22, 2011 — PWM: 3, 5, 6, 9, 10, and 11. Provide 8-bit PWM output with the analogWrite() function. However, pin 3 is Reset.Read more

#define LED_LEVEL_1   9
#define LED_SETTLED_1 6
#define LED_LEVEL_2   10
#define LED_SETTLED_2 7
#define LED_LEVEL_3   11
#define LED_SETTLED_3 8

TrendDetector *trend_detector_main;
TrendDetector *trend_detector_high;
TrendDetector *trend_detector_low;

#define REPORT_RATE 50
int report = REPORT_RATE;

void TCA9548A(uint8_t bus){
    Wire.beginTransmission(0x70);  // TCA9548A address is 0x70
    Wire.write(1 << bus);          // send byte to select bus
    Wire.endTransmission();
    // Serial.print(bus);
}

void setup() {
  Serial.begin(115200);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
  // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use
  // Wire.begin(D2, D1);

  TCA9548A(0);
  lightMeter1.begin();
  float lux1 = lightMeter1.readLightLevel();

  TCA9548A(1);
  lightMeter2.begin();
  float lux2 = lightMeter1.readLightLevel();

  TCA9548A(2);
  lightMeter3.begin();
  float lux3 = lightMeter3.readLightLevel();

  // TCA9548A(3);
  // lightMeter4.begin();
  // float lux4 = lightMeter4.readLightLevel();

  // Serial.println(F("BH1750 Test begin"));

  trend_detector_main = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux1);
  trend_detector_high = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux1);
  trend_detector_low = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux1);

  pinMode(LED_LEVEL_1, OUTPUT);
  pinMode(LED_SETTLED_1, OUTPUT);
  pinMode(LED_LEVEL_2, OUTPUT);
  pinMode(LED_SETTLED_2, OUTPUT);
  pinMode(LED_LEVEL_3, OUTPUT);
  pinMode(LED_SETTLED_3, OUTPUT);
}

void loop() {
  // Serial.println("--------");

  TCA9548A(0);
  float lux1 = lightMeter1.readLightLevel();
  // Serial.println(lux1);

  TCA9548A(1);
  float lux2 = lightMeter2.readLightLevel();
  // Serial.println(lux2);

  TCA9548A(2);
  float lux3 = lightMeter3.readLightLevel();
  // Serial.println(lux3);

  // TCA9548A(3);
  // float lux4 = lightMeter4.readLightLevel();
  // Serial.println(lux4);

  // delay(1000);
  // Serial.print(lux1);
  // Serial.print(",");
  // Serial.print(lux2);
  // Serial.print(",");
  // Serial.println(lux3);

  trend_detector_main->sample(lux1);
  float mean = trend_detector_main->slow_mean();

  if(lux1 > mean){
    trend_detector_high->sample(lux1);
  } else {
    trend_detector_low->sample(lux1);
  }

  float range = trend_detector_high->fast_mean() - trend_detector_low->fast_mean();
  // float range = trend_detector_high->slow_mean() - trend_detector_low->slow_mean();
  float spread = range / 256.0;
  float base = trend_detector_low->slow_mean();
  float val = (trend_detector_main->fast_mean() - base) * spread;
  float ival = val * 256.0;
  int i = int(ival);

  if(i >= 0)
    analogWrite(LED_LEVEL_1, 1 + (i * 4));

  if(!--report){
    digitalWrite(LED_SETTLED_1, trend_detector_main->settled() ? HIGH : LOW);

    Serial.print(lux1);
    Serial.print(",");
    Serial.print(lux2);
    Serial.print(",");
    Serial.println(lux3);

    report = REPORT_RATE;
  }
}
