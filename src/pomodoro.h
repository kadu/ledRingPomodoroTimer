#ifndef POMODORO_H
#define POMODORO_H

#include <Arduino.h>
#include <Ticker.h>

enum STATES {
  FSM_POMODORO_STOPPED,
  FSM_POMODORO_RUNNING
};

enum INTERVAL_STATE {
  FSM_INTERVAL,
  FSM_WORKTIME
};

class Pomodoro {

  private:

    byte count;    // numero de pomodoros feitos (sera reiniciado quando apertar o STOPPP)
    unsigned int  countdown = 0;
    bool isTimerRunning = false;
    bool botaoApertado = false;
    int  minutesTimer;
    void (*_on_start_callback)();
    void (*_on_update_callback)(int);
    void (*_on_finish_callback)();
    void (*_on_resume_callback)();
    void (*_on_count_callback)();
    void (*_on_tick_callback)();
    void (*_on_pause_callback)();
    void (*_on_stop_callback)();

    enum STATES state = FSM_POMODORO_STOPPED;
    enum INTERVAL_STATE intervalState = FSM_INTERVAL;

    void countdownFinished();
    void changeIntervalState(enum INTERVAL_STATE newState);
    void changePomodoroState(enum STATES newState);

  public:
    unsigned int smallInterval;
    unsigned int bigInterval;
    unsigned int tempo;    // Pomodoro Time
    Pomodoro(unsigned int smallInterval = 300, unsigned int bigInterval = 900, unsigned int tempo = 1500);
    ~Pomodoro();
    void init();
    void update(bool btn_play = false, bool btn_pause = false, bool btn_stop = false);
    void start();
    void pause();
    void stop();
    void click();
    void onStart(void (*callback)());
    void onUpdate(void (*callback)(int));
    void onFinish(void (*callback)());
    void onResume(void (*callback)());
    void onCount(void (*callback)());
    void onTick(void (*callback)());
    void onPause(void (*callback)());
    void onStop(void (*callback)());

    byte getPomodoros();
    unsigned int getCountdown();
    enum STATES getStates();
    enum INTERVAL_STATE getIState();

    Ticker *minutesTicker = NULL;
};

#endif