#include <Arduino.h>
#include <OneButton.h>
#include <pomodoro.h>
#include <Ticker.h>
#include <ws2812fx.h>

#define POMODOROINTERVAL    5
#define POMODOROTIMER       25
#define POMODOROBIGINTERVAL 15
#define _MINUTE_IN_MS       1000

unsigned int contapomodoro = 0;
byte LEDCOUNT = 16;
byte NEOPIXELPIN= 4;

OneButton button1(D1, false);
Pomodoro pomodoro(POMODOROINTERVAL, POMODOROBIGINTERVAL, POMODOROTIMER);
WS2812FX ws2812fx = WS2812FX(LEDCOUNT, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);

void printCounter();

Ticker timer(printCounter, 1000, 0, MILLIS);

void setup() {

  Serial.begin(115200);

  ws2812fx.init();
  ws2812fx.setBrightness(5);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
  ws2812fx.start();

  button1.attachClick([](){
    pomodoro.start();
    /*for (size_t i = 0; i < 1; i++) {
        this->ws2812fx->stop();
        this->ws2812fx->clear();
        this->ws2812fx->fill(BLUE,0, this->ledCount);
        this->ws2812fx->show();
        delay(100);
        this->ws2812fx->stop();
        this->ws2812fx->clear();
        this->ws2812fx->show();
        delay(100);
      }*/
  });
  button1.attachDoubleClick([](){
    pomodoro.pause();
  });
  button1.attachLongPressStop([](){
    pomodoro.stop();
  });

  pomodoro.onStart([](){
    Serial.println("onStart");

  });
  pomodoro.onUpdate([](int countdown){
    Serial.println("onUpdate");
    Serial.println(countdown);
  });
  pomodoro.onFinish([](){
    Serial.println("OnFinish");
  });

  timer.start();
}

void loop() {
  button1.tick();
  pomodoro.update();
  ws2812fx.service();
  timer.update();
}

void printCounter() {
  Serial.print("Countdown [");
  Serial.print(pomodoro.getCountdown());
  Serial.print("] - State [");
  Serial.print(pomodoro.getStates() == FSM_POMODORO_RUNNING ? "Running": "Stopped");
  Serial.print("] - Interval [");
  Serial.print(pomodoro.getIState() ==  FSM_INTERVAL ? "Interval": "Worktime");
  Serial.println("]");
}