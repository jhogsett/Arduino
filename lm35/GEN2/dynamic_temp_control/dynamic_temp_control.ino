#include <trend_detector.h>

#define SETTLED_PIN 4
#define COOLING_PIN 5
#define FAN_PIN 3
#define SENSOR_PIN A7

#define FAN_MAX 255
#define FAN_MIN 63
#define FAN_OFF 0
#define FAN_SPEED_STEPS 16
#define FAN_SETTLE_TIME 20

#define FAN_OPEN_START 2000
#define FAN_OPEN_DELAY 333
#define FAN_OPEN_STEP 16

#define FAN_SPEED_STEP (int((FAN_MAX - FAN_MIN) / FAN_SPEED_STEPS))
#define FAN_SPEED_MAX_STEP FAN_SPEED_STEPS 

#define SAMPLE_DELAY 1

#define FAST_WINDOW 500
#define SLOW_WINDOW 5000
#define TREND_WINDOW 500
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25

#define FAN_PAUSE_DELAY 5

TrendDetector *trend_detector;

int current_fan_speed = 0;
int current_fan_speed_step = -1;

void fan_opening(){
  Serial.println("opening");
  analogWrite(FAN_PIN, FAN_MAX);
  delay(FAN_OPEN_START);
  for(int i = FAN_MAX; i >= FAN_MIN; i -= FAN_OPEN_STEP){
    Serial.println(i);
    analogWrite(FAN_PIN, i);
    delay(FAN_OPEN_DELAY);
  }  
}

void fan_settle_delay(){
  delay(FAN_SETTLE_TIME);
}

void set_fan_off(bool settle=true){
  digitalWrite(FAN_PIN, LOW);
  digitalWrite(COOLING_PIN, LOW);
  current_fan_speed = 0;
  current_fan_speed_step = 0;
  if(settle)
    fan_settle_delay();
}

void set_fan_speed(int speed, bool settle=true){
  int last_step = current_fan_speed_step;
  if(speed != current_fan_speed_step){
    if(speed <= 0){
      set_fan_off(settle);
      current_fan_speed_step = 0;

    } else {
      if(speed > FAN_SPEED_STEP)
        speed = FAN_SPEED_STEP;
        
      current_fan_speed_step = speed;
      current_fan_speed = (speed * FAN_SPEED_STEPS) + FAN_MIN;

      if(current_fan_speed_step > last_step){
        analogWrite(FAN_PIN, FAN_MAX);
        if(settle)
          fan_settle_delay();
      }
      
      analogWrite(FAN_PIN, current_fan_speed);
      analogWrite(COOLING_PIN, int(current_fan_speed_step * (256 / FAN_SPEED_STEPS)) / 2);
      // if(settle)
      //   fan_settle_delay();
    }
  }
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

#define MODE_CHECKING 0
#define MODE_COOLING 1
int current_mode = MODE_CHECKING;

void setup() {
  Serial.begin(115200);

  pinMode(SETTLED_PIN, OUTPUT);
  pinMode(COOLING_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  fan_opening();

  float sample = sample_temp();
  trend_detector = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, sample_temp());
  }

#define OVERHEAT_TEMP 80.0
#define OVERHEAT_MAX 100.0
#define OVERHEAT_RANGE (OVERHEAT_MAX - OVERHEAT_TEMP)
#define REPORTING_FREQ 500
#define TREND_BOOST 4

int reporting_counter = 0;

bool trend_boost = false;
int over_boost = 0;

void loop() {
  trend_detector->sample(sample_temp());
  if(!trend_detector->settled() && trend_detector->mean() > 0.0){
    digitalWrite(SETTLED_PIN, HIGH);
  } else {
    digitalWrite(SETTLED_PIN, LOW);
  }

  if(++reporting_counter > REPORTING_FREQ){
    reporting_counter = 0;

    Serial.print(trend_detector->fast_mean());
    Serial.print(" ");
    Serial.print(trend_detector->slow_mean());
    Serial.print(" ");
    Serial.print(trend_detector->mean());
    Serial.print(" ");
    Serial.print(current_fan_speed_step);
    Serial.println();
  }

  trend_boost = !trend_detector->settled();
  
  if(trend_detector->fast_mean() > OVERHEAT_TEMP){
    float temp = trend_detector->fast_mean();
    if(temp > OVERHEAT_MAX)
      temp = OVERHEAT_MAX;

    // map the capped overheat amount to 0.0 - 1.0
    float overheat_value = (temp - OVERHEAT_TEMP) / OVERHEAT_RANGE;
    
    // map the overheat value to the fan speed range
    over_boost = 1 + int(overheat_value * FAN_SPEED_MAX_STEP);
  
  } else {
    over_boost = 0;
  }

  set_fan_speed(int(over_boost + (trend_boost ? TREND_BOOST : 0)));
}
