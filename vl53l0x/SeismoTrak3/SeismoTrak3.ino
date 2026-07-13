#include <windowed_mean.h>
#include "Adafruit_VL53L0X.h"
#include "zscore.h"

// address we will assign if dual sensor is present
#define LOX1_ADDRESS 0x30
#define LOX2_ADDRESS 0x31

// set the pins to shutdown
#define SHT_LOX1 5
#define SHT_LOX2 6

// objects for the vl53l0x
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();

// this holds the measurement
VL53L0X_RangingMeasurementData_t measureX;
VL53L0X_RangingMeasurementData_t measureY;

constexpr uint8_t SLEEP_PIN = A0;
constexpr uint16_t SLEEP_TIME = 10000;
constexpr uint32_t TIME_BUDGET = 100000;

constexpr uint16_t EVENT_WINDOW_SIZE = 25;
constexpr uint16_t BASELINE_WINDOW_SIZE = 300;
constexpr uint16_t PRIMED_VALUE = 50;
constexpr float NOISE_FLOOR = 2.0;
constexpr float EVENT_THRESHOLD = 2.0;

// WindowedMean mean_x(EVENT_WINDOW_SIZE, PRIMED_VALUE);
// WindowedMean mean_y(EVENT_WINDOW_SIZE, PRIMED_VALUE);

ZScore zscore_x(EVENT_WINDOW_SIZE, BASELINE_WINDOW_SIZE, PRIMED_VALUE, NOISE_FLOOR, EVENT_THRESHOLD);
ZScore zscore_y(EVENT_WINDOW_SIZE, BASELINE_WINDOW_SIZE, PRIMED_VALUE, NOISE_FLOOR, EVENT_THRESHOLD);

/*
    Reset all sensors by setting all of their XSHUT pins low for delay(10), then set all XSHUT high to bring out of reset
    Keep sensor #1 awake by keeping XSHUT pin high
    Put all other sensors into shutdown by pulling XSHUT pins low
    Initialize sensor #1 with lox.begin(new_i2c_address) Pick any number but 0x29 and it must be under 0x7F. Going with 0x30 to 0x3F is probably OK.
    Keep sensor #1 awake, and now bring sensor #2 out of reset by setting its XSHUT pin high.
    Initialize sensor #2 with lox.begin(new_i2c_address) Pick any number but 0x29 and whatever you set the first sensor to
 */
void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  // activating LOX1 and resetting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);

  // initing LOX1
  if(!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot first VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX2, HIGH);
  delay(10);

  //initing LOX2
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
}

void read_dual_sensors() {
  
  lox1.rangingTest(&measureY, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measureX, false); // pass in 'true' to get debug data printout!

  if(measureX.RangeStatus != 4) {     // if not out of range
    // mean_x.sample(measureX.RangeMilliMeter);
    zscore_x.sample(measureX.RangeMilliMeter);
  }
  
  if(measureY.RangeStatus != 4) {
    // mean_y.sample(measureY.RangeMilliMeter);
    zscore_y.sample(measureY.RangeMilliMeter);
  }

  // Serial.print("min:40.0 max:60.0 ");

  Serial.print("X:");
  Serial.print(zscore_x.mean());  
  Serial.print(" Y:");
  Serial.print(zscore_y.mean());  
  Serial.print(" XB:");
  Serial.print(zscore_x.baseline_score());  
  Serial.print(" YB:");
  Serial.print(zscore_y.baseline_score());  
  Serial.print(" XS:");
  Serial.print(zscore_x.sample_score());  
  Serial.print(" YS:");
  Serial.print(zscore_y.sample_score());  
  Serial.print(" ET:");
  Serial.print(zscore_x.get_event_triggered() ? 100 : 0);  
  Serial.print(" EA:");
  Serial.print(zscore_x.is_event_active() ? 100 : 0);  
  Serial.println();
}

void sleep(){
  if(digitalRead(SLEEP_PIN) == LOW){
    Serial.println("Enter sleep mode");
    while(true);
  }
}

void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);

  pinMode(SLEEP_PIN, INPUT_PULLUP);

  // Serial.println(F("Shutdown pins inited..."));

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);

  // Serial.println(F("Both in reset mode...(pins are low)"));
  
  
  // Serial.println(F("Starting..."));
  setID();

  // Explicit high-speed call for Adafruit's library
  // 20000 microseconds = 20ms timing budget
  lox1.setMeasurementTimingBudgetMicroSeconds(TIME_BUDGET); // 20 ms timing budget for high speed
  lox2.setMeasurementTimingBudgetMicroSeconds(TIME_BUDGET); // 20 ms timing budget for high speed

  // Serial.println("\"X\",\"Y\"");
  sleep();  
}

void loop() {
  sleep();  

  read_dual_sensors();
  // delay(10);
}
