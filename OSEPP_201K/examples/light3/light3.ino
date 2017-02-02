

#define READ_PIN A0
#define LIGHT_PIN 13
#define SETTLE_TIME 5
#define WAIT_TIME 5

void setup()
{
    // set up serial at 9600 baud   
    Serial.begin(115200);
}

void loop()
{ 
    int analogValue;

    digitalWrite(LIGHT_PIN, LOW);
    delay(SETTLE_TIME);

    analogValue = analogRead(READ_PIN);
    digitalWrite(LIGHT_PIN, HIGH);

    Serial.print("Value: ");
    Serial.println(analogValue);

    delay(WAIT_TIME);

    analogValue = analogRead(READ_PIN);

    Serial.print("Value: ");
    Serial.println(analogValue);

}

