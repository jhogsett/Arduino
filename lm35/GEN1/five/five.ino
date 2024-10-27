#define LED_PIN 13
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

#define SHORT_WINDOW 10
#define LONG_WINDOW 1000
#define TREND_WINDOW 100

#define FAN_PAUSE_DELAY 10

void blinky_delay(int del, int res=2){
  int times = res / 2;
  for(int i = 0; i < times; i++){
    digitalWrite(LED_PIN, HIGH);
    delay(del / res);
    digitalWrite(LED_PIN, LOW);
    delay(del / res);
  }
}

int current_fan_speed = 0;
int current_fan_speed_step = FAN_SPEED_MAX_STEP;

float mean_temp_accum = 0.0;
float last_mean_temp = 0.0;

float long_mean_temp_accum = 0.0;
float last_long_mean_temp = 0.0;

float mean_trend_accum = 0.0;
float last_mean_trend = 0.0;

void fan_settle_delay(){
  delay(FAN_SETTLE_TIME);
  // blinky_delay(FAN_SETTLE_TIME, 10);
}

void set_fan_off(bool settle=true){
  // Serial.println("set fan off");
  digitalWrite(FAN_PIN, LOW);
  current_fan_speed = 0;
  if(settle)
    fan_settle_delay();
}

void set_fan_speed(int speed){
  Serial.print("sfs:");
  Serial.println(speed);
  if(speed <= 0){
    set_fan_off();
  } else {
    if(speed > FAN_SPEED_STEPS)
      speed = FAN_SPEED_STEPS;
    current_fan_speed = speed * FAN_SPEED_STEP - 1;
    current_fan_speed_step = speed;
    analogWrite(FAN_PIN, current_fan_speed);
    fan_settle_delay();
  }
}

void pause_fan(){
  set_fan_off(false);
  delay(FAN_PAUSE_DELAY);
}

void resume_fan(){
  if(current_fan_speed != 0){
    analogWrite(FAN_PIN, current_fan_speed);
  }
  // delay(FAN_PAUSE_DELAY);
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

float mean_sample_temp(){
  float sample = sample_temp();
  mean_temp_accum -= last_mean_temp;
  mean_temp_accum += sample;
  last_mean_temp = mean_temp_accum / SHORT_WINDOW;

  long_mean_temp_accum -= last_long_mean_temp;
  long_mean_temp_accum += sample;
  last_long_mean_temp = long_mean_temp_accum / LONG_WINDOW;


  float trend;
  if(last_long_mean_temp > last_mean_temp)
    // trend = -1.0;
    trend = -0.5;
  else if(last_long_mean_temp < last_mean_temp)
    trend = 0.5;
    // trend = 1.0;

  mean_trend_accum -= last_mean_trend;
  mean_trend_accum += trend;
  last_mean_trend = mean_trend_accum / TREND_WINDOW;

  return last_mean_temp;
}

void prime_temp_sampler(){
  float sample = sample_temp();
  mean_temp_accum = sample * SHORT_WINDOW;
  long_mean_temp_accum = sample * LONG_WINDOW;

  for(int i = 0; i <= SHORT_WINDOW; i++){
    mean_sample_temp();
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  // fan_opening();  
  prime_temp_sampler();

  while(true){
    Serial.println("test without fan");
    set_fan_speed(0);
    for(int i = 0; i < 2000; i++){
      float temp_f = mean_sample_temp();

      Serial.print("Short Mean Temp: ");
      Serial.println(last_mean_temp);

      Serial.print("Long Mean Temp: ");
      Serial.println(last_long_mean_temp);

      Serial.print("Trend:");
      Serial.println(last_mean_trend);

      if(last_mean_trend >= 0.5)
        digitalWrite(LED_PIN, HIGH);
      else
        digitalWrite(LED_PIN, LOW);
    }

    Serial.println("test with fan");
    set_fan_speed(FAN_SPEED_MAX_STEP);
    for(int i = 0; i < 2000; i++){
      float temp_f = mean_sample_temp();

      Serial.print("Short Mean Temp: ");
      Serial.println(last_mean_temp);

      Serial.print("Long Mean Temp: ");
      Serial.println(last_long_mean_temp);

      Serial.print("Trend:");
      Serial.println(last_mean_trend);

      if(last_mean_trend >= 0.5)
        digitalWrite(LED_PIN, HIGH);
      else
        digitalWrite(LED_PIN, LOW);
    }
  }
  
  // while(true){
  //   // int r = random(0, FAN_SPEED_STEPS+1);
  //   // Serial.print("Speed:");
  //   // Serial.println(r);
  //   // set_fan_speed(r);

  //   float temp_f = mean_sample_temp();
  //   Serial.print("Temperature = ");
  //   Serial.println(temp_f);

  //   Serial.print("Trend = ");
  //   Serial.println(last_long_mean_temp);
  // }
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
