#define SPEAKER_PIN 13
#define DELAY 300

// #define START 200
// #define END 1000
#define TIMES 100
#define TDELAY 100

#define START_F 60
#define END_F 2200
#define STEP_F 10
#define DURATION 300
#define GAP 50


void setup() {
  Serial.begin(115200);
}

void beep(int freq, int time){
  int pulse_width = (int)((1000000L / freq) / 2L);
  int pulses = (int)((time * 1000L) / (pulse_width * 2L));
	pinMode(SPEAKER_PIN, OUTPUT);
	for(int j = 0; j < pulses; j++){
		digitalWrite(SPEAKER_PIN, HIGH);
		delayMicroseconds(pulse_width);
		digitalWrite(SPEAKER_PIN, LOW);
		delayMicroseconds(pulse_width);
	}
}

void loop() {
  for(int freq = START_F; freq <= END_F; freq += STEP_F){
    Serial.println(freq);
    beep(freq, DURATION);    
    delay(GAP);
  }
}
