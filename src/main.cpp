#include <Arduino.h>
#include <OneButton.h>
#include <pomodoro.h>
#include <Ticker.h>

#define POMODOROINTERVAL    5
#define POMODOROTIMER       25
#define POMODOROBIGINTERVAL 15
#define _MINUTE_IN_MS       1000

unsigned int contapomodoro = 0;
byte LEDCOUNT = 16;
byte NEOPIXELPIN= 4;

OneButton button1(D1, false);
Pomodoro pomodoro(NEOPIXELPIN, LEDCOUNT, POMODOROINTERVAL, POMODOROBIGINTERVAL, POMODOROTIMER);

void printCounter();

Ticker timer2(printCounter, 1000, 0, MILLIS);

void minutosTick() {
  if(contapomodoro > 0) {
    contapomodoro--;
  }

  // pixels.clear();
  // uint32_t cor;
  // int mapa = map(contapomodoro, 25,0,LEDCOUNT,0);

  // if(contapomodoro > 5) {
  //   cor = azul;
  // } else {
  //   cor = vermelho;
  // }

  // for (int i = 0; i < mapa; i++) {
  //   pixels.setPixelColor(i,cor);
  // }
  // pixels.show();
}

void debugSegundo() {
  Serial.println(contapomodoro);
}

void setup() {
  Serial.begin(115200);
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

  timer2.start();

  // timer.setInterval(_MINUTE_IN_MS, minutosTick);
  // timer.setInterval(1000, debugSegundo);
}

void loop() {
  // timer.run();
  button1.tick();
  pomodoro.update();
  //ws2812fx.update();
  timer2.update();
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

///