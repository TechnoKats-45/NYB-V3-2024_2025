#include "OrbitingPoints.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

#define LEDDelay 50  // Delay between frame updates in milliseconds
#define TrailFade 0.95f // Factor by which trails fade
#define OrbitSpeed 0.1f // Speed of orbit
#define SpiralStep 0.1f // Outward spiral step

static float centerX = 0.0f;
static float centerY = 0.0f;
static float angle = 0.0f;

// Arrays to hold LED positions
static float* ledPositionsX = nullptr;
static float* ledPositionsY = nullptr;

// Positions of the three orbiting points
static float pointRadius = 0.0f;

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

boolean firstTime;


void OrbitingPoints_setup(SPIController& spiController) 
{
    spiController.begin();

	firstTime = true;

    // Initialize LED positions
    ledPositionsX = new float[NUM_LEDS];
    ledPositionsY = new float[NUM_LEDS];

    // Calculate center and LED positions
    int maxRowSize = 0;
    int ledCount = 0;

    for (int row = 1; row < totalRows; row++) {
        if (rowSizes[row] > maxRowSize)
            maxRowSize = rowSizes[row];
    }

    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];
        float y = (float)row;

        for (int i = 0; i < numLEDsInRow; i++) {
            int ledNumber = panelMapping[row][i];
            float x = (float)(i - (numLEDsInRow - 1) / 2.0f);

            ledPositionsX[ledNumber] = x;
            ledPositionsY[ledNumber] = y;

            centerX += x;
            centerY += y;
            ledCount++;
        }
    }

    centerX /= ledCount;
    centerY /= ledCount;
}

void OrbitingPoints_loop(SPIController& spiController) 
{
    uint8_t ledBuffer[NUM_LEDS][3];
    bool resetPoints = false;

    if (firstTime)
    {
        // Explicitly set each LED color to off (0, 0, 0)
        for (int i = 0; i < NUM_LEDS; i++) {
            ledBuffer[i][0] = 0;  // Red
            ledBuffer[i][1] = 0;  // Green
            ledBuffer[i][2] = 0;  // Blue
        }

		firstTime = false;
    }

    // Fade trails
    for (int i = 0; i < NUM_LEDS; i++) {
        for (int c = 0; c < 3; c++) {
            ledBuffer[i][c] *= TrailFade;
        }
    }

    // Calculate new positions for orbiting points
    for (int i = 0; i < 3; i++) {
        float currentAngle = angle + i * (2 * PI / 3);
        float x = centerX + pointRadius * cos(currentAngle);
        float y = centerY + pointRadius * sin(currentAngle);

        // Find the closest LED to the calculated position
        int closestLED = -1;
        float minDist = INFINITY;
        for (int j = 0; j < NUM_LEDS; j++) {
            float dx = ledPositionsX[j] - x;
            float dy = ledPositionsY[j] - y;
            float dist = sqrt(dx * dx + dy * dy);
            if (dist < minDist) {
                minDist = dist;
                closestLED = j;
            }
        }

        // Set LED color for the current point
        if (closestLED >= 0) {
            if (i == 0) { // First point - Red
                ledBuffer[closestLED][0] = 255;
                ledBuffer[closestLED][1] = 0;
                ledBuffer[closestLED][2] = 0;
            }
            else if (i == 1) { // Second point - Green
                ledBuffer[closestLED][0] = 0;
                ledBuffer[closestLED][1] = 255;
                ledBuffer[closestLED][2] = 0;
            }
            else if (i == 2) { // Third point - Blue
                ledBuffer[closestLED][0] = 0;
                ledBuffer[closestLED][1] = 0;
                ledBuffer[closestLED][2] = 255;
            }

            // Check if the current point is touching the edge
            if (closestLED == panelMapping[24][1] || // Bottom-left corner
                closestLED == panelMapping[totalRows - 1][rowSizes[totalRows - 1] - 1] || // Bottom-right corner
                closestLED == panelMapping[1][0]) { // Top corner
                resetPoints = true;
            }
        }
    }

    // Update point radius and angle
    if (resetPoints) {
        pointRadius = 0.0f;  // Reset radius to start again
    }
    else {
        pointRadius += SpiralStep;
    }

    angle += OrbitSpeed;

    // Send LED data
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);
}

