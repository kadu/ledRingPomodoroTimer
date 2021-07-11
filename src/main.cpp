#include <Arduino.h>
#include <OneButton.h>
#include <pomodoro.h>
#include <Ticker.h>
#include <ws2812fx.h>

#define POMODOROINTERVAL    5
#define POMODOROTIMER       25
#define POMODOROBIGINTERVAL 15
#define _MINUTE_IN_MS       1000
#define LEDCOUNT            16

unsigned int contapomodoro = 0;
byte c = 16;
byte NEOPIXELPIN= 4;

OneButton button1(D1, false);
Pomodoro pomodoro(POMODOROINTERVAL, POMODOROBIGINTERVAL, POMODOROTIMER);
WS2812FX ws2812fx = WS2812FX(LEDCOUNT, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
void printCounter();

void setup() {

  Serial.begin(115200);

  ws2812fx.init();
  ws2812fx.setBrightness(1);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
  ws2812fx.start();

  button1.attachClick([](){
    pomodoro.start();
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
    printCounter();

    int maxTempo = pomodoro.getIState() == FSM_INTERVAL ? POMODOROINTERVAL : POMODOROTIMER;
    int mapa = map(countdown, 0, maxTempo, 0, LEDCOUNT);
    uint32_t cor;

    if(pomodoro.getStates() == FSM_POMODORO_RUNNING) {
      switch (pomodoro.getIState()) {
      case FSM_INTERVAL:
        cor = BLUE;
        break;
      case FSM_WORKTIME:
        cor = RED;
        break;
      default:
        cor = ORANGE;
        break;
      }
    } else {
      cor = YELLOW;
    }

    Serial.println(mapa);
    ws2812fx.stop();
    ws2812fx.clear();
    ws2812fx.fill(cor, 0, mapa+1);
    ws2812fx.show();
  });

  pomodoro.onFinish([](){
    Serial.println("OnFinish");
    Serial.printf("Pomodoros: %02d", pomodoro.getPomodoros());
    printCounter();
    uint32_t cor;

    switch (pomodoro.getIState()) {
    case FSM_INTERVAL:
      cor = BLUE;
      break;
    case FSM_WORKTIME:
      cor = RED;
      break;
    default:
      cor = ORANGE;
      break;
    }

    ws2812fx.stop();
    ws2812fx.clear();
    ws2812fx.fill(cor, 0, LEDCOUNT);

    if(pomodoro.getStates() != FSM_POMODORO_RUNNING) {
      cor = YELLOW;
      ws2812fx.fill(cor, LEDCOUNT/2, LEDCOUNT);
    }
    ws2812fx.show();

  });
}

void loop() {
  button1.tick();
  pomodoro.update();
  ws2812fx.service();
}

void printCounter() {
  Serial.printf("Countdown [%02d] - State [%s] - Interval [%s]\n",
    pomodoro.getCountdown(),
    pomodoro.getStates() == FSM_POMODORO_RUNNING ? "Running": "Stopped",
    pomodoro.getIState() ==  FSM_INTERVAL ? "Interval": "Worktime"
  );
}