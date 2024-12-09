#include "CountdownCircle.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define CountdownDelay 1000 // Delay between panel updates in milliseconds

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect

// Correct clockwise panel order (excluding Panels 1, 7, and 12, and ending with Panel 16 in the center)
const int panelOrder[] = {
    2, 3, 4, 5, 6, 8, 9, 10, 11, 13, 14, 15, 16
};
const int totalPanelsInOrder = sizeof(panelOrder) / sizeof(panelOrder[0]);

// Setup function for CountdownCircle mode
void CountdownCircle_setup(SPIController& spiController) {
    spiController.begin();
}

// Loop function for CountdownCircle mode
void CountdownCircle_loop(SPIController& spiController) {
    static int currentPanelIndex = 0; // Start with the first panel in the order
    static int secondsRemaining = totalPanelsInOrder; // Number of panels to turn off

    if (secondsRemaining <= 0) {
        // Reset after countdown ends
        currentPanelIndex = 0;
        secondsRemaining = totalPanelsInOrder;
    }

    // Clear the LED buffer
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 };

    // Light up all panels except those already turned off
    for (int i = currentPanelIndex; i < totalPanelsInOrder; i++) {
        int panelID = panelOrder[i];
        for (int j = 0; j < (int)(sizeof(panelDefinitions[panelID - 1]) / sizeof(int)); j++) {
            if (panelDefinitions[panelID - 1][j] == 0) break; // End of panel's LEDs
            int ledNumber = panelDefinitions[panelID - 1][j];
            ledBuffer[ledNumber][0] = 255; // Red
            ledBuffer[ledNumber][1] = 255; // Green
            ledBuffer[ledNumber][2] = 255; // Blue
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int i = 0; i < NUM_LEDS; i++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[i][0], ledBuffer[i][1], ledBuffer[i][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(CountdownDelay); // Wait for the delay

    // Move to the next panel and decrease the countdown
    currentPanelIndex++;
    secondsRemaining--;
}
