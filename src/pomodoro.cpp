#include "pomodoro.h"

Pomodoro::Pomodoro(unsigned int smallInterval, unsigned int bigInterval, unsigned int tempo) {
  this->smallInterval = smallInterval;
  this->bigInterval = bigInterval;
  this->tempo = tempo;

  /** ticker **/
  this->minutesTicker = new Ticker([this](){
    if(this->countdown > 0) {
      this->countdown--;
    }

    if (this->_on_tick_callback) {
      this->_on_tick_callback();
    }
  }, 1000, 0, MILLIS);
  this->minutesTicker->start();

  init();
}

Pomodoro::~Pomodoro() {

  delete this->minutesTicker;
}

void Pomodoro::init() {
  this->state = FSM_POMODORO_STOPPED;
  this->intervalState = FSM_WORKTIME;
  this->countdown = this->tempo;
}

void Pomodoro::update(bool btn_play, bool btn_pause, bool btn_stop) {

  if (this->_on_update_callback) {
    this->_on_update_callback(this->countdown);
  }

  if(this->minutesTicker && this->state == FSM_POMODORO_RUNNING) {
    this->minutesTicker->update();
  }
  if (this->state == FSM_POMODORO_RUNNING && btn_pause) {
    this->state = FSM_POMODORO_STOPPED;
  }
  if (this->state == FSM_POMODORO_RUNNING && btn_stop) {
    this->countdown = 0;
  }
  if (this->state == FSM_POMODORO_RUNNING && this->intervalState == FSM_WORKTIME && this->countdown <= 0){
    this->state = FSM_POMODORO_STOPPED;
    this->intervalState = FSM_INTERVAL;
    this->countdown = this->count > 0 && this->count % 4 == 0 ? this->bigInterval :this->smallInterval;
    this->count++;
    if (this->_on_finish_callback) {
      this->_on_finish_callback();
    }
  }
  if (this->state == FSM_POMODORO_RUNNING && this->intervalState == FSM_INTERVAL  && this->countdown <= 0){
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
  this->minutesTicker->start();
}

void Pomodoro::pause() {
  this->update(false, true, false);
  this->minutesTicker->pause();
  if (this->_on_pause_callback) {
    this->_on_pause_callback();
  }
}

void Pomodoro::stop() {
  this->update(false, false, true);
  this->minutesTicker->stop();
  if (this->_on_stop_callback) {
    this->_on_stop_callback();
  }
}

void Pomodoro::onStart(void (*callback)()) {
  this->_on_start_callback = callback;
}

void Pomodoro::onUpdate(void (*callback)(int)) {
  this->_on_update_callback = callback;
}

void Pomodoro::onFinish(void (*callback)()) {
  this->_on_finish_callback = callback;
}

void Pomodoro::onCount(void (*callback)()) {
  this->_on_count_callback = callback;
}

void Pomodoro::onTick(void (*callback)()) {
  this->_on_tick_callback = callback;
}

void Pomodoro::onPause(void (*callback)()) {
  this->_on_pause_callback = callback;
}

void Pomodoro::onResume(void (*callback)()) {
  this->_on_resume_callback = callback;
}

void Pomodoro::onStop(void (*callback)()) {
  this->_on_stop_callback = callback;
}

byte Pomodoro::getPomodoros() {
  return this->count;
}

unsigned int Pomodoro::getCountdown() {
  return this->countdown;
}


void Pomodoro::click() {
}

void Pomodoro::countdownFinished() {
}

enum STATES Pomodoro::getStates() {
  return this->state;
}
enum INTERVAL_STATE Pomodoro::getIState() {
  return this->intervalState;
}
