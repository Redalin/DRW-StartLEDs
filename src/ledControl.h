// connect-wifi.h
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

// Removing the LED Control Part into another file
#include <FastLED.h>

void initLEDs();
void setLEDs(CRGB colour);
void waveChequeredFlag(CRGB colour);

#endif  //  LED_CONTROL_H