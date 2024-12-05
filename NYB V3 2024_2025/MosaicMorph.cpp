#include "MosaicMorph.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 50  // Delay between frame updates in milliseconds

// Declare ledHues as a pointer
float* ledHues = nullptr;

// Helper function to convert HSV to RGB
void HSVtoRGB(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b)
{
    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i % 6) {
    case 0: r = v * 255; g = t * 255; b = p * 255; break;
    case 1: r = q * 255; g = v * 255; b = p * 255; break;
    case 2: r = p * 255; g = v * 255; b = t * 255; break;
    case 3: r = p * 255; g = q * 255; b = v * 255; break;
    case 4: r = t * 255; g = p * 255; b = v * 255; break;
    case 5: r = v * 255; g = p * 255; b = q * 255; break;
    }
}

// Setup function
void MosaicMorph_setup(SPIController& spiController)
{
    spiController.begin();

    // Dynamically allocate memory for ledHues
    ledHues = new float[NUM_LEDS];

    // Initialize ledHues with random starting hues
    for (int i = 0; i < NUM_LEDS; i++) {
        ledHues[i] = random(0, 360) / 360.0f; // Random hue between 0.0 and 1.0
    }
}

// Loop function
void MosaicMorph_loop(SPIController& spiController)
{
    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[NUM_LEDS][3];  // Declare without initializer

    // Explicitly set each LED color to off (0, 0, 0)
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0;  // Red
        ledBuffer[i][1] = 0;  // Green
        ledBuffer[i][2] = 0;  // Blue
    }

    // Set the first LED (status LED) to green
    int statusLED = panelMapping[0][0];
    ledBuffer[statusLED][0] = 0;   // Red
    ledBuffer[statusLED][1] = 255; // Green
    ledBuffer[statusLED][2] = 0;   // Blue

    // Update hue values and convert to RGB
    for (int i = 0; i < NUM_LEDS; i++) {
        // Slightly change the hue to create a morphing effect
        ledHues[i] += 0.005f; // Increment hue
        if (ledHues[i] > 1.0f) {
            ledHues[i] -= 1.0f;
        }

        // Convert HSV to RGB
        uint8_t r, g, b;
        HSVtoRGB(ledHues[i], 1.0f, 1.0f, r, g, b);

        ledBuffer[i][0] = r; // Red
        ledBuffer[i][1] = g; // Green
        ledBuffer[i][2] = b; // Blue
    }

    // Send the frame without stars
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);  // Delay before the next frame update
}
