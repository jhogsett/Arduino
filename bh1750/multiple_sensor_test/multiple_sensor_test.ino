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
BH1750 lightMeter4;


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

  TCA9548A(3);
  lightMeter4.begin();
  float lux4 = lightMeter4.readLightLevel();

  // Serial.println(F("BH1750 Test begin"));

  // trend_detector_main = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);
  // trend_detector_high = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);
  // trend_detector_low = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, lux);

  // pinMode(13, OUTPUT);
}

void loop() {
  Serial.println("--------");

  TCA9548A(0);
  float lux1 = lightMeter1.readLightLevel();
  Serial.println(lux1);

  TCA9548A(1);
  float lux2 = lightMeter2.readLightLevel();
  Serial.println(lux2);

  TCA9548A(2);
  float lux3 = lightMeter3.readLightLevel();
  Serial.println(lux3);

  TCA9548A(3);
  float lux4 = lightMeter4.readLightLevel();
  Serial.println(lux4);

  delay(1000);

}
