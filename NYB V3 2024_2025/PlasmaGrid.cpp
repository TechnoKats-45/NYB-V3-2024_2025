#include "PlasmaGrid.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define PLASMA_DELAY 50 // Delay in milliseconds between updates
#define GRID_SPEED 3 // Controls the speed of grid motion
#define COLOR_SHIFT_SPEED 2 // Controls the speed of color cycling

static float brightnessScaling = 0.5; // Adjust the overall brightness
static float timeStep = 0; // Time variable for plasma motion

// Converts a position to a hue value based on plasma effect
uint8_t calculateHue(float x, float y, float t) {
    float value = sin(x * 0.2 + t) + sin(y * 0.3 - t) + sin((x + y) * 0.1 + t);
    return (uint8_t)((value + 3) * 42.5); // Normalize to 0-255
}

// Sets an LED's RGB values based on a hue
void setRGBFromHue(uint8_t hue, uint8_t& red, uint8_t& green, uint8_t& blue) {
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

void PlasmaGrid_setup(SPIController& spiController) {
    spiController.begin();
}

void PlasmaGrid_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Calculate grid plasma effect for each LED in the panel
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Map LED position to grid coordinates
            float x = (i % 4) * GRID_SPEED; // Simulate X grid position
            float y = (i / 4) * GRID_SPEED; // Simulate Y grid position

            // Calculate the hue based on the plasma field
            uint8_t hue = calculateHue(x, y, timeStep);

            // Convert hue to RGB
            uint8_t red, green, blue;
            setRGBFromHue(hue, red, green, blue);

            // Apply brightness scaling and set the LED color
            ledBuffer[ledNumber][0] = (uint8_t)(brightnessScaling * red);
            ledBuffer[ledNumber][1] = (uint8_t)(brightnessScaling * green);
            ledBuffer[ledNumber][2] = (uint8_t)(brightnessScaling * blue);
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(PLASMA_DELAY); // Delay for smooth animation

    // Increment the time step for plasma motion
    timeStep += COLOR_SHIFT_SPEED * 0.01;
}
