// ask_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_ASK driver to control a ASK radio.
// It is designed to work with the other example ask_reliable_datagram_server
// Tested on Arduino Mega, Duemilanova, Uno, Due, Teensy, ESP-12
 
// #include <Wire.h>
// #include <AHT20.h>

#include <RHReliableDatagram.h>
#include <RH_ASK.h>
#include <SPI.h>

// AHT20 aht20;

// #define PAIR1
// #define PAIR2
// #define PAIR3
#define PAIR4

// range finder/counter unit
#ifdef PAIR1
#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2
#endif

// temp display #1 garage
#ifdef PAIR2
#define CLIENT_ADDRESS 3
#define SERVER_ADDRESS 4
#endif

// temp display #2 backroom or shed
#ifdef PAIR3
#define CLIENT_ADDRESS 5
#define SERVER_ADDRESS 6
#endif

// garage door status
#ifdef PAIR4
#define CLIENT_ADDRESS 7
#define SERVER_ADDRESS 8
#endif

#define DATARATE 480
#define RETRIES 3
#define TIMEOUT 1000
#define WAITDEL 30000L
// 30000

// Singleton instance of the radio driver
RH_ASK driver(DATARATE, 11, 12, 10, false);
// RH_ASK driver(2000, 4, 5, 0); // ESP8266 or ESP32: do not use pin 11 or 2
// RH_ASK driver(2000, PD14, PD13, 0); STM32F4 Discovery: see tx and rx on Orange and Red LEDS
 
// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);
 
// #define PTT_PIN 10

constexpr int SENSOR_PIN = 3;

constexpr int BUZZER_PIN = 2;
constexpr int BUZZ_TIMES_OPENED = 3;
constexpr int BUZZ_WAIT_OPENED = 12500;
constexpr int BUZZ_TIMES_CLOSED = 7;
constexpr int BUZZ_WAIT_CLOSED = 1500;
constexpr int BUZZ_INT = 100;

bool last_state = false;

void buzz_change(){
  digitalWrite(BUZZER_PIN, HIGH);
  delay(BUZZ_INT);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzz_opened(){
  // digitalWrite(BUZZER_PIN, HIGH);
  // delay(BUZZ_INT);
  // digitalWrite(BUZZER_PIN, LOW);
  // delay(BUZZ_INT);

  delay(BUZZ_WAIT_OPENED);

  for(int i = 0; i < BUZZ_TIMES_OPENED; i++){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZ_INT);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZ_INT);
  }
}

void buzz_closed(){
  // digitalWrite(BUZZER_PIN, HIGH);
  // delay(BUZZ_INT);
  // digitalWrite(BUZZER_PIN, LOW);
  // delay(BUZZ_INT);

  delay(BUZZ_WAIT_CLOSED);

  for(int i = 0; i < BUZZ_TIMES_CLOSED; i++){
    digitalWrite(BUZZER_PIN, HIGH);
    delay(BUZZ_INT);
    digitalWrite(BUZZER_PIN, LOW);
    delay(BUZZ_INT);
  }
}

uint32_t start_time = 0L;

 
// float sample_temp(){
//   float temp_c, temp_f;
//   temp_c = aht20.getTemperature();
//   temp_f = temp_c * (9.0 / 5.0) + 32.0;
//   return temp_f;
// }

// float sample_humid(){
//   float humid;
//   humid = aht20.getHumidity();
//   return humid;
// }

#define DATASIZE 1
uint8_t data[DATASIZE]; // = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_ASK_MAX_MESSAGE_LEN];

int msgcount = 0;
int ackfailcount = 0;
int sendfailcount = 0;
 
void transmit_state(bool state){
  memcpy((void*)data, (const void *)&state, sizeof(state));
  // memcpy((void*)data + sizeof(temp), (const void *)&humid, sizeof(humid));

  // Send a message to manager_server
  if (manager.sendtoWait(data, DATASIZE, SERVER_ADDRESS))
  {

    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      msgcount++;
  
      // Serial.print("got reply from : 0x");
      // Serial.print(from, HEX);
      // Serial.print(": ");
      // Serial.println((char*)buf);
      // Serial.print("Resends: ");
      // Serial.println(manager.retransmissions());
      char buf[20];
      sprintf(buf, "%d A:%d S:%d R:%d", msgcount, ackfailcount, sendfailcount, manager.retransmissions());
      Serial.println(buf);
    }
    else
    {
      // Serial.println("No reply, is ask_reliable_datagram_server running?");
      ackfailcount++;
    }
  }
  else {
    // Serial.println("sendtoWait failed");
    sendfailcount++;
  }
  // delay(WAITDEL);

}

void setup() 
{
  Serial.begin(9600);

  // pinMode(PTT_PIN, OUTPUT);

  if (!manager.init())
    Serial.println("init failed");

  manager.setRetries(10);
  manager.setTimeout(TIMEOUT);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, INPUT);

  last_state = digitalRead(SENSOR_PIN);

  transmit_state(last_state);

  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);

  start_time = millis();

  // Wire.begin();
  // if (aht20.begin() == false)
  // {
  //   Serial.println("AHT20 not detected. Please check wiring. Freezing.");
  //   while(true);
  // }
}

void loop()
{
  // Serial.println("start");

  // float temp = sample_temp();
  // float humid = sample_humid();

  bool current_state = digitalRead(SENSOR_PIN);

  if(current_state != last_state){
    last_state = current_state;

    buzz_change();

    transmit_state(current_state);
    start_time = millis();

    switch(current_state){
      case LOW:
        buzz_closed();
        break;
      case HIGH:
        buzz_opened();
        break;
    }
    // transmit_state(current_state);
    // start_time = millis();
  }

  Serial.print("State: ");
  Serial.println(current_state == LOW ? "Closed" : "Opened");

  // Serial.print("Humid: ");
  // Serial.println(humid);

  // Serial.println("Sending to ask_reliable_datagram_server");

  // sprintf(data, "%ld", millis());
  // int len = strlen(data);

  if(millis() - start_time > WAITDEL){
    start_time = millis();
    transmit_state(current_state);
  }

  delay(1000);
}
