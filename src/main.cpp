#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <SPIFFS.h>
#include <TaskScheduler.h>
#include <WebSerial.h>
#include <WiFi.h>

#include "L293N.h"
#include "S3-RGB.h"

/*
note :: real
in1 in2 motor1 : p5 p4
in1 in2 en motor4 : 40 39  
in1 in2 en motor3 : 41 42
in1 in2 en motor2 : P3 13
*/

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define UP_LEFT 5
#define UP_RIGHT 6
#define DOWN_LEFT 7
#define DOWN_RIGHT 8
#define TURN_LEFT 9
#define TURN_RIGHT 10
#define FORK_UP 11
#define FORK_DOWN 12
#define STOP 0

#define FRONT_RIGHT_MOTOR 0
#define BACK_RIGHT_MOTOR 1
#define FRONT_LEFT_MOTOR 2
#define BACK_LEFT_MOTOR 3

#define DIR 4
#define STEP 5

#define FORWARD 1
#define BACKWARD -1

bool turnloop = false;

Scheduler ts;

void forkloop();

Task FORK(5, TASK_FOREVER, &forkloop);
Task RGBT(100, TASK_FOREVER, &RGB_LED);

L293N motor1(0, 40000, 8, 8, 18, 17);
L293N motor2(1, 40000, 8, 9, 15, 16);
L293N motor3(2, 40000, 8, 47, 41, 42);
L293N motor4(3, 40000, 8, 21, 40, 39);

const char *ssid = "ESP32";
const char *password = "mimimimimi";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void processCarMovement(String inputValue) {
    WebSerial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());
    switch (inputValue.toInt()) {
    case UP:
        motor1.forward(255);
        motor2.forward(255);
        motor3.forward(255);
        motor4.forward(255);
        break;

    case DOWN:
        motor1.backward(255);
        motor2.backward(255);
        motor3.backward(255);
        motor4.backward(255);
        break;

    case LEFT:
        motor1.backward(255);
        motor2.forward(255);
        motor3.forward(255);
        motor4.backward(255);
        break;

    case RIGHT:
        motor1.forward(255);
        motor2.backward(255);
        motor3.backward(255);
        motor4.forward(255);
        break;

    case UP_LEFT:
        motor1.stop();
        motor2.forward(255);
        motor3.forward(255);
        motor4.stop();
        break;

    case UP_RIGHT:
        motor1.forward(255);
        motor2.stop();
        motor3.stop();
        motor4.forward(255);
        break;

    case DOWN_LEFT:
	    motor1.backward(255);
        motor2.stop();
        motor3.stop();
        motor4.backward(255);
        break;

    case DOWN_RIGHT:
	    motor1.stop();
        motor2.backward(255);
        motor3.backward(255);
        motor4.stop();
        break;

    case TURN_LEFT:
	    motor1.backward(255);
        motor2.forward(255);
        motor3.backward(255);
        motor4.forward(255);
        break;

    case TURN_RIGHT:
	    motor1.forward(255);
        motor2.backward(255);
        motor3.forward(255);
        motor4.backward(255);
        break;

    case STOP:
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        turnloop = false;
        break;

    case FORK_UP:
        digitalWrite(DIR, HIGH);
        turnloop = true;
        break;

    case FORK_DOWN:
        digitalWrite(DIR, LOW);
        turnloop = true;
        break;

    default:
        motor1.stop();
        motor2.stop();
        motor3.stop();
        motor4.stop();
        break;
    }
}

void handleNotFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Error 404 : File Not Found");
}

void onWebSocketEvent(AsyncWebSocket *server,
                      AsyncWebSocketClient *client,
                      AwsEventType type,
                      void *arg,
                      uint8_t *data,
                      size_t len) {
    switch (type) {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        // client->text(getRelayPinsStatusJson(ALL_RELAY_PINS_INDEX));
        WebSerial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        WebSerial.printf("WebSocket client #%u disconnected\n", client->id());
        processCarMovement("0");
        break;
    case WS_EVT_DATA:
        AwsFrameInfo *info;
        info = (AwsFrameInfo *)arg;
        if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
            std::string myData = "";
            myData.assign((char *)data, len);
            processCarMovement(myData.c_str());
        }
        break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    default:
        break;
    }
}

void recvMsg(uint8_t *data, size_t len) {
    WebSerial.println("Received Data...");
    String d = "";
    for (int i = 0; i < len; i++) {
        d += char(data[i]);
    }
    WebSerial.println(d);
}

void setup() {
    setup_led();
    Serial.begin(115200);
    pinMode(STEP, OUTPUT);
    pinMode(DIR, OUTPUT);

    if (!SPIFFS.begin(true)) {
        Serial.println("An Error has occurred while mounting SPIFFS");
        return;
    }

    WiFi.softAP(ssid, password);
    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/homepage.html"); });
    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/control.html"); });
    server.onNotFound(handleNotFound);

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    WebSerial.begin(&server);
    WebSerial.msgCallback(recvMsg);

    ElegantOTA.begin(&server);

    server.begin();
    Serial.println("HTTP server started");
    ts.addTask(FORK);
	ts.addTask(RGBT);
	RGBT.enable();
}

void loop() {
    if ((turnloop == true) && (!FORK.isEnabled())) {
        FORK.enable();
    } else if ((turnloop == false) && (FORK.isEnabled())) {
        FORK.disable();
    }

    ws.cleanupClients();
    ts.execute();
    ElegantOTA.loop();
}

void forkloop() {
    static bool state = HIGH;
    digitalWrite(STEP, state);
    WebSerial.println(state);
    state = !state;
}