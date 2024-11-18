#include "SPIController.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define CheckerboardDelay 100 // Delay between pattern updates in milliseconds

void Checkerboard_setup(SPIController& spiController) {
    spiController.begin(); // Initialize SPI
}

void Checkerboard_loop(SPIController& spiController) {
    static bool toggle = false; // Toggle to alternate colors
    uint8_t ledBuffer[NUM_LEDS][3]; // RGB buffer for each LED

    // Clear the LED buffer
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Alternate colors in a checkerboard pattern
    for (int row = 0; row < totalRows; row++) {
        for (int i = 0; i < rowSizes[row]; i++) {
            int ledNumber = panelMapping[row][i];
            bool isEvenCell = ((row + i) % 2 == 0); // Checkerboard logic

            // Toggle between two colors for the checkerboard
            if (isEvenCell ^ toggle) {
                // Color 1: Red
                ledBuffer[ledNumber][0] = 255; // Red
                ledBuffer[ledNumber][1] = 0;   // Green
                ledBuffer[ledNumber][2] = 0;   // Blue
            }
            else {
                // Color 2: Green
                ledBuffer[ledNumber][0] = 0;   // Red
                ledBuffer[ledNumber][1] = 255; // Green
                ledBuffer[ledNumber][2] = 0;   // Blue
            }
        }
    }

    // Send the checkerboard pattern to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    // Toggle the checkerboard pattern for the next frame
    toggle = !toggle;

    delay(CheckerboardDelay); // Wait before the next update
}
