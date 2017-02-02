/* Ping))) Sensor

   This sketch reads a PING))) ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse
   to return.  The length of the returning pulse is proportional to
   the distance of the object from the sensor.

   The circuit:
	* +V connection of the PING))) attached to +5V
	* GND connection of the PING))) attached to ground
	* SIG connection of the PING))) attached to digital pin 7

   http://www.arduino.cc/en/Tutorial/Ping

   created 3 Nov 2008
   by David A. Mellis
   modified 30 Aug 2011
   by Tom Igoe

   This example code is in the public domain.

 */

// this constant won't change.  It's the pin number
// of the sensor's output:
const int pingPin = 7;

void setup() {
  // initialize serial communication:
  Serial.begin(115200);
}

#define LED_RED 9
#define LED_GREEN 10
#define LED_BLUE 3

#define MIN_DUR 140.0
#define MAX_DUR 19800.0
#define DUR_RANGE (MAX_DUR - MIN_DUR)
#define BRIGHTNESS 50

void loop() {
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT);
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(pingPin, INPUT);
  duration = pulseIn(pingPin, HIGH);

  int color = ((duration - MIN_DUR) / DUR_RANGE) * 255;

  Serial.print(color);
  
  setLED(color, BRIGHTNESS);
  delay(100);

  
//  setLED(0, BRIGHTNESS);

//  ledWrite(255,0,0);
//  delay(1000);
//  ledWrite(0,255,0);
//  delay(1000);
//  ledWrite(0,0,255);
//  delay(1000);

//  for(int i = 0; i <= 259; i++){
//    setLED(i, BRIGHTNESS);
//    delay(10);
//  }

//  pinMode(LED_RED, OUTPUT);
//  digitalWrite(LED_RED, HIGH);
//  delay(1000);
//  digitalWrite(LED_RED, LOW);
//
//  analogWrite(LED_RED, 63);
//  delay(1000);
//  analogWrite(LED_RED, 0);
//  delay(1000);
//  
//
//  pinMode(LED_GREEN, OUTPUT);
//  digitalWrite(LED_GREEN, HIGH);
//  delay(1000);
//  digitalWrite(LED_GREEN, LOW);
//
//  analogWrite(LED_GREEN, 63);
//  delay(1000);
//  analogWrite(LED_GREEN, 0);
//  delay(1000);
//  
//  pinMode(LED_BLUE, OUTPUT);
//  digitalWrite(LED_BLUE, HIGH);
//  delay(1000);
//  digitalWrite(LED_BLUE, LOW);
//
//  analogWrite(LED_BLUE, 63);
//  delay(1000);
//  analogWrite(LED_BLUE, 0);
//  delay(1000);
//
//  analogWrite(LED_RED, 63);
//  analogWrite(LED_GREEN, 63);
//  analogWrite(LED_BLUE, 63);
//  delay(1000);
//  analogWrite(LED_RED, 0);
//  analogWrite(LED_GREEN, 0);
//  analogWrite(LED_BLUE, 0);
//  delay(1000);
//


/*
  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  Serial.print(duration);
  Serial.print(" dur, ");
  Serial.print(inches);
  Serial.print(" in, ");
  Serial.print(cm);
  Serial.print(" cm");
  Serial.println();

  delay(100);
*/  
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}



void setLED(int hue, int l){
  int col[3] = {0,0,0};
  getRGB(hue, 255, l, col);
  ledWrite(col[0], col[1], col[2]);
}

void getRGB(int hue, int sat, int val, int colors[3]) {
  // hue: 0-259, sat: 0-255, val (lightness): 0-255
  int r, g, b, base;

  if (sat == 0) { // Achromatic color (gray).
    colors[0]=val;
    colors[1]=val;
    colors[2]=val;
  } else  {
    base = ((255 - sat) * val)>>8;
    switch(hue/60) {
      case 0:
        r = val;
        g = (((val-base)*hue)/60)+base;
        b = base;
        break;
      case 1:
        r = (((val-base)*(60-(hue%60)))/60)+base;
        g = val;
        b = base;
        break;
      case 2:
        r = base;
        g = val;
        b = (((val-base)*(hue%60))/60)+base;
        break;
      case 3:
        r = base;
        g = (((val-base)*(60-(hue%60)))/60)+base;
        b = val;
        break;
      case 4:
        r = (((val-base)*(hue%60))/60)+base;
        g = base;
        b = val;
        break;
      case 5:
        r = val;
        g = base;
        b = (((val-base)*(60-(hue%60)))/60)+base;
        break;
    }
    colors[0]=r;
    colors[1]=g;
    colors[2]=b;
 
  }
}


void ledWrite(int r, int g, int b){
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}
