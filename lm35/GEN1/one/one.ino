const int lm35_pin = A7;	/* LM35 O/P pin */

void setup() {
  Serial.begin(115200);
}

void loop() {
  int temp_adc_val;
  float temp_val, temp_c, temp_f;
  temp_adc_val = analogRead(lm35_pin);
  temp_val = (temp_adc_val * 4.88);	/* Convert adc value to equivalent voltage */
  temp_c = (temp_val / 10.0);	/* LM35 gives output of 10mv/°C */
  temp_f = temp_c * (9.0 / 5.0) + 32.0;
  Serial.print("Temperature = ");
  Serial.println(temp_f);
  // Serial.print(" Degree Celsius\n");
  delay(1000);
}