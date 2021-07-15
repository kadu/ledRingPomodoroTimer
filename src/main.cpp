#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFiUdp.h>
#include <OneButton.h>
#include <ws2812fx.h>
#include <ArduinoOTA.h>
#include <Ticker.h>
#include <pomodoro.h>

#define POMODOROINTERVAL    5
#define POMODOROTIMER       25
#define POMODOROBIGINTERVAL 15
#define _MINUTE_IN_MS       60000
#define LEDCOUNT            16

unsigned int contapomodoro = 0;
byte c = 16;
byte NEOPIXELPIN= 4;

OneButton button1(D1, false);
Pomodoro pomodoro(POMODOROINTERVAL, POMODOROBIGINTERVAL, POMODOROTIMER);
WS2812FX ws2812fx = WS2812FX(LEDCOUNT, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin("...", "...");
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void printCounter() {
  Serial.printf("Countdown [%02d] - State [%s] - Interval [%s]\n",
    pomodoro.getCountdown(),
    pomodoro.getStates() == FSM_POMODORO_RUNNING ? "Running": "Stopped",
    pomodoro.getIState() ==  FSM_INTERVAL ? "Interval": "Worktime"
  );
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void setupOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting...");

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

  button1.attachLongPressStart([](){
    pomodoro.stop();
  });

  pomodoro.onStart([](){
    Serial.println("onStart");
  });

  pomodoro.onCount([]() {
    Serial.println("onCount");
  });

  pomodoro.onPause([](){
    Serial.println("onPause");
  });

  pomodoro.onResume([](){
    Serial.println("onResume");
  });

  pomodoro.onTick([](){
    Serial.println("onTick");
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

  //wifi
  initWiFi();
  setupOTA();

  // webserver
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Hello, world");
  });
  server.onNotFound(notFound);
  server.begin();
}

void loop() {
  ArduinoOTA.handle();
  ws2812fx.service();
  button1.tick();
  pomodoro.update();
}