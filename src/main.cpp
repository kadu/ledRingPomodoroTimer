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
#include <secrets.h>

#define POMODOROTIMER       16   // segundos
#define POMODOROINTERVAL    8    // segundos
#define POMODOROBIGINTERVAL 16    // segundos

#define _MINUTE_IN_MS       60000

#define LEDCOUNT            16
#define BUZZPIN             D6

unsigned int frequency = 1000;
unsigned int beeps = 10;

unsigned int contapomodoro = 0;
byte c = 16;
byte NEOPIXELPIN= 4;
bool isEffectRunning = false;

OneButton button1(D1, false);
Pomodoro pomodoro(POMODOROINTERVAL, POMODOROBIGINTERVAL, POMODOROTIMER);
WS2812FX ws2812fx = WS2812FX(LEDCOUNT, NEOPIXELPIN, NEO_GRB + NEO_KHZ800);
AsyncWebServer server(80);

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
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
  Serial.printf("Countdown [%02d secs] - State [%s] - Interval [%s]\n",
    pomodoro.getCountdown(),
    pomodoro.getStates() == FSM_POMODORO_RUNNING ? "Running": "Stopped",
    pomodoro.getIState() ==  FSM_INTERVAL ? "Interval": "Worktime"
  );
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void clickEffect() {
  ws2812fx.stop();
  ws2812fx.clear();
  ws2812fx.fill(WHITE,0,LEDCOUNT);
  ws2812fx.show();
  Serial.println("show 1");
  delay(50);
  ws2812fx.clear();
}

void setupOTA() {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println(" OTA Start");
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
  pinMode(BUZZPIN, OUTPUT);
  digitalWrite(BUZZPIN, LOW);

  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting...");

  ws2812fx.init();
  ws2812fx.setBrightness(1);
  ws2812fx.setSpeed(1000);
  ws2812fx.setColor(0x007BFF);
  ws2812fx.setMode(FX_MODE_CHASE_RAINBOW);
  ws2812fx.start();
  Serial.println("start 1");

  button1.attachClick([](){
    Serial.println("Click (start/resume)");
    clickEffect();
    ws2812fx.stop();
    ws2812fx.clear();

    if(pomodoro.getStates() != FSM_POMODORO_RUNNING) {
      pomodoro.start();
    }
  });

  button1.attachDoubleClick([](){
    Serial.println("double Click (pause)");
    clickEffect();
    clickEffect();
    pomodoro.pause();
  });

  button1.attachLongPressStart([](){
    clickEffect();
    clickEffect();
    clickEffect();
    Serial.println("longPress (stop [skip])");
    pomodoro.stop();
  });

  pomodoro.onStart([](){
    Serial.print("onStart - ");
    printCounter();
  });

  pomodoro.onCount([]() {
    Serial.print("onCount - ");
    printCounter();
  });

  pomodoro.onPause([](){
    Serial.print("onPause - ");
    printCounter();
  });

  pomodoro.onResume([](){
    Serial.print("onResume - ");
    printCounter();
  });

  pomodoro.onTick([](){
    Serial.print("onTick - ");
    unsigned int countdown = pomodoro.getCountdown();
    unsigned int maxTempo = pomodoro.getIState() == FSM_INTERVAL ? POMODOROINTERVAL : POMODOROTIMER;
    unsigned int mapa = map(countdown, 0, maxTempo, 0, LEDCOUNT);
    Serial.printf("mapa [%d] - ", mapa);
    printCounter();
  });

  pomodoro.onUpdate([](int countdown){
    unsigned int maxTempo = pomodoro.getIState() == FSM_INTERVAL ? POMODOROINTERVAL : POMODOROTIMER;
    unsigned int mapa = map(countdown, 0, maxTempo, 0, LEDCOUNT);
    uint32_t cor = CYAN;

    // Verifica qual cor ser colocada nos leds baseados no estaudo do pomodoro
    // intervalo curto (Azul) , intervalo longo (Magenta), pomodoro (RED)
    switch (pomodoro.getIState()) {
    case FSM_INTERVAL:
      cor = pomodoro.getPomodoros() > 0 && pomodoro.getPomodoros() % 4 == 0 ? MAGENTA:BLUE;
      break;
    case FSM_WORKTIME:
      cor = RED;
      break;
    default:
      cor = ORANGE;
      break;
    }

    ws2812fx.clear();
    ws2812fx.setColor(cor);
    ws2812fx.fill(cor, 0, mapa);  // Coloca no led ring a cor nos LEDS corretos
    ws2812fx.show();

    if(pomodoro.getStates() != FSM_POMODORO_RUNNING) {
      cor = YELLOW;
      ws2812fx.stop();
      ws2812fx.fill(cor, LEDCOUNT/2, LEDCOUNT);
      ws2812fx.show();
    }
  });

  pomodoro.onFinish([](){
    isEffectRunning = false;
    Serial.print("OnFinish - ");
    Serial.printf("Pomodoros: %02d\n\n", pomodoro.getPomodoros());
    printCounter();
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

  Serial.println("Ready!");
  Serial.printf(" big %d smal %d tempo %d", pomodoro.bigInterval, pomodoro.smallInterval, pomodoro.tempo);
}

void loop() {
  ArduinoOTA.handle();
  ws2812fx.service();
  button1.tick();
  pomodoro.update();
}