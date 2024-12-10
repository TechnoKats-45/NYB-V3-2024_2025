#include "FractalBloom.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define FRACTAL_DELAY 60 // Delay in milliseconds between updates
#define FRACTAL_SPEED 1 // Speed of fractal growth and retraction

static float brightnessScaling = 0.5; // Adjust the overall brightness
static int growthPhase = 0; // Tracks the current growth phase of the fractal bloom
static bool isGrowing = true; // Indicates whether the fractal is growing or retracting

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

void FractalBloom_setup(SPIController& spiController) {
    spiController.begin();
}

void FractalBloom_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Calculate fractal growth/retraction for the panel
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Determine the level of the fractal growth for this LED
            int fractalLevel = (i + growthPhase) % numLEDsInPanel;

            // Adjust brightness and color based on fractal level
            uint8_t hue = (fractalLevel * 10) % 255; // Hue varies based on fractal level
            uint8_t red, green, blue;
            setRGBFromHue(hue, red, green, blue);

            // Scale brightness based on fractal level
            float brightness = 1.0 - (float)fractalLevel / numLEDsInPanel;
            ledBuffer[ledNumber][0] = (uint8_t)(brightnessScaling * red * brightness);
            ledBuffer[ledNumber][1] = (uint8_t)(brightnessScaling * green * brightness);
            ledBuffer[ledNumber][2] = (uint8_t)(brightnessScaling * blue * brightness);
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(FRACTAL_DELAY); // Delay for smooth animation

    // Update the growth phase
    if (isGrowing) {
        growthPhase += FRACTAL_SPEED;
        if (growthPhase >= totalPanels) {
            isGrowing = false; // Switch to retraction
        }
    }
    else {
        growthPhase -= FRACTAL_SPEED;
        if (growthPhase <= 0) {
            isGrowing = true; // Switch to growth
        }
    }
}
