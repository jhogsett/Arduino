#define LIGHT_SENSOR_PIN A0
#define LED_PIN 13

#define LIGHT_SAMPLE_COUNT 20
int light_samples[LIGHT_SAMPLE_COUNT] = {0,0,0,0,0};

void setup() {
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(115200);
}

void loop() {
//  int sensorValue = get_next_light_sample();
  int sensorValue = get_average_light_level();

  digitalWrite(LED_PIN,  HIGH); 
  delay(sensorValue); 
  digitalWrite(LED_PIN,  LOW);
  delay(sensorValue);

  Serial.println(sensorValue, DEC);
}


int get_average_light_level(){
  int result;
  for(int i = 0; i < LIGHT_SAMPLE_COUNT; i++){
    result = get_next_light_sample();  
  }
}

int get_next_light_sample(){
  int accum = 0;
  for(int i = LIGHT_SAMPLE_COUNT-1; i > 0; i--){
    light_samples[i] = light_samples[i-1];
    accum += light_samples[i];
  }
  light_samples[0] = get_one_light_sample();
  accum += light_samples[0];

  return accum / LIGHT_SAMPLE_COUNT;
}

int get_one_light_sample() {
  return analogRead(LIGHT_SENSOR_PIN);  
}




