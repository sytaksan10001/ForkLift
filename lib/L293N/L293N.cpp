#include "L293N.h"
#include "Arduino.h"

L293N::L293N(int CHANNEL, int FREQ, int RESOLUTION, int EN, int IN1, int IN2) {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(EN, OUTPUT);
    
    ledcSetup(CHANNEL, FREQ, RESOLUTION);
    ledcAttachPin(EN, CHANNEL);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);

    _IN1 = IN1;
    _IN2 = IN2;
    _CHANNEL = CHANNEL;
}

void L293N::forward(int dutyCycle){
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, HIGH);
    ledcWrite(_CHANNEL, dutyCycle); 
}

void L293N::backward(int dutyCycle){
    digitalWrite(_IN1, HIGH);
    digitalWrite(_IN2, LOW);
    ledcWrite(_CHANNEL, dutyCycle); 
}

void L293N::stop(){
    digitalWrite(_IN1, LOW);
    digitalWrite(_IN2, LOW);
    ledcWrite(_CHANNEL, 0); 
}