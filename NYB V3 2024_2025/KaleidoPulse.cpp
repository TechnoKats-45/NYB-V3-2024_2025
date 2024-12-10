#include "KaleidoPulse.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define PULSE_DELAY 50 // Delay in milliseconds between updates
#define PULSE_SPEED 3 // Speed of the pulse rotation

static float brightnessScaling = 0.5; // Adjust the overall brightness
static int rotationPhase = 0; // Current phase for ripple rotation
static int colorOffset = 0; // Offset to shift through the color palette

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

void KaleidoPulse_setup(SPIController& spiController) {
    spiController.begin();
}

void KaleidoPulse_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Ripple effect within the panel
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Calculate the ripple phase and color based on the LED index
            int ripplePhase = (rotationPhase + i) % numLEDsInPanel;
            uint8_t hue = (ripplePhase * 10 + colorOffset) % 255;

            // Convert the hue to RGB
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

    delay(PULSE_DELAY); // Delay for smooth animation

    // Update the rotation phase and color offset
    rotationPhase = (rotationPhase + PULSE_SPEED) % totalPanels; // Advance the ripple
    colorOffset = (colorOffset + 1) % 255; // Shift the color palette
}
