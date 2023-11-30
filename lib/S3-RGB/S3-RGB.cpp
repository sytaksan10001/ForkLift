#include "S3-RGB.h"

#define LED_PIN     48
#define NUM_LEDS    1
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType currentBlending;

void setup_led(){
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);

    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
}

void RGB_LED(){
    static uint8_t brightness = 255;
    static uint8_t colorIndex = 0;
    colorIndex++;

    for (int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }

    FastLED.show();
}