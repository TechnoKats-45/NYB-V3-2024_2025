#include "HypnoticSpiral.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define SpiralDelay 150 // Delay between updates in milliseconds

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect

// Spiral order of panels (skipping 1, 7, 12, and 16)
const int spiralOrder[] = {
    2, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15
};
const int totalSpiralPanels = sizeof(spiralOrder) / sizeof(spiralOrder[0]);

// Setup function for Hypnotic Spiral mode
void HypnoticSpiral_setup(SPIController& spiController) {
    spiController.begin();
}

// Loop function for Hypnotic Spiral mode
void HypnoticSpiral_loop(SPIController& spiController) {
    static float hueOffset = 0.0; // Offset for the color transition
    static float hueIncrement = 1.0 / totalSpiralPanels; // Hue change per panel

    // LED buffer for RGB data
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 };

    // Light up all panels in the spiral order with smooth color transitions
    for (int i = 0; i < totalSpiralPanels; i++) {
        int panelID = spiralOrder[i];

        // Calculate hue for this panel
        float hue = fmod(hueOffset + i * hueIncrement, 1.0);

        // Convert hue to RGB
        uint8_t red, green, blue;
        spiController.hsvToRgb(hue, 1.0, 1.0, red, green, blue);

        // Set LEDs in the current panel
        for (int j = 0; j < (int)(sizeof(panelDefinitions[panelID - 1]) / sizeof(int)); j++) {
            if (panelDefinitions[panelID - 1][j] == 0) break; // End of panel's LEDs
            int ledNumber = panelDefinitions[panelID - 1][j];
            ledBuffer[ledNumber][0] = red;
            ledBuffer[ledNumber][1] = green;
            ledBuffer[ledNumber][2] = blue;
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int i = 0; i < NUM_LEDS; i++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[i][0], ledBuffer[i][1], ledBuffer[i][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(SpiralDelay); // Wait for the delay

    // Update the hue offset for smooth color transitions
    hueOffset += hueIncrement;
    if (hueOffset >= 1.0) {
        hueOffset -= 1.0;
    }
}
