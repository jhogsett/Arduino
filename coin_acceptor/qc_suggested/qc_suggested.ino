	#include <Arduino.h>
	#include <EEPROM.h>
	

	const int coinPin = 2;
	volatile int impulsCount = 0;
	int coinType = 0;
	float totalAmount = 0.0;
	float coinValue;
	

	const int coinPulses[] = {2, 1, 5, 10, 25};
	const float coinValues[] = {1.0, 0.01, 0.05, 0.1, 0.25};
	

	int lookup(int pulses) {
	  for (int i = 0; i < sizeof(coinPulses) / sizeof(coinPulses[0]); i++) {
	    if (pulses == coinPulses[i]) {
	      return coinValues[i];
	    }
	  }
	  return -1;
	}
	
	void setup() {
	  Serial.begin(9600);
	  attachInterrupt(digitalPinToInterrupt(coinPin), coinInterrupt, FALLING);
	  totalAmount = 0.0;
	  EEPROM.write(0, 0);
	}
	

	void coinInterrupt() {
	  impulsCount++;
	}
	

	void loop() {
	  if (impulsCount == coinPulses[coinType]) {
	    coinValue = lookup(impulsCount);
	    if (coinValue > 0) {
	      totalAmount += coinValue;
	    }
	    delay(100);
	    Serial.print("Total Amount: ");
	    Serial.println(totalAmount, 2);
	    impulsCount = 0;
	  }
	}
