#include <arduino.h>
#include "Stepper.h"
#include "S3-RGB.h"

auto timer = timer_create_default();
Timer<10, millis, String> timerString;
Timer<10, millis, bool> timerBool;

int test = 1;

#define IN1 15
#define IN2 16
#define IN3 17
#define IN4 18

bool SerialPrint(String text){
    Serial.println(text);
    return true;
}

void setup() {
    delay(2000);
    Serial.begin(115200);

    setup_led();
    init_Stepper(IN1, IN2, IN3, IN4);

    timer.every(100, RGB_LED);
    timerBool.every(2, Stepper, true);
    timerString.every(1000, SerialPrint, "RGB and Stepper Test");
}

void loop() {
    timer.tick(); 
    timerBool.tick();
    timerString.tick();
}


// #include <Arduino.h>
// #include "L293N.h"
// #include "Encoder.h"

// #define ENA 14
// #define IN1 13
// #define IN2 12
// #define ENB 9
// #define IN3 11
// #define IN4 10
// #define ENCA 1 
// #define ENCB 2 

// #define LED_CHANNEL 0
// #define freq 30000
// #define RESOLUTION 8

// L293N motor1(0, freq, RESOLUTION, ENA, IN1, IN2);
// L293N motor2(1, freq, RESOLUTION, ENB, IN3, IN4);
// // Encoder Encoder1(ENCA, ENCB);

// void setup(){
//   Serial.begin(115200);
// }

// void loop(){
//   motor1.maju(250);
//   motor2.maju(250);
//   // Encoder1.RPM();
//   Serial.println("testing");
// }