#include "pomodoro.h"

Pomodoro::Pomodoro(byte smallInterval, byte bigInterval, byte tempo) {
  this->smallInterval = smallInterval;
  this->bigInterval = bigInterval;
  this->tempo = tempo;

  /** ticker **/
  this->ticker = new Ticker([this](){
    if(this->countdown > 0) {
      this->countdown--;
    }
    if (this->_on_update_callback) {
      this->_on_update_callback(this->countdown);
    }
  }, _ONE_MINUTE, 0, MILLIS);
  this->ticker->start();

  init();
}

Pomodoro::~Pomodoro() {

  delete this->ticker;
}

void Pomodoro::init() {
  this->state = FSM_POMODORO_STOPPED;
  this->intervalState = FSM_INTERVAL;
  this->countdown = this->tempo;
}

void Pomodoro::update(bool btn_play, bool btn_pause, bool btn_stop) {
  if(this->ticker && this->state == FSM_POMODORO_RUNNING) {
    this->ticker->update();
  }
  if (this->state == FSM_POMODORO_RUNNING && btn_pause) {
    this->state = FSM_POMODORO_STOPPED;
  }
  if (this->state == FSM_POMODORO_RUNNING && btn_stop) {
    this->countdown = 0;
  }
  if (this->state == FSM_POMODORO_RUNNING && this->intervalState == FSM_WORKTIME && this->countdown <= 0){
    Serial.println("1");
    this->state = FSM_POMODORO_STOPPED;
    this->intervalState = FSM_INTERVAL;
    this->countdown = this->smallInterval;
    if (this->_on_finish_callback) {
      this->_on_finish_callback();
    }
  }
  if (this->state == FSM_POMODORO_RUNNING && this->intervalState == FSM_INTERVAL  && this->countdown <= 0){
    Serial.println("2");
    this->state = FSM_POMODORO_STOPPED;
    this->intervalState = FSM_WORKTIME;
    this->countdown = this->tempo;
    if (this->_on_finish_callback) {
      this->_on_finish_callback();
    }
  }
  if (this->state == FSM_POMODORO_STOPPED && btn_play){
    this->state = FSM_POMODORO_RUNNING;
    if (this->_on_finish_callback) {
      this->_on_start_callback();
    }
  }
}

void Pomodoro::start() {
  this->update(true, false, false);
}

void Pomodoro::pause() {
  this->update(false, true, false);
}

void Pomodoro::stop() {
  this->update(false, false, true);
}

void Pomodoro::onStart(void (*callback)())
{
  this->_on_start_callback = callback;
}

void Pomodoro::onUpdate(void (*callback)(int))
{
  this->_on_update_callback = callback;
}

void Pomodoro::onFinish(void (*callback)())
{
  this->_on_finish_callback = callback;
}

void Pomodoro::ledEffect(byte effectNumber) {
  Serial.print("ledEffect ");

  switch (effectNumber) {
    case 0 /* constant-expression */:

      break;
    case 1:
      Serial.println("1");

      break;
    default:
      break;
  }
}

byte Pomodoro::getPomodoros() {
  return this->count;
}

byte Pomodoro::getCountdown() {
  return this->countdown;
}


void Pomodoro::click() {
  this->ledEffect(0);
}

void Pomodoro::countdownFinished() {
  this->ledEffect(1);
}

enum STATES Pomodoro::getStates() {
  return this->state;
}
enum INTERVAL_STATE Pomodoro::getIState() {
  return this->intervalState;
}
