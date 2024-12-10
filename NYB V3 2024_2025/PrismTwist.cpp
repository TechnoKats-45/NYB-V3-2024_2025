#include "PrismTwist.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define TWIST_DELAY 50       // Delay in milliseconds between updates
#define TWIST_SPEED 2        // Speed of the prism twist
#define HUE_SPREAD 40        // Spread of hues across the panel

static float brightnessScaling = 0.5; // Adjust the overall brightness
static int twistPhase = 0;             // Tracks the current phase of the twist animation

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

void PrismTwist_setup(SPIController& spiController) {
    spiController.begin();
}

void PrismTwist_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Assign colors with a twisting effect
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Calculate the hue with a twisting effect
            uint8_t hue = (twistPhase + i * HUE_SPREAD / numLEDsInPanel) % 255;
            uint8_t red, green, blue;
            setRGBFromHue(hue, red, green, blue);

            // Set the LED color
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

    delay(TWIST_DELAY); // Delay for smooth animation

    // Update the twist phase for dynamic motion
    twistPhase = (twistPhase + TWIST_SPEED) % 255;
}
