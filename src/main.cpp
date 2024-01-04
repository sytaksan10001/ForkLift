#include "Encoder.h"
#include "L298N.h"
#include "S3-RGB.h"
#include <Arduino.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h>
#include <PCF8574.h>
#include <QuickPID.h>
#include <SPIFFS.h>
#include <TaskScheduler.h>
#include <WebSerial.h>
#include <WiFi.h>

#define DIR 4
#define STEP 5

float Kp = 1, Ki = 0, Kd = 0;
bool turnloop = false;
long pulse;
unsigned long prevT, currT;
double rpm[2];
double error[2], integral[2], derivatif[2], lastError[2], PID[2], prev[2], curr[2], puls[2];
int count;
double mapped, k, pred;
bool cases[9];
double p;
double errors, integrals, derivatifs, lastErrors;

float Setpoint, Input, Output;
QuickPID myPID(&Input, &Output, &Setpoint);

/*
note :: real
in1 in2 motor1 : p5 p4
in1 in2 en motor4 : 40 39
in1 in2 en motor3 : 41 42
in1 in2 en motor2 : P3 13
*/

#define A1 1
#define A2 2
#define A3 3
#define B1_ 4
#define B2 5
#define B3 6
#define C1 7
#define C2 8
#define C3 9

Scheduler ts;

void forkloop();
void forward(int jarak);
long limit(long val);
void Kanan();
void Kiri();

Task FORK(5, TASK_FOREVER, &forkloop);
Task RGBT(100, TASK_FOREVER, &RGB_LED);

L298N motor1(0, 40000, 8, 13, P6, P5, true);
L298N motor2(1, 40000, 8, 47, 42, 41);
L298N motor3(2, 40000, 8, 14, P4, P3, true);
L298N motor4(3, 40000, 8, 21, 40, 39);

Encoder enc1(motor1, 17, 18);
Encoder enc2(motor2, 37, 38);
Encoder enc3(motor3, 10, 9);
Encoder enc4(motor4, 36, 35);

const char *ssid = "ESP32";
const char *password = "mimimimimi";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void processCarMovement(String inputValue) {
    WebSerial.printf("Got value as %s %d\n", inputValue.c_str(), inputValue.toInt());
    switch (inputValue.toInt()) {
    case A1:
        cases[0] = true;
        break;
    case A2:
        cases[1] = true;
        break;
    case A3:
        cases[2] = true;
        break;
    case B1_:
        cases[3] = true;
        break;
    case B2:
        cases[4] = true;
        break;
    case B3:
        cases[5] = true;
        break;
    case C1:
        cases[6] = true;
        break;
    case C2:
        cases[7] = true;
        break;
    case C3:
        cases[8] = true;
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
    millis();
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
    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request) { request->send(SPIFFS, "/mobile.html"); });
    server.onNotFound(handleNotFound);

    ws.onEvent(onWebSocketEvent);
    server.addHandler(&ws);

    WebSerial.begin(&server);
    WebSerial.msgCallback(recvMsg);

    ElegantOTA.begin(&server);

    server.begin();
    _pcf.begin();
    ts.addTask(FORK);
    ts.addTask(RGBT);

    RGBT.enable();

    myPID.SetTunings(Kp, Ki, Kd);
    myPID.SetMode(1);
    myPID.SetOutputLimits(130, 240);
    pred = 0;
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

    currT = millis();

    if (cases[0]) {
        forward(6000);
        cases[0] = false;

    } else if (cases[1]) {
        forward(3000);
        Kiri();
        forward(1500);
        Kanan();
        forward(3000);
        cases[1] = false;

    } else if (cases[2]) {
        forward(3000);
        Kiri();
        forward(3000);
        Kanan();
        forward(3000);
        cases[2] = false;

    } else if (cases[3]) {
        forward(3000);
        Kanan();
        forward(1500);
        Kiri();
        forward(3000);
        cases[3] = false;

    } else if (cases[4]) {
        forward(6000);
        cases[4] = false;

    } else if (cases[5]) {
        forward(3000);
        Kiri();
        forward(1500);
        Kanan();
        forward(3000);
        cases[5] = false;

    } else if (cases[6]) {
        forward(3000);
        Kanan();
        forward(3000);
        Kiri();
        forward(3000);
        cases[6] = false;

    } else if (cases[7]) {
        forward(3000);
        Kanan();
        forward(1500);
        Kiri();
        forward(3000);
        cases[7] = false;

    } else if (cases[8]) {
        forward(6000);
        cases[8] = false;
    }
}

void forkloop() {
    static bool state = HIGH;
    digitalWrite(STEP, state);
    WebSerial.println(state);
    state = !state;
}

void forward(int jarak) {
    pulse = 0;
    enc4._encoder.clearCount();
    enc3._encoder.clearCount();
    Setpoint = 50;

    while (pulse <= jarak) {
        currT = millis();
        if (currT - prevT > 100) {
            puls[0] = enc3._encoder.getCount();
            puls[1] = enc4._encoder.getCount();
            curr[0] = puls[0] - prev[0];
            curr[1] = puls[1] - prev[1];

            rpm[0] = ((curr[0] / (11 * 45 * 2)) * (60 / 0.1));
            rpm[1] = ((curr[1] / (11 * 45 * 2)) * (60 / 0.1));

            motor3.forward(205);
            motor4.forward(200);

            prev[0] = puls[0];
            prev[1] = puls[1];
            prevT = currT;

            Serial.print(rpm[0]);
            Serial.print("\t");
            Serial.println(rpm[1]);
        }
        pulse = enc4._encoder.getCount();
    }
    motor3.stop();
    motor4.stop();
}

void Kanan() {
    pulse = 0;

    enc4._encoder.clearCount();
    enc3._encoder.clearCount();

    while (pulse <= 600) {
        motor3.forward(206);
        motor4.backward(200);
        pulse = enc4._encoder.getCount();
    }
    // delay(1000);
    
    motor3.stop();
    motor4.stop();
}

void Kiri() {
    motor3.backward(206);
    motor4.forward(200);
    delay(950);
    motor3.stop();
    motor4.stop();
}

long limit(long val) {
    if (val > 255) {
        val = 255;
    } else if (val < 140) {
        val = 255;
    }
    return val;
}
