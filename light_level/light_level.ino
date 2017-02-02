

#define READ_PIN A0
#define LIGHT_PIN 13
#define SETTLE_TIME 50
#define WAIT_TIME 50

#define MAX_DIFF 3
#define BUZZER_PIN 4
#define BUZZER_FREQ 440
#define BUZZER_TIME 50

void setup()
{
    // set up serial at 9600 baud   
    Serial.begin(115200);
}

void loop()
{ 
//    digitalWrite(8, HIGH);
//    delay(1000);
//    digitalWrite(8, LOW);
//    delay(1000);

    int previous;
    int diff;
    int analogValue1, analogValue2;

    while(true){
      //digitalWrite(LIGHT_PIN, LOW);
      //delay(SETTLE_TIME);
  
      analogValue1 = analogRead(READ_PIN);
      //digitalWrite(LIGHT_PIN, HIGH);
  
      Serial.print("Value: ");
      Serial.println(analogValue1);
  
      delay(WAIT_TIME);
  
      //analogValue2 = analogRead(READ_PIN);
  
      //diff = abs(analogValue1 - analogValue2);
  
      //int diffdiff = abs(previous - diff);
     
      //Serial.print("dd: ");
      //Serial.println(diffdiff);
      //previous = diff;

      //if(diffdiff > MAX_DIFF){
      //  tone(BUZZER_PIN, BUZZER_FREQ, BUZZER_TIME);        
      }
      
}    



