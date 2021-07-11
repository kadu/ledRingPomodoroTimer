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
    byte smallInterval;
    byte bigInterval;
    byte tempo;    // Pomodoro Time
    byte count;    // numero de pomodoros feitos (sera reiniciado quando apertar o STOPPP)
    byte countdown = 0;
    bool isTimerRunning = false;
    bool botaoApertado = false;
    int  minutesTimer;
    void (*_on_start_callback)();
    void (*_on_update_callback)(int);
    void (*_on_finish_callback)();
    enum STATES state = FSM_POMODORO_STOPPED;
    enum INTERVAL_STATE intervalState = FSM_INTERVAL;

    void countdownFinished();
    void changeIntervalState(enum INTERVAL_STATE newState);
    void changePomodoroState(enum STATES newState);

  public:
    Pomodoro(byte smallInterval = 5, byte bigInterval = 15, byte tempo = 25);
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

    byte getPomodoros();
    byte getCountdown();
    enum STATES getStates();
    enum INTERVAL_STATE getIState();

    Ticker *ticker = NULL;
};

#endif