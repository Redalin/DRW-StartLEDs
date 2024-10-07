// Include the LED Control Headers
#include "ledControl.h"

#define LED_PIN 16
#define NUM_LEDS 30
#define BRIGHTNESS 200
#define WAVE_DURATION 10000 // wage checkered flag 10 seconds
#define BLOCK_SIZE 5  // Define block size for black and white segments
#define SWITCH_INTERVAL 500  // Interval in milliseconds for switching colors

CRGB leds[NUM_LEDS];

// Setup all the LEDs
void setupLEDs () {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
}

// Set all LEDs to the specified color
void setLEDs(CRGB colour)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = colour;
    }
    FastLED.show();
}

void waveChequeredFlag(CRGB colour) {
    // int wavePosition = 0;
    bool switchFlag = false;  // Variable to keep track of when to switch

    unsigned long startTime = millis(); // Get the starting time

    while (millis() - startTime < WAVE_DURATION)
        // {
        //     // Loop through the LEDs and set the chequered pattern with wave effect
        //     for (int i = 0; i < NUM_LEDS; i++)
        //     {
        //         // make a block of colour 'blockwidth' wide
        //         if ((i + wavePosition) % blockWidth == 0) {
        //             leds[i] = colour;
        //         }
        //         if (wavePosition % blockWidth == 0) {
        //             // once the block is the right width, show the colour
        //             FastLED.show();
        //             delay(200);
        //         }
        //     }

        //     // Increment wavePosition to shift the wave across the strip
        //     wavePosition++;
        //     if (wavePosition >= NUM_LEDS) {
        //         wavePosition = 0;  // Reset wavePosition when it completes a full cycle
        //     }
        // }
        // Set alternating blocks of 5 LEDs to black and white
        for (int i = 0; i < NUM_LEDS; i++) {
            // Alternate between black and white depending on the switchFlag state
            if (((i / BLOCK_SIZE) % 2 == 0 && !switchFlag) || ((i / BLOCK_SIZE) % 2 != 0 && switchFlag)) {
                leds[i] = colour;  // Set input colour for current block
            } else {
                leds[i] = CRGB::Black;  // Set black for current block
            }
        }

        // Show the updated pattern on the LEDs
        FastLED.show();
        
        // Wait for SWITCH_INTERVAL before switching colors
        delay(SWITCH_INTERVAL);

        // Toggle the switchFlag to alternate blocks
        switchFlag = !switchFlag;
}