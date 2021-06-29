#include "pomodoro.h"

Pomodoro::Pomodoro(byte ledPin,byte ledCount, byte smallInterval, byte bigInterval, byte tempo) {
  this->ledPin = ledPin;
  this->ledCount = ledCount;
  this->smallInterval = smallInterval;
  this->bigInterval = bigInterval;
  this->tempo = tempo;
  init();
}

void Pomodoro::init() {
  //
}

void Pomodoro::update() {
  //
}

void Pomodoro::start() {

}

void Pomodoro::pause() {

}

void Pomodoro::stop() {

}

byte Pomodoro::getPomodoros() {
  return this->count;
}