#include <Arduino.h>
#include <OneButton.h>
#include <pomodoro.h>
#include <Ticker.h>

#define POMODOROINTERVAL    5
#define POMODOROTIMER       2
#define POMODOROBIGINTERVAL 15
#define _MINUTE_IN_MS       1000

unsigned int contapomodoro = 0;
byte LEDCOUNT = 16;
byte NEOPIXELPIN= 4;

OneButton button1(D1, false);
Pomodoro pomodoro(NEOPIXELPIN, LEDCOUNT, POMODOROINTERVAL, POMODOROBIGINTERVAL, POMODOROTIMER);
void printCounter();

Ticker timer2(printCounter, 1000, 0, MILLIS);

void btnStartPause() {
  Serial.println("Play / Pause");
  pomodoro.start();

  contapomodoro = 25;
}

void btnStop_StartPress() {
  Serial.println("Play / Pause");
  pomodoro.start();
}

void btnStop() {
  Serial.println("Stop Pressed");
}

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

  button1.attachClick(btnStartPause);
  button1.attachLongPressStart(btnStop_StartPress);
  button1.attachLongPressStop(btnStop);

  timer2.start();

  // timer.setInterval(_MINUTE_IN_MS, minutosTick);
  // timer.setInterval(1000, debugSegundo);
}

void loop() {
  // timer.run();
  button1.tick();
  pomodoro.update();

  timer2.update();
}

void printCounter() {
  Serial.print("Counter ");
  Serial.println(pomodoro.getCountdown());
}