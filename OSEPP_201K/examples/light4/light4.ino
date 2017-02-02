

#define READ_PIN A0
#define LIGHT_PIN 13
#define SETTLE_TIME 50
#define WAIT_TIME 50

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
  
    int analogValue1, analogValue2;

    digitalWrite(LIGHT_PIN, LOW);
    delay(SETTLE_TIME);

    analogValue1 = analogRead(READ_PIN);
    digitalWrite(LIGHT_PIN, HIGH);

    //Serial.print("Value: ");
    //Serial.println(analogValue);

    delay(WAIT_TIME);

    analogValue2 = analogRead(READ_PIN);

    Serial.print("Value: ");
    Serial.println(analogValue1 - analogValue2);

}

