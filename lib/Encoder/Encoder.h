#ifndef Encoder_h
#define Encoder_h

#include <arduino.h>
#include <ESP32Encoder.h>

class Encoder{
public:
    Encoder(int ENCA, int ENCB);
    void RPM();
private:
    const float GEAR_RATIO = 45.0;
    const float PPR = 11.0;
    unsigned long Time;
    int rpm;
    ESP32Encoder _enc;
};

#endif