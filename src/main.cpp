#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include <SimpleTimer.h>

int tempo = 30;

SimpleTimer timer;
OneButton button1(D1, false);
Adafruit_NeoPixel pixels(72, 2, NEO_GRB + NEO_KHZ800);

void btnStartPause() {
  Serial.println("Play / Pause");
  pixels.clear();
  pixels.setPixelColor(20, pixels.Color(255,0, 0));
  pixels.show();
}

void btnStop_StartPress() {
  Serial.println("Play / Pause");
  pixels.clear();
  pixels.setPixelColor(20, pixels.Color(0,255, 0));
  pixels.show();
}

void btnStop() {
  Serial.println("Stop Pressed");
  pixels.clear();
  pixels.setPixelColor(20, pixels.Color(0,0, 255));
  pixels.show();
}

void repeatMe() {
    Serial.print("Uptime (s): ");
    Serial.println(millis() / 1000);
}


void setup() {
  Serial.begin(115200);
  pixels.begin();
  pixels.setBrightness(15);

  button1.attachClick(btnStartPause);
  button1.attachLongPressStart(btnStop_StartPress);
  button1.attachLongPressStop(btnStop);

  pixels.clear();
  pixels.show();
  timer.setInterval(1000, repeatMe);
}

void loop() {
  timer.run();
  button1.tick();
}