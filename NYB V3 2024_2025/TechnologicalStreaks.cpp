#include "TechnologicalStreaks.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

#define LEDDelay 100    // Delay between frame updates in milliseconds
#define StreakLength 5  // Length of each streak

static int streakPositions[totalRows][StreakLength]; // Track streak positions for all rows
static bool streakDirection[totalRows];             // Track streak direction (true: up, false: down)

static float brightnessScaling = 1;   // Brightness scaling factor

// Randomly initialize streak positions and directions
void initializeStreaks() {
    for (int row = 0; row < totalRows; row++) {
        for (int s = 0; s < StreakLength; s++) {
            if (rowSizes[row] > StreakLength) {
                streakPositions[row][s] = random(rowSizes[row] - StreakLength);
            }
            else {
                streakPositions[row][s] = 0; // Default to 0 if the row is too small
            }
        }
        streakDirection[row] = random(2) == 0; // Random direction
    }
}

void TechnologicalStreaks_setup(SPIController& spiController) {
    spiController.begin();
    initializeStreaks();
}

void TechnologicalStreaks_loop(SPIController& spiController) {
    
    uint8_t ledBuffer[NUM_LEDS][3];

    // Clear the LED buffer
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    for (int row = 0; row < totalRows; row++) {
        // Update streak positions
        if (streakDirection[row]) {
            for (int s = 0; s < StreakLength; s++) {
                streakPositions[row][s]++;
                if (streakPositions[row][s] >= rowSizes[row]) {
                    streakPositions[row][s] = 0; // Loop back to the start
                }
            }
        }
        else {
            for (int s = 0; s < StreakLength; s++) {
                streakPositions[row][s]--;
                if (streakPositions[row][s] < 0) {
                    streakPositions[row][s] = rowSizes[row] - 1; // Loop back to the end
                }
            }
        }

        // Render streaks
        for (int s = 0; s < StreakLength; s++) {
            if (row < totalRows && streakPositions[row][s] < rowSizes[row]) {
                int led = panelMapping[row][streakPositions[row][s]];
                uint8_t brightness = 255 - (s * 50); // Dimming effect

                // Alternate red and blue streaks
                if (row % 2 == 0) {
                    ledBuffer[led][0] = brightness; // Red
                    ledBuffer[led][1] = 0;         // Green
                    ledBuffer[led][2] = 0;         // Blue
                }
                else {
                    ledBuffer[led][0] = 0;         // Red
                    ledBuffer[led][1] = 0;         // Green
                    ledBuffer[led][2] = brightness; // Blue
                }
            }
        }
    }

    // Check for collisions and add bursts of white light
    for (int i = 0; i < NUM_LEDS; i++) {
        if (ledBuffer[i][0] > 0 && ledBuffer[i][2] > 0) { // Red and blue overlap
            ledBuffer[i][0] = 255; // Red
            ledBuffer[i][1] = 255; // Green
            ledBuffer[i][2] = 255; // Blue
        }
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay); // Delay before the next frame update
}
