#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>


class Pomodoro {

  private:
    byte ledPin;   // pin to control leds ws2812
    byte ledCount; // number of leds
    byte smallInterval;
    byte bigInterval;
    byte tempo;
    byte count;

  public:
    Pomodoro(byte ledPin,byte ledCount, byte smallInterval = 5, byte bigInterval = 15, byte tempo = 25);
    void init();
    void update();
    void start();
    void pause();
    void stop();
    byte getPomodoros();

};

#endif