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


TrendDetector *trend_detector;

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

  trend_detector = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);

  pinMode(13, OUTPUT);
}

void loop() {
  float lux = lightMeter.readLightLevel();

  trend_detector->sample(lux);

  Serial.print(trend_detector->fast_mean());
  Serial.print(" ");
  Serial.print(trend_detector->slow_mean());
  Serial.print(" ");
  Serial.print(trend_detector->mean());
  Serial.print(" ");
  Serial.print(trend_detector->settled());
  Serial.println(" ");

  digitalWrite(13, trend_detector->settled() ? HIGH : LOW);

  // Serial.print("Light: ");
  // Serial.print(lux);
  // Serial.println(" lx");
  // delay(1000);
}
