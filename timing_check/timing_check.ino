



byte timer_hour = 0, timer_minute = 0, timer_second = 0;
unsigned long time = 0;
unsigned long next_second = 0;



// void render_timer_string(byte seconds, byte minutes, byte hours, bool running) {
// 	char indicator[5];
// 	if (running)
//         load_f_string(F("STOP"), indicator);
// 	else
//         load_f_string(F("RUN "), indicator);

// 	if (timer_hour < 1)
// 		sprintf_P(display_buffer, PSTR("%s %02d %02d  "), indicator, timer_minute, timer_second);
// 	else {
// 		sprintf_P(display_buffer, PSTR("%s %02d. %02d  "), indicator, timer_hour, timer_minute);
// 	}
// }

long time_to_seconds(byte second, byte minute, byte hour) {
	long result = (long)second + (60L * (long)minute) + (3600L * (long)hour);
	return result;
}

void seconds_to_time(long seconds, byte &second, byte &minute, byte &hour) {
	hour = seconds / 3600L;
	seconds -= hour * 3600L;
	minute = seconds / 60L;
	seconds -= minute * 60L;
	second = seconds;
}

void increment_timer(byte &second, byte &minute, byte &hour, byte seconds=1, byte minutes=0, byte hours=0) {
	long total_seconds = time_to_seconds(second, minute, hour);
	total_seconds += time_to_seconds(seconds, minutes, hours);
	seconds_to_time(total_seconds, second, minute, hour);
}



void setup() {
  Serial.begin(115200);
  Serial.println("ready");
}

void wait_for_press(){
  char buffer[10];
  byte read = 0;
  while((read = Serial.readBytesUntil('\n', buffer, 9)) == 0)
    ;
}

void loop() {
  wait_for_press();
  
  time = millis();
  next_second = time + 1000;
  
  while(true){
    time = millis();
		if(time >= next_second) {
  		increment_timer(timer_second, timer_minute, timer_hour);

      char buffer[20];
      sprintf(buffer, "%d:%02d:%02d", timer_hour, timer_minute, timer_second);
      Serial.println(buffer);
      
			next_second = time + 1000;
    }
  }
}