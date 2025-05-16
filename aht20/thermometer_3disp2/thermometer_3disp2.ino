

#include <Wire.h>
#include <HT16K33Disp.h>
#include <random_seed.h>
#include <trend_detector.h>

#define SENSOR_PIN A2

#define RANDOM_SEED_PIN A0
static RandomSeed<RANDOM_SEED_PIN> randomizer;

HT16K33Disp disp1(0x70, 3);
// HT16K33Disp disp2(0x71, 1);
// HT16K33Disp disp3(0x72, 1);

#include <AHT20.h>

AHT20 aht20;

#define FAST_WINDOW 200
#define SLOW_WINDOW 2000
#define TREND_WINDOW 200
#define TREND_SENSE 0.25
#define SETTLED_WINDOW 0.25

#define FIRST_LED 9
#define LAST_LED 11
#define RED_LED 9
#define GREEN_LED 10
#define BLUE_LED 11

TrendDetector *trend_detector_main;
TrendDetector *trend_detector_high;
TrendDetector *trend_detector_low;

#define REPORT_RATE 1000
int report = REPORT_RATE;

void setup() {  
  Serial.begin(115200);
  
  pinMode(SENSOR_PIN, INPUT);
  for(int i = FIRST_LED; i <= LAST_LED; i++)
    pinMode(i, OUTPUT);

  randomizer.randomize();

  byte brightness1[3] = { 2, 2, 2 };

  Wire.begin();

  disp1.init(brightness1);
  // disp2.init(brightness2);
  // disp3.init(brightness3);
  
  aht20.begin();

  float temp = sample_temp();
  trend_detector_main = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, temp);
  trend_detector_high = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, temp);
  trend_detector_low = new TrendDetector(FAST_WINDOW, SLOW_WINDOW, TREND_WINDOW, TREND_SENSE, SETTLED_WINDOW, temp);

}

bool running1, running2, running3 = false;

#define SAMPLES 20
#define SAMPLE_DEL 5
bool sampled;

float sample_temp(){
  float temp_c, temp_f;
  temp_c = aht20.getTemperature();
  temp_f = temp_c * (9.0 / 5.0) + 32.0;
  return temp_f;
}

float sample_humid(){
  float humid;
  humid = aht20.getHumidity();
  return humid;
}

void float_to_fixed(float value, char *buffer, const char *pattern, byte decimals=1){
  int split = 10 * decimals;
  int ivalue = int(value * split);
  int valuei = ivalue / split;
  int valued = ivalue % split;
  sprintf(buffer, pattern, valuei, valued);
}


// https://www.wpc.ncep.noaa.gov/html/heatindex_equation.shtml
// HI = -42.379 + 2.04901523*T + 10.14333127*RH - .22475541*T*RH - .00683783*T*T - .05481717*RH*RH + .00122874*T*T*RH + .00085282*T*RH*RH - .00000199*T*T*RH*RH


void loop() {

  float temp = sample_temp();
  float humid = sample_humid();

  float heat_index = 0.0;
  float steadman_index = 0.5 * (temp + 61.0 + ((temp - 68.0) * 1.2) + (humid * 0.094));
  float initial_index = (steadman_index + temp) / 2.0;
  if(initial_index >= 80.0){
    heat_index = -42.379 
                + (2.04901523 * temp) 
                + (10.14333127 * humid) 
                - (0.22475541 * temp * humid) 
                - (6.83783e-3 * pow(temp, 2)) 
                - (5.481717e-2 * pow(humid, 2)) 
                + (1.22874e-3 * pow(temp, 2) * humid) 
                + (8.5282e-4 * temp * pow(humid, 2)) 
                - (1.99e-6 * pow(temp, 2) * pow(humid, 2));

    float adjust = 0.0;

    if (humid < 13 && temp >= 80 && temp <= 112){
      adjust = ( (13 - humid) / 4) * sqrt((17 - abs(temp - 95)) / 17);
      heat_index -= adjust;

    } else if (humid > 85 && temp >= 80 && temp <= 87){
      adjust = ((humid - 85) / 10) * ((87 - temp) / 5);
      heat_index += adjust;
    }
  } else {
    heat_index = initial_index;
  }

  Serial.println(heat_index);

  char condition[10];
    if (heat_index < 50) { strcpy_P(condition, PSTR("COLD")); }
    else if (heat_index < 64) { strcpy_P(condition, PSTR("COOL")); }
    else if (heat_index < 75) { strcpy_P(condition, PSTR("Nice")); }
    else if (heat_index < 81) { strcpy_P(condition, PSTR("Warm")); }
    else if (heat_index < 90) { strcpy_P(condition, PSTR("HOT")); }
    else { strcpy_P(condition, PSTR("FIRE")); }


  char temps[10];
  float_to_fixed(temp, temps, "%3d.%1d");

  char humids[10];
  float_to_fixed(humid, humids, "%3d.%1d");

  char buffer[30];
  sprintf(buffer, "%4s%4s%4s", temps, humids, condition);

  unsigned long time = millis();
  if(!running1)
    disp1.begin_scroll_string(buffer, 100, 100);

    running1 = disp1.step_scroll_string(time);
}





