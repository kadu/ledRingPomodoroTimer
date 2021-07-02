#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>
#include <WS2812FX.h>
#include <Ticker.h>

#define _ONE_MINUTE 60000
class Pomodoro {

  private:
    byte ledPin;   // pin to control leds ws2812
    byte ledCount; // number of leds
    byte smallInterval;
    byte count;    // numero de pomodoros feitos (sera reiniciado quando apertar o STOPPP)
    byte bigInterval;
    byte tempo;
    byte countdown = 0;
    int  minutesTimer;
    WS2812FX *ws2812fx;

  public:
    Pomodoro(byte ledPin,byte ledCount, byte smallInterval = 5, byte bigInterval = 15, byte tempo = 25);
    ~Pomodoro();
    void init();
    void update();
    void start();
    void pause();
    void stop();
    byte getPomodoros();
    byte getCountdown();
    Ticker *ticker = NULL;
};

#endif