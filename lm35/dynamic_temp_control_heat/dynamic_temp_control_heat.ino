#include <trend_detector.h>

#define SETTLED_PIN 2

#define COOLING_PIN 5
#define HEATING_PIN 3

#define HEAT_PIN 10
#define COOL_PIN 6

#define SENSOR_PIN A7

#define HEAT_MAX 255
#define HEAT_MIN 63
#define HEAT_OFF 0
#define HEAT_LEVEL_STEPS 16
#define HEAT_SETTLE_TIME 20

// #define HEAT_OPEN_START 2000
// #define HEAT_OPEN_DELAY 200
// #define HEAT_OPEN_STEP 16

#define HEAT_LEVEL_STEP (int((HEAT_MAX - HEAT_MIN) / HEAT_LEVEL_STEPS))
#define HEAT_LEVEL_MAX_STEP HEAT_LEVEL_STEPS 

#define SAMPLE_DELAY 1

#define FAST_WINDOW 500
#define SLOW_WINDOW 5000
#define TREND_WINDOW 500
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25

#define HEAT_PAUSE_DELAY 5

#define UNDERHEAT_TEMP 100.0
#define UNDERHEAT_MAX 75.0
#define UNDERHEAT_RANGE (UNDERHEAT_TEMP - UNDERHEAT_MAX)
#define REPORTING_FREQ 1000
#define TREND_BOOST 0

TrendDetector *trend_detector;

int current_heat_level = 0;
int current_heat_level_step = -1;

void heat_opening(){
  // Serial.println("opening");
  // analogWrite(HEAT_PIN, HEAT_MAX);
  // delay(HEAT_OPEN_START);
  // for(int i = HEAT_MAX; i >= HEAT_MIN; i -= HEAT_OPEN_STEP){
  //   Serial.println(i);
  //   analogWrite(HEAT_PIN, i);
  //   delay(HEAT_OPEN_DELAY);
  // }  
}

void heat_settle_delay(){
  delay(HEAT_SETTLE_TIME);
}

void set_heat_off(bool settle=true){
  digitalWrite(HEAT_PIN, LOW);
  digitalWrite(HEATING_PIN, LOW);
  current_heat_level = 0;
  current_heat_level_step = 0;
  if(settle)
    heat_settle_delay();
}

void set_heat_level(int level, bool settle=true){
  int last_step = current_heat_level_step;
  if(level != current_heat_level_step){
    if(level <= 0){
      set_heat_off(settle);
      current_heat_level_step = 0;

    } else {
      if(level > HEAT_LEVEL_STEP)
        level = HEAT_LEVEL_STEP;
        
      current_heat_level_step = level;
      current_heat_level = (level * HEAT_LEVEL_STEPS) + HEAT_MIN;

      if(current_heat_level_step > last_step){
        analogWrite(HEAT_PIN, HEAT_MAX);
        if(settle)
          heat_settle_delay();
      }
      
      analogWrite(HEAT_PIN, current_heat_level);
      analogWrite(HEATING_PIN, int(current_heat_level_step * (256 / HEAT_LEVEL_STEPS)) / 2);
      // if(settle)
      //   heat_settle_delay();
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
  pinMode(HEATING_PIN, OUTPUT);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  heat_opening();

  float sample = sample_temp();
  trend_detector = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, sample_temp());
  }


int reporting_counter = 0;

bool trend_boost = false;
int under_boost = 0;

void loop() {
  trend_detector->sample(sample_temp());
  if(!trend_detector->settled() && trend_detector->mean() < 0.0){
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
    Serial.print(current_heat_level_step);
    Serial.println();
  }

  trend_boost = !trend_detector->settled();
  
  if(trend_detector->fast_mean() < UNDERHEAT_TEMP){
    float temp = trend_detector->fast_mean();
    if(temp < UNDERHEAT_MAX)
      temp = UNDERHEAT_MAX;

    // map the capped UNDERHEAT amount to 0.0 - 1.0
    float UNDERHEAT_value = (UNDERHEAT_TEMP - temp) / UNDERHEAT_RANGE;
    
    // map the UNDERHEAT value to the heat level range
    under_boost = 1 + int(UNDERHEAT_value * HEAT_LEVEL_MAX_STEP);
  
  } else {
    under_boost = 0;
  }

  set_heat_level(int(under_boost + (trend_boost ? TREND_BOOST : 0)));
}
