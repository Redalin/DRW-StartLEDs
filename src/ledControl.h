// connect-wifi.h
#ifndef LED_CONTROL_H
#define LED_CONTROL_H

// Removing the LED Control Part into another file
#include <FastLED.h>

void setupLEDs();
void setLEDs(CRGB colour);
void waveChequeredFlag(CRGB colour);

#endif  //  LED_CONTROL_H