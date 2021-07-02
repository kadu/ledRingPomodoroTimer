#include "pomodoro.h"

Pomodoro::Pomodoro(byte ledPin,byte ledCount, byte smallInterval, byte bigInterval, byte tempo) {
  this->ledPin = ledPin;
  this->ledCount = ledCount;
  this->smallInterval = smallInterval;
  this->bigInterval = bigInterval;
  this->tempo = tempo;
  init();
}

Pomodoro::~Pomodoro() {
  delete this->ws2812fx;
}

void Pomodoro::init() {
  this->ws2812fx = new WS2812FX(this->ledCount, this->ledPin, NEO_RGB + NEO_KHZ800);
  this->ws2812fx->init();
  this->ws2812fx->setBrightness(20);
  this->ws2812fx->setSpeed(1000);
  this->ws2812fx->setColor(0xFF0000);
  this->ws2812fx->setMode(FX_MODE_BLINK);
  this->ws2812fx->start();
}

void Pomodoro::update() {
  this->ws2812fx->service();
  if(this->ticker) {
    this->ticker->update();
  }

  int mapa = map(this->countdown, 0, this->bigInterval, 0, this->ledCount);
  this->ws2812fx->clear();
  this->ws2812fx->fill(RED, 0, mapa);
  this->ws2812fx->show();
}

void Pomodoro::start() {
  if(this->count == 0) {
    this->countdown = this->bigInterval;
    this->ws2812fx->stop();
    this->ws2812fx->clear();
    this->ws2812fx->setPixelColor(0, YELLOW);
    this->ws2812fx->show();
    this->ticker = new Ticker([this](){
      if(this->countdown > 0) {
        this->countdown--;
      }
    }, _ONE_MINUTE, 0, MILLIS);
    this->ticker->start();

    Serial.println(this->minutesTimer);
  }
}

void Pomodoro::pause() {

}

void Pomodoro::stop() {

}

byte Pomodoro::getPomodoros() {
  return this->count;
}

byte Pomodoro::getCountdown() {
  return this->countdown;
}