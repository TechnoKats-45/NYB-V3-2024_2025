#include "PulsarRings.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define RING_DELAY 50       // Delay in milliseconds between updates
#define EXPANSION_SPEED 1   // Speed of the ring expansion and contraction
#define COLOR_SHIFT_SPEED 2 // Speed of the color palette shift

static float brightnessScaling = 1; // Adjust the overall brightness
static int ringPhase = 0;              // Tracks the current phase of ring expansion
static int colorOffset = 0;            // Tracks the color palette shift

// Converts a hue value to RGB
static void setRGBFromHue(uint8_t hue, uint8_t& red, uint8_t& green, uint8_t& blue) {
    if (hue < 85) {
        red = hue * 3;
        green = 255 - hue * 3;
        blue = 0;
    }
    else if (hue < 170) {
        hue -= 85;
        red = 255 - hue * 3;
        green = 0;
        blue = hue * 3;
    }
    else {
        hue -= 170;
        red = 0;
        green = hue * 3;
        blue = 255 - hue * 3;
    }
}

void PulsarRings_setup(SPIController& spiController) {
    spiController.begin();
}

void PulsarRings_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Define the center and expand outward in concentric rings
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Calculate distance-based ring intensity
            int distanceFromCenter = abs(i - ringPhase % numLEDsInPanel);
            float intensity = max(0.0f, 1.0f - distanceFromCenter / 5.0f); // Decrease intensity further from the center

            // Adjust color based on ring phase and intensity
            uint8_t hue = (ringPhase * COLOR_SHIFT_SPEED + colorOffset) % 255;
            uint8_t red, green, blue;
            setRGBFromHue(hue, red, green, blue);

            // Apply intensity scaling
            ledBuffer[ledNumber][0] = (uint8_t)(brightnessScaling * red * intensity);
            ledBuffer[ledNumber][1] = (uint8_t)(brightnessScaling * green * intensity);
            ledBuffer[ledNumber][2] = (uint8_t)(brightnessScaling * blue * intensity);
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(RING_DELAY); // Delay for smooth animation

    // Update the ring phase and color offset
    ringPhase = (ringPhase + EXPANSION_SPEED) % NUM_LEDS;
    colorOffset = (colorOffset + 1) % 255;
}
