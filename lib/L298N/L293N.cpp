#include "Arduino.h"
#include "L298N.h"

PCF8574 _pcf(0x38, 12, 11);

L298N::L298N(int CHANNEL, int FREQ, int RESOLUTION, int EN, int IN1, int IN2) : _IN1(IN1), _IN2(IN2), _CHANNEL(CHANNEL) {
    USE_PCF = false;
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(EN, OUTPUT);

    ledcSetup(CHANNEL, FREQ, RESOLUTION);
    ledcAttachPin(EN, CHANNEL);

    _IN1 = IN1;
    _IN2 = IN2;
    _CHANNEL = CHANNEL;
}

L298N::L298N(int CHANNEL, int FREQ, int RESOLUTION, int EN, int IN1, int IN2, bool tes) : _IN1(IN1), _IN2(IN2), _CHANNEL(CHANNEL) {
    USE_PCF = true;
    Serial.begin(115200);
    _pcf.pinMode(IN1, OUTPUT);
    _pcf.pinMode(IN2, OUTPUT);
    _pcf.pinMode(EN, OUTPUT);

    ledcSetup(CHANNEL, FREQ, RESOLUTION);
    ledcAttachPin(EN, CHANNEL);

    _IN1 = IN1;
    _IN2 = IN2;
    _CHANNEL = CHANNEL;
}

void L298N::forwardNormal(int dutyCycle) {
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, HIGH);
    ledcWrite(_CHANNEL, dutyCycle);
}

void L298N::backwardNormal(int dutyCycle) {
    digitalWrite(_IN1, HIGH);
    digitalWrite(_IN2, LOW);
    ledcWrite(_CHANNEL, dutyCycle);
}

void L298N::stopNormal() {
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, LOW);
    ledcWrite(_CHANNEL, 0);
}

void L298N::forwardI2C(int dutyCycle) {
    _pcf.digitalWrite(_IN1, LOW);
    _pcf.digitalWrite(_IN2, HIGH);
    ledcWrite(_CHANNEL, dutyCycle);
}

void L298N::backwardI2C(int dutyCycle) {
    _pcf.digitalWrite(_IN1, HIGH);
    _pcf.digitalWrite(_IN2, LOW);
    ledcWrite(_CHANNEL, dutyCycle);
}

void L298N::stopI2C() {
    _pcf.digitalWrite(_IN1, LOW);
    _pcf.digitalWrite(_IN2, LOW);
    ledcWrite(_CHANNEL, 0);
}

void L298N::test() {
    _pcf.digitalWrite(_IN1, LOW);
    _pcf.digitalWrite(_IN2, HIGH);
}

void L298N::test2() {
    Serial.println(_IN1);
    Serial.println(_IN2);
}

void L298N::forward(int dutyCycle) {
    if (USE_PCF) {
        L298N::forwardI2C(dutyCycle);
    } else {
        L298N::forwardNormal(dutyCycle);
    }
}

void L298N::backward(int dutyCycle) {
    if (USE_PCF) {
        L298N::backwardI2C(dutyCycle);
    } else {
        L298N::backwardNormal(dutyCycle);
    }
}

void L298N::stop() {
    if (USE_PCF) {
        L298N::stopI2C();
    } else {
        L298N::stopNormal();
    }
}