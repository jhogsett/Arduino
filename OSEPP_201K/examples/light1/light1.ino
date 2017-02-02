

#define READ_PIN A0
#define LIGHT_PIN 13
#define SETTLE_TIME 1
#define WAIT_TIME 5

void setup()
{
    // set up serial at 9600 baud   
    Serial.begin(115200);
}

void loop()
{ 
    int analogValue;
    float temperature;

    digitalWrite(LIGHT_PIN, HIGH);
    delay(SETTLE_TIME);

    // read our temperature sensor
    analogValue = analogRead(READ_PIN);

    // print the temperature over serial
    Serial.print("Value: ");
    Serial.println(analogValue);
    
    // wait 1s before reading the temperature again
    delay(WAIT_TIME);

    digitalWrite(LIGHT_PIN, LOW);
    delay(SETTLE_TIME);

        // read our temperature sensor
    analogValue = analogRead(READ_PIN);

    // print the temperature over serial
    Serial.print("Value: ");
    Serial.println(analogValue);
    
    // wait 1s before reading the temperature again
    delay(WAIT_TIME);

}

