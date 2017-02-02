void fade(float rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) *= rate;
  }
}

void fade_fast(){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = *(p + i) >> 1;
  }
}

void fade_fast2(int rate = FADE_RATE){
  unsigned char *p;
  p = (unsigned char *)colors; 
  for(int i = 0; i < LED_COUNT * 3; i++){
    *(p + i) = max(0, *(p + i) - rate);
  }
}

float decay(float value){
  float orig = value;
  if(value >= 0){
    value -= DECAY;
    if(value < 0){
      value = random_dir(); 
    }
  } else {
    value += DECAY;
    if(value > 0){
      value = random_dir(); 
    }
  }
  return value;
}

void fade_down(){
  for(int i = 0; i < CHANGE_FADE; i++){
    fade();
    ledStrip.write(colors, LED_COUNT); 
    delay(CHANGE_FADE_DELAY);
  }

    for(int i = 0; i < MLED_COUNT; i++){
      existence[i] = 0;
  }
}

