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

BH1750 lightMeter;


#define FAST_WINDOW 200
#define SLOW_WINDOW 2000
#define TREND_WINDOW 200
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25
// #define TREND_BOOST 2


TrendDetector *trend_detector_main;
TrendDetector *trend_detector_high;
TrendDetector *trend_detector_low;

#define REPORT_RATE 1000
int report = REPORT_RATE;

void setup() {
  Serial.begin(115200);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
  // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use
  // Wire.begin(D2, D1);

  lightMeter.begin();

  float lux = lightMeter.readLightLevel();

  // Serial.println(F("BH1750 Test begin"));

  trend_detector_main = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);
  trend_detector_high = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);
  trend_detector_low = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);

  pinMode(9, OUTPUT);
  pinMode(11, OUTPUT);
}

void loop() {
  float lux = lightMeter.readLightLevel();

  trend_detector_main->sample(lux);
  float mean = trend_detector_main->slow_mean();

  if(lux > mean){
    trend_detector_high->sample(lux);
  } else {
    trend_detector_low->sample(lux);
  }

  float range = trend_detector_high->fast_mean() - trend_detector_low->fast_mean();
  // float range = trend_detector_high->slow_mean() - trend_detector_low->slow_mean();
  float spread = range / 256.0;
  float base = trend_detector_low->slow_mean();
  float val = (trend_detector_main->fast_mean() - base) * spread;
  float ival = val * 256.0;
  int i = int(ival);

  if(i >= 0)
    analogWrite(9, 1 + (i * 4));

  // Serial.println(i);

  if(false && !--report){
    Serial.print(lux);
    Serial.print(" - ");

    Serial.print(trend_detector_main->fast_mean());
    Serial.print(" ");
    Serial.print(trend_detector_main->slow_mean());
    Serial.print(" ");
    Serial.print(trend_detector_main->mean());
    Serial.print(" ");
    Serial.print(trend_detector_main->settled());
    Serial.print("   ");

    Serial.print(trend_detector_high->fast_mean());
    Serial.print(" ");
    Serial.print(trend_detector_high->slow_mean());
    Serial.print(" ");
    Serial.print(trend_detector_high->mean());
    Serial.print(" ");
    Serial.print(trend_detector_high->settled());
    Serial.print("   ");

    Serial.print(trend_detector_low->fast_mean());
    Serial.print(" ");
    Serial.print(trend_detector_low->slow_mean());
    Serial.print(" ");
    Serial.print(trend_detector_low->mean());
    Serial.print(" ");
    Serial.print(trend_detector_low->settled());
    Serial.print(" ");

    Serial.println();

    digitalWrite(11, trend_detector_main->settled() ? HIGH : LOW);

    report = REPORT_RATE;
  }
}
