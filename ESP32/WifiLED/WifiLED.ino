/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is based on the Arduino WiFi Shield library, but has significant changes as newer WiFi functions are supported.
 *  E.g. the return value of `encryptionType()` different because more modern encryption is supported.
 */
#include "WiFi.h"
#include <string.h>
#include <Adafruit_NeoPixel.h> 

#define PIN 38
#define NUMPIXELS 1

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // Serial.begin(115200);

  pixels.begin();
  pixels.clear();

  // Set WiFi to station mode and disconnect from an AP if it was previously connected.
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Serial.println("Setup done");
}

void HSLtoRGB(int h, int s, int l, byte &r, byte &g, byte &b) {
  float hf = h / 360.0;
  float sf = s / 255.0;
  float lf = l / 255.0;

  float c = (1 - abs(2 * lf - 1)) * sf;
  float x = c * (1 - abs(fmod(hf * 6, 2) - 1));
  float m = lf - c / 2;

  float rf, gf, bf;
  if (hf < 1.0 / 6) { rf = c; gf = x; bf = 0; }
  else if (hf < 2.0 / 6) { rf = x; gf = c; bf = 0; }
  else if (hf < 3.0 / 6) { rf = 0; gf = c; bf = x; }
  else if (hf < 4.0 / 6) { rf = 0; gf = x; bf = c; }
  else if (hf < 5.0 / 6) { rf = x; gf = 0; bf = c; }
  else { rf = c; gf = 0; bf = x; }

  r = round((rf + m) * 255);
  g = round((gf + m) * 255);
  b = round((bf + m) * 255);
}

void loop() {
  // Serial.println("Scan start");

  // WiFi.scanNetworks will return the number of networks found.
  int n = WiFi.scanNetworks();
  // Serial.println("Scan done");
  // if (n == 0) {
  //   Serial.println("no networks found");
  // } else 
  {
    // Serial.print(n);
    // Serial.println(" networks found");
    // Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) {
      if(strcmp(WiFi.SSID(i).c_str(), "ChocolateCentral") == 0){

        long rssi = WiFi.RSSI(i);

        int irssi = int(rssi);

        // irssi -= 60;
        // Serial.println(irssi);


        // rssi -= 20L;

        // Serial.printf("%4ld", rssi); 
        // Serial.println();

        // Serial.println("----------");

        int dirssi = irssi * -2;
        // Serial.println(dirssi);

        if(dirssi < 0)
          dirssi = 0;

        if(dirssi > 255)
          dirssi = 255;

        // Serial.println("==========");
        // Serial.println(dirssi);

        int rdirssi = 255 - dirssi;

        // Serial.println(rdirssi);

        long hrdirssia = rdirssi * 360L;
        // Serial.println(hrdirssia);
        long hrdirssib = hrdirssia / 256L;
        // Serial.println(hrdirssib);
        int hrdirssi = int(hrdirssib);
        // Serial.println(hrdirssi);

        // int value = int(255 - (rssi * -2L));
        // Serial.println(value);

        // if(value < 0)
        //   value = 0;
        // if(value > 255);
        //   value = 255;

        // int a = value * 360;
        // Serial.println(a);
        // int b = a / 256;
        // Serial.println(b);

        // int hue = (value * 360) / 256;
        // Serial.println(hue);

        byte red, green, blue;
        // HSLtoRGB(hue, 255, 127, red, green, blue);
        HSLtoRGB(hrdirssi, 255, 12, red, green, blue);

        // Serial.printf("%d %d %d", red, green, blue);
        // Serial.println();

        // pixels.clear();
        pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        pixels.show();

        delay(50);

        pixels.setPixelColor(i, pixels.Color(red, green, blue));
        pixels.show();
      }
    }
  }
  // Serial.println("");

  // Delete the scan result to free memory for code below.
  WiFi.scanDelete();

  // Wait a bit before scanning again.
  // delay(1000);
}
