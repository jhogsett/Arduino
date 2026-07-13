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

constexpr uint16_t EVENT_WINDOW_SIZE = 10;
constexpr uint16_t BASELINE_WINDOW_SIZE = 300;
constexpr uint16_t PRIMED_VALUE = 50;
constexpr float NOISE_FLOOR = 1.0;
constexpr float EVENT_THRESHOLD = 1.5;

constexpr uint8_t X_LED_PIN = 2;
constexpr uint8_t Y_LED_PIN = 3;

constexpr uint8_t CHAMBER_WIDTH = 110;
constexpr uint8_t WEIGHT_DIAMETER = 34;
constexpr uint8_t CALIBRATION_SAMPLES = 20;
constexpr uint8_t MAX_DISPLACEMENT = (CHAMBER_WIDTH - WEIGHT_DIAMETER) / 2;
uint8_t x_calibrated_center = CHAMBER_WIDTH / 2;
uint8_t y_calibrated_center = CHAMBER_WIDTH / 2;

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

void x_led_on(bool on = true){
  digitalWrite(X_LED_PIN, on ? LOW : HIGH);
}

void y_led_on(bool on = true){
  digitalWrite(Y_LED_PIN, on ? LOW : HIGH);
}

void read_dual_sensors() {
  lox1.rangingTest(&measureY, false); // pass in 'true' to get debug data printout!
  lox2.rangingTest(&measureX, false); // pass in 'true' to get debug data printout!

  if(measureX.RangeStatus == 4 || measureY.RangeStatus == 4){
    // one or both values is out of range; skip this sample round
    return;
  }

  uint16_t current_x = 0;
  uint16_t current_y = 0;

  current_x = measureX.RangeMilliMeter;
  zscore_x.sample(current_x);
  
  current_y = measureY.RangeMilliMeter;
  zscore_y.sample(current_y);

  int16_t x_displacement = current_x - x_calibrated_center;
  int16_t y_displacement = current_y - y_calibrated_center;
  // int16_t x_displacement = zscore_x.mean() - x_calibrated_center;
  // int16_t y_displacement = zscore_y.mean() - y_calibrated_center;

  if (x_displacement > MAX_DISPLACEMENT)  x_displacement = MAX_DISPLACEMENT;
  if (x_displacement < -MAX_DISPLACEMENT) x_displacement = -MAX_DISPLACEMENT;
  if (y_displacement > MAX_DISPLACEMENT)  y_displacement = MAX_DISPLACEMENT;
  if (y_displacement < -MAX_DISPLACEMENT) y_displacement = -MAX_DISPLACEMENT;

  float x_led_pos = (x_displacement + MAX_DISPLACEMENT) / (MAX_DISPLACEMENT * 2);
  float y_led_pos = (y_displacement + MAX_DISPLACEMENT) / (MAX_DISPLACEMENT * 2);
  // Serial.print("min:40.0 max:60.0 ");

  // Serial.print("X:");
  // Serial.print(zscore_x.mean());  
  // Serial.print(" Y:");
  // Serial.print(zscore_y.mean());  
  Serial.print(" XL:");
  Serial.print(x_displacement);
  Serial.print(" YL:");
  Serial.print(y_displacement);
  Serial.print(" XB:");
  Serial.print(zscore_x.baseline_score());  
  Serial.print(" YB:");
  Serial.print(zscore_y.baseline_score());  
  Serial.print(" XS:");
  Serial.print(zscore_x.sample_score());  
  Serial.print(" YS:");
  Serial.print(zscore_y.sample_score());  
  // Serial.print(" ET:");
  // Serial.print(zscore_x.get_event_triggered() ? 100 : 0);  
  // Serial.print(" EA:");
  // Serial.print(zscore_x.is_event_active() ? 100 : 0);  
  Serial.println();

  x_led_on(zscore_x.is_event_active());
  y_led_on(zscore_y.is_event_active());
}

void sleep(){
  if(digitalRead(SLEEP_PIN) == LOW){
    Serial.println("Enter sleep mode");
    while(true);
  }
}

void calibrate() {
  uint16_t x_sum = 0;
  uint16_t y_sum = 0;

  bool x_ok = false;
  bool y_ok = false;

  for (int i = 0; i < CALIBRATION_SAMPLES; ) {
    x_led_on(true);
    lox1.rangingTest(&measureY, false);
    if(measureX.RangeStatus != 4) {
      x_sum += measureX.RangeMilliMeter;
      x_ok = true;
    } else {
      x_ok = false;
    }
    x_led_on(false);
    
    y_led_on(true);
    lox2.rangingTest(&measureX, false);
    if(measureY.RangeStatus != 4) {
      y_sum +=measureY.RangeMilliMeter;
      y_ok = true;
    } else {
      y_ok = false;
    }
    y_led_on(false);

    if(x_ok && y_ok){
      i++;
    }
  }

  x_calibrated_center = x_sum / CALIBRATION_SAMPLES;
  y_calibrated_center = y_sum / CALIBRATION_SAMPLES;

  Serial.print("Calibrated Center Point ");
  Serial.print("X:");
  Serial.print(x_calibrated_center);
  Serial.print(" Y:");
  Serial.print(y_calibrated_center);

  delay(5000);
}


void setup() {
  Serial.begin(115200);

  // wait until serial port opens for native USB devices
  while (! Serial) { delay(1); }

  Wire.begin();
  Wire.setClock(400000);

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

  sleep();  

  pinMode(X_LED_PIN, OUTPUT);
  pinMode(Y_LED_PIN, OUTPUT);
  digitalWrite(X_LED_PIN, LOW);
  digitalWrite(Y_LED_PIN, LOW);
  delay(500);
  digitalWrite(X_LED_PIN, HIGH);
  digitalWrite(Y_LED_PIN, HIGH);
  delay(500);

  calibrate();

}

void loop() {
  sleep();  

  read_dual_sensors();
  // delay(10);
}
