#ifndef L293N_H
#define L293N_H

#include <arduino.h>

class L293N {
public:
    L293N(int CHANNEL, int FREQ, int RESOLUTION, int EN, int IN1, int IN2);
    void forward(int dutyCycle);
    void backward(int dutyCycle);
    void stop();
private:
    int _CHANNEL;
    int _IN1;
    int _IN2;
};

#endif