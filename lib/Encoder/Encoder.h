#ifndef ENCODER_H
#define ENCODER_H

#include "L298N.h"
#include <Arduino.h>
#include <ESP32Encoder.h>
#include <QuickPID.h>

class Encoder {
  public:
    Encoder(L298N &motor, int phaseA, int phaseB);
    void RPM(float setpoint);
    void foward();
    void init();
    QuickPID myPID;
    float Setpoint, Input, Output;
    ESP32Encoder _encoder;

  private:
    L298N _motor;
    float position, mapped;
    float Kp = 5, Ki = 1, Kd = 0;
};

void move(float jarak, Encoder enc1, Encoder enc2, Encoder enc3, Encoder enc4);

#endif