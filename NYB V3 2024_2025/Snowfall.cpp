#include "Snowfall.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define SnowflakeDelay 100    // Delay between frame updates in milliseconds
#define MaxSnowflakes 10      // Maximum number of simultaneous snowflakes

// Struct to represent a single snowflake
struct Snowflake {
    int row;
    int column;
    bool active;
};

// Array to hold snowflakes
Snowflake snowflakes[MaxSnowflakes];

// Setup function for Snowfall mode
void Snowfall_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize snowflakes
    for (int i = 0; i < MaxSnowflakes; i++) {
        snowflakes[i] = {
            random(0, totalRows),   // Random starting row
            random(0, rowSizes[random(0, totalRows)]), // Random column
            false                  // Initially inactive
        };
    }
}

// Loop function for Snowfall mode
void Snowfall_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // RGB buffer for LED colors

    // Process each snowflake
    for (int i = 0; i < MaxSnowflakes; i++) {
        Snowflake& snowflake = snowflakes[i];

        if (!snowflake.active) {
            // Activate new snowflakes randomly
            if (random(0, 100) < 10) { // 10% chance to activate
                snowflake.row = 0;    // Start at the top
                snowflake.column = random(0, rowSizes[snowflake.row]);
                snowflake.active = true;
            }
        }
        else {
            // Draw the active snowflake
            if (snowflake.row < totalRows && snowflake.column < rowSizes[snowflake.row]) {
                int ledNumber = panelMapping[snowflake.row][snowflake.column];
                ledBuffer[ledNumber][0] = 255; // Snowflake color (White)
                ledBuffer[ledNumber][1] = 255;
                ledBuffer[ledNumber][2] = 255;
            }

            // Move the snowflake down
            snowflake.row++;

            if (snowflake.row >= totalRows) {
                snowflake.active = false; // Deactivate snowflake at the bottom
            }
            else {
                // Randomly decide movement direction with weights
                int move = random(0, 9); // Random number [0, 9]
                if (move < 2) {
                    // 20% chance to go left
                    snowflake.column--;
                }
                else if (move < 5) {
                    // 30% chance to go straight
                    // No change to column
                }
                else {
                    // 40% chance to go right
                    snowflake.column++;
                }

                // Ensure the column stays within bounds
                if (snowflake.column < 0) {
                    snowflake.column = 0;
                }
                else if (snowflake.column >= rowSizes[snowflake.row]) {
                    snowflake.column = rowSizes[snowflake.row] - 1;
                }
            }
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(SnowflakeDelay); // Wait before the next frame
}
