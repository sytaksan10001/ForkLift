#include "Encoder.h"

Encoder::Encoder(L298N &motor, int phaseA, int phaseB) : _motor(motor) {
    ESP32Encoder encoder;
    encoder.attachSingleEdge(phaseA, phaseB);
    encoder.setCount(0);
    _encoder = encoder;
    Serial.begin(115200);
}

void Encoder::init() {
    QuickPID _myPID(&Input, &Output, &Setpoint);
    myPID = _myPID;
    float Kp = 5, Ki = 0, Kd = 0.01;
    myPID.SetTunings(Kp, Ki, Kd);
    myPID.SetMode(1);
    myPID.SetOutputLimits(-255, 255);
    myPID.SetSampleTimeUs(10000);
}

void Encoder::RPM(float setpoint) {
    Setpoint = setpoint;
    position = _encoder.getCount();
    Input = ((position / (11 * 45 * 2)) * (60 / 0.01));
    // Input = 2.00;

    // myPID.Compute();
    _encoder.clearCount();
    // Serial.println(Output);
    mapped = map(abs(Output), 0, 255, 115, 240);

    // if (Output > 0) {
        // digitalWrite(40, LOW);
        // digitalWrite(39, HIGH);
        // _motor.forward(mapped);
    // } else {
        // digitalWrite(40, HIGH);
        // digitalWrite(39, LOW);
        // _motor.backward(mapped);
    // }
    _motor.forward(mapped);
    // Serial.print("rpm: ");
    // Serial.println(Input);
}

void Encoder::foward() {
    _motor.forward(255);
    static long i;
    i = _encoder.getCount();
    Serial.println(i);
}

void move(float jarak, Encoder enc1, Encoder enc2, Encoder enc3, Encoder enc4) {
    static float setpoint = jarak;
    static float input, output, rpm = 0, elapsed = 0, distance_traveled = 0;
    static float const diameter = 80;

    static QuickPID myPID(&input, &output, &setpoint);
    int i;

    distance_traveled += (PI * diameter * rpm * elapsed) / 60000;
    input = distance_traveled;

    static bool once = [&i]() {
        float Kp = 5, Ki = 1, Kd = 0;

        myPID.SetTunings(Kp, Ki, Kd);
        myPID.SetMode(1);
        myPID.SetOutputLimits(0, 90);
        myPID.SetSampleTimeUs(10000);
        i = 10;
        return true;
    }();

    Serial.println(i);
    myPID.Compute();

    // rpm = ((enc1.RPM(output) + enc2.RPM(output) + enc3.RPM(output) + enc4.RPM(output)) / 4);

    // enc1.RPM(output);
    // enc2.RPM(output);
    // enc3.RPM(output);
    // enc4.RPM(output);

    elapsed += 0.01;
}