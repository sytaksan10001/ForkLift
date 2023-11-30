#include "Arduino.h"
#include "Encoder.h"

// const float GEAR_RATIO = 45.0;
// const float PPR = 11.0;
// unsigned long Time;
// int rpm;

Encoder::Encoder(int ENCA, int ENCB){
    ESP32Encoder enc;
    _enc = enc;
    pinMode(ENCA, INPUT_PULLUP);
    pinMode(ENCB, INPUT_PULLUP);
    _enc.attachFullQuad(ENCA, ENCB);
    _enc.setCount(0);
    Time = millis();
}

void Encoder::RPM(){
    long newPosition = _enc.getCount();
    while(millis()-Time>=100){
        rpm = (newPosition/4/GEAR_RATIO/PPR/2) * 60 / 0.1;
        Serial.print("RPM : ");
        Serial.println(rpm);
        _enc.clearCount();
        Time=millis();
    }
    
}