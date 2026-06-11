#include "Adafruit_VL53L0X.h"

Adafruit_VL53L0X lox = Adafruit_VL53L0X();

#define SENSOR_EN_0 5
#define SENSOR_EN_1 6

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }

  pinMode(SENSOR_EN_0, OUTPUT);
  pinMode(SENSOR_EN_1, OUTPUT);

  // pinMode(SENSOR_EN_0, INPUT);
  // pinMode(SENSOR_EN_1, INPUT);

  digitalWrite(SENSOR_EN_0, HIGH);
  digitalWrite(SENSOR_EN_1, HIGH);

  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 

  digitalWrite(SENSOR_EN_0, LOW);
  digitalWrite(SENSOR_EN_1, LOW);

}


void loop() {
  VL53L0X_RangingMeasurementData_t measure;
    
  Serial.print("Reading a measurement... ");

  // digitalWrite(SENSOR_EN_0, LOW);
  digitalWrite(SENSOR_EN_0, HIGH);
  // delay(10);

  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    Serial.print("Distance (mm): "); Serial.println(measure.RangeMilliMeter);
  } else {
    Serial.println(" out of range ");
  }

  digitalWrite(SENSOR_EN_0, LOW);

  // delay(100);
}
