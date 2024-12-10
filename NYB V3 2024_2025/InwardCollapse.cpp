#include "InwardCollapse.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Configuration
#define LEDDelay 50         // Delay between frame updates in milliseconds
#define CollapseSpeed 0.5   // Speed of the inward collapse animation
#define FadeSpeed 0.1       // Speed of fade-out effect
#define ColorCycleSpeed 0.02 // Rate of color cycling

static float collapsePhase = 0; // Phase of the collapse animation
static float centerX = 0.0f;    // Center x-coordinate of the triangle
static float centerY = 0.0f;    // Center y-coordinate of the triangle

// LED positions
static float* ledPositionsX = nullptr;
static float* ledPositionsY = nullptr;

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect


void InwardCollapse_setup(SPIController& spiController)
{
    spiController.begin();

    // Initialize LED positions
    ledPositionsX = new float[NUM_LEDS];
    ledPositionsY = new float[NUM_LEDS];

    if (!ledPositionsX || !ledPositionsY) {
        Serial.println("Memory allocation failed for LED positions.");
        return;
    }

    int ledCount = 0;

    // Calculate LED positions and triangle center
    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];
        float y = (float)row; // Use row number as y-coordinate

        for (int i = 0; i < numLEDsInRow; i++) {
            int ledNumber = panelMapping[row][i];
            float x = (float)(i - (numLEDsInRow - 1) / 2.0f); // Center x at 0

            ledPositionsX[ledNumber] = x;
            ledPositionsY[ledNumber] = y;

            // Accumulate center position
            centerX += x;
            centerY += y;
            ledCount++;
        }
    }

    // Average center position
    centerX /= ledCount;
    centerY /= ledCount;
}

void InwardCollapse_loop(SPIController& spiController)
{
    uint8_t ledBuffer[NUM_LEDS][3];

    // Set all LEDs to off initially
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Update collapse phase
    collapsePhase += CollapseSpeed;
    if (collapsePhase > totalRows + 1) {
        collapsePhase = 0; // Reset to create looping animation
    }

    // Process LEDs for inward collapse
    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];
        float rowProgress = row + collapsePhase - totalRows;

        if (rowProgress >= 0 && rowProgress <= 1) {
            // LEDs from the edges collapse inward
            for (int i = 0; i < numLEDsInRow; i++) {
                float progress = (float)i / (numLEDsInRow - 1);
                int ledNumber = panelMapping[row][i];

                // Adjust intensity based on collapse progress
                float intensity = 1.0f - fabs(progress - 0.5f) * 2.0f; // Intensity peaks at the center
                intensity = intensity * (1.0f - rowProgress);         // Fade out as it collapses

                // Assign striped colors
                uint8_t red = (row % 2 == 0) ? (uint8_t)(intensity * 255) : 0;
                uint8_t green = (row % 2 == 1) ? (uint8_t)(intensity * 255) : 0;
                uint8_t blue = (uint8_t)(intensity * 127); // Constant blue for contrast

                ledBuffer[ledNumber][0] = red;
                ledBuffer[ledNumber][1] = green;
                ledBuffer[ledNumber][2] = blue;
            }
        }
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);
}
