#include "windowed_mean.h"

// need a target temp, when above that, the fan is always on at least a minimum amount, 
// depending on how far away from the target, the minimum speed is set
// when the temp trends up, the minimum temp is boosted
// after some time if it's still tending up, the min is boosted again

#define SETTLED_PIN 4
#define COOLING_PIN 2
#define FAN_PIN 3
#define SENSOR_PIN A7

#define FAN_MAX 255
#define FAN_MIN 31
#define FAN_OFF 0
#define FAN_SPEED_STEP 16
#define FAN_SETTLE_TIME 3000
#define FAN_SPEED_STEPS (int((FAN_MAX - FAN_MIN) / FAN_SPEED_STEP) + 1)
#define FAN_SPEED_MAX_STEP FAN_SPEED_STEPS 
#define FAN_OPEN_DELAY 250
#define FAN_OPEN_STEP 25

#define SAMPLE_DELAY 10

#define FAST_WINDOW 20
#define SLOW_WINDOW 300
#define TREND_WINDOW 50

#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25

#define FAN_PAUSE_DELAY 10

void blinky_delay(int del, int res=2){
  int times = res / 2;
  for(int i = 0; i < times; i++){
    digitalWrite(SETTLED_PIN, HIGH);
    delay(del / res);
    digitalWrite(SETTLED_PIN, LOW);
    delay(del / res);
  }
}

int current_fan_speed = 0;
int current_fan_speed_step = -1;

void fan_settle_delay(){
  delay(FAN_SETTLE_TIME);
  // blinky_delay(FAN_SETTLE_TIME, 10);
}

void set_fan_off(bool settle=true){
  // Serial.println("set fan off");
  digitalWrite(FAN_PIN, LOW);
  current_fan_speed = 0;
  current_fan_speed_step = 0;
  if(settle)
    fan_settle_delay();
}

void set_fan_speed(int speed, bool settle=true){
  Serial.print(speed);
  Serial.print(current_fan_speed_step);
  // delay(1000);
  if(speed != current_fan_speed_step)
    if(speed <= 0){
      set_fan_off(settle);
      current_fan_speed_step = 0;
    } else {
      if(speed > FAN_SPEED_STEPS)
        speed = FAN_SPEED_STEPS;
      current_fan_speed_step = speed;
      current_fan_speed = speed * FAN_SPEED_STEP - 1;
      analogWrite(FAN_PIN, current_fan_speed);
      if(settle)
        fan_settle_delay();
    }
}

void pause_fan(){
  digitalWrite(FAN_PIN, LOW);
  delay(FAN_PAUSE_DELAY);
}

void resume_fan(){
  if(current_fan_speed != 0)
    analogWrite(FAN_PIN, current_fan_speed);
  delay(FAN_PAUSE_DELAY);
}

void fan_opening(){
  Serial.println("opening");
  analogWrite(FAN_PIN, FAN_MAX);
  for(int i = FAN_MAX; i >= FAN_MIN; i -= FAN_OPEN_STEP){
    Serial.println(i);
    analogWrite(FAN_PIN, i);
    blinky_delay(FAN_OPEN_DELAY);
  }  
}

float sample_temp(){
  int temp_adc_val;
  float temp_val, temp_c, temp_f;

  pause_fan();
  temp_adc_val = analogRead(SENSOR_PIN);
  resume_fan();

  temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  temp_c = (temp_val / 10.0);	/* LM35 gives output of 10mv/°C */
  temp_f = temp_c * (9.0 / 5.0) + 32.0;

  delay(SAMPLE_DELAY);
  return temp_f;
}

WindowedMean *fast_mean, *slow_mean, *trend_mean;

float mean_sample_temp(){
  float sample = sample_temp();

  fast_mean->sample(sample);
  slow_mean->sample(sample);

  float trend;
  if(slow_mean->mean() > fast_mean->mean())
    trend = -TREND_SENSE * (slow_mean->mean() - fast_mean->mean());
  else if(slow_mean->mean() < fast_mean->mean())
    trend = TREND_SENSE * (fast_mean->mean() - slow_mean->mean());

  trend_mean->sample(trend);
  return fast_mean->mean();
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
  
  fast_mean = new WindowedMean(FAST_WINDOW, sample_temp());
  slow_mean = new WindowedMean(SLOW_WINDOW, sample_temp());
  trend_mean = new WindowedMean(TREND_WINDOW, 0.0);

  while(true){
    while(current_mode == MODE_CHECKING){
      float temp_f = mean_sample_temp();

      if(trend_mean->mean() >= -SETTLED_WINDOW && trend_mean->mean() <= SETTLED_WINDOW){
        digitalWrite(SETTLED_PIN, HIGH);
      } else {
        digitalWrite(SETTLED_PIN, LOW);

        if(trend_mean->mean() > 0.0){
          current_mode = MODE_COOLING;
          set_fan_speed(FAN_SPEED_MAX_STEP, false);
          digitalWrite(COOLING_PIN, HIGH);
          break;
        } else {
          set_fan_speed(0, false);
          digitalWrite(COOLING_PIN, LOW);
        }
      }

      Serial.print("Checking: ");
      Serial.print(fast_mean->mean());
      Serial.print(" ");
      Serial.print(slow_mean->mean());
      Serial.print(" ");
      Serial.println(trend_mean->mean());
    }

    while(current_mode == MODE_COOLING){
      float temp_f = mean_sample_temp();

      if(trend_mean->mean() <= 0.0){
        digitalWrite(SETTLED_PIN, HIGH);
        current_mode = MODE_CHECKING;
        set_fan_speed(0, false);
        digitalWrite(COOLING_PIN, LOW);
        break;
      }

      Serial.print("Cooling: ");
      Serial.print(fast_mean->mean());
      Serial.print(" ");
      Serial.print(slow_mean->mean());
      Serial.print(" ");
      Serial.println(trend_mean->mean());
    }
  }
}

void loop() {
  int temp_adc_val;
  float temp_val, temp_c, temp_f;
  temp_adc_val = analogRead(SENSOR_PIN);
  temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  temp_c = (temp_val / 10.0);	/* LM35 gives output of 10mv/°C */
  temp_f = temp_c * (9.0 / 5.0) + 32.0;

  Serial.print("Temperature = ");
  Serial.println(temp_f);

  delay(1000);
}
