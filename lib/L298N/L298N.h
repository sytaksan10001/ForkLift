#ifndef L298N_H
#define L298N_H

#include <PCF8574.h>
#include <arduino.h>

extern PCF8574 _pcf;

class L298N {
  public:
    L298N(int CHANNEL, int FREQ, int RESOLUTION, int EN, int IN1, int IN2);
    L298N(int CHANNEL, int FREQ, int RESOLUTION, int EN, int IN1, int IN2, bool tes);
    void forward(int dutyCycle);
    void backward(int dutyCycle);
    void stop();
    void test();
    void test2();
    
  private:
    void forwardI2C(int dutyCycle);
    void backwardI2C(int dutyCycle);
    void stopI2C();
    void forwardNormal(int dutyCycle);
    void backwardNormal(int dutyCycle);
    void stopNormal();
    int _CHANNEL;
    int _IN1;
    int _IN2;
    // PCF8574 _pcf;
    bool USE_PCF;
};

#endif