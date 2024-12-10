#include "DiamondWeave.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define WEAVE_DELAY 100 // Delay in milliseconds between weave transitions

static float brightnessScaling = 1; // Adjust the overall brightness
static int weavePhase = 0; // Tracks the current phase of the weave pattern

void DiamondWeave_setup(SPIController& spiController) {
    spiController.begin();
}

void DiamondWeave_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Create crisscross woven texture
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Calculate row and column based on panel mapping
            int row = -1, col = -1;
            for (int r = 0; r < totalRows; r++) {
                for (int c = 0; c < rowSizes[r]; c++) {
                    if (panelMapping[r][c] == ledNumber) {
                        row = r;
                        col = c;
                        break;
                    }
                }
                if (row != -1) break;
            }

            // Apply a crisscross pattern based on row and column indices
            if ((row + col + weavePhase) % 2 == 0) {
                // Bright part of the weave
                ledBuffer[ledNumber][0] = (uint8_t)(brightnessScaling * MAX_BRIGHTNESS); // Red
                ledBuffer[ledNumber][1] = (uint8_t)(brightnessScaling * MAX_BRIGHTNESS); // Green
                ledBuffer[ledNumber][2] = (uint8_t)(brightnessScaling * MAX_BRIGHTNESS); // Blue
            }
            else {
                // Dimmer part of the weave
                ledBuffer[ledNumber][0] = (uint8_t)(brightnessScaling * MAX_BRIGHTNESS / 4); // Red
                ledBuffer[ledNumber][1] = (uint8_t)(brightnessScaling * MAX_BRIGHTNESS / 4); // Green
                ledBuffer[ledNumber][2] = (uint8_t)(brightnessScaling * MAX_BRIGHTNESS / 4); // Blue
            }
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(WEAVE_DELAY); // Delay for the effect
    weavePhase = (weavePhase + 1) % 2; // Alternate the weave phase
}
