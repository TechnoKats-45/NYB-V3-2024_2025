#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>
#include "InversePinwheel.h"

// Configuration
#define LEDDelay 50    // Delay between frame updates in milliseconds
#define SpokeSpeed 0.1 // Speed of rotation
#define SpokeWidth 1.0 // Width of each spoke
#define ColorCycleSpeed 0.02 // Rate of color cycling

static float angleOffset = 0; // Rotational offset for the pinwheel
static float centerX = 0.0f;  // Center x-coordinate of the triangle
static float centerY = 0.0f;  // Center y-coordinate of the triangle

static float brightnessScaling = 0.4; // Brightness scaling factor for the wave effect

// LED positions
static float* ledPositionsX = nullptr;
static float* ledPositionsY = nullptr;

void InversePinwheel_setup(SPIController& spiController)
{
    spiController.begin();

    // Initialize LED positions
    ledPositionsX = new float[NUM_LEDS];
    ledPositionsY = new float[NUM_LEDS];

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

void InversePinwheel_loop(SPIController& spiController)
{
    uint8_t ledBuffer[NUM_LEDS][3];

    // Set all LEDs to off initially
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Calculate the color cycle value
    static float colorPhase = 0;
    colorPhase += ColorCycleSpeed;
    if (colorPhase > 1.0f) {
        colorPhase -= 1.0f;
    }

    // Update angle offset for rotation
    angleOffset += SpokeSpeed;
    if (angleOffset > 2 * M_PI) {
        angleOffset -= 2 * M_PI;
    }

    // Set LED colors based on spoke positions
    for (int i = 0; i < NUM_LEDS; i++) {
        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;
        float angle = atan2(dy, dx) - angleOffset;

        // Normalize angle to [0, 2π)
        while (angle < 0) angle += 2 * M_PI;
        while (angle >= 2 * M_PI) angle -= 2 * M_PI;

        // Check if LED is within one of the spokes
        bool inSpoke = false;
        for (int spoke = 0; spoke < 3; spoke++) {
            float spokeAngle = spoke * 2 * M_PI / 3; // 120° spacing
            float diff = fabs(angle - spokeAngle);
            if (diff > M_PI) diff = 2 * M_PI - diff; // Account for wrap-around
            if (diff < SpokeWidth) {
                inSpoke = true;
                break;
            }
        }

        // If in spoke, set color
        if (inSpoke) {
            uint8_t red = (uint8_t)(sin(2 * M_PI * colorPhase) * 127 + 128);
            uint8_t green = (uint8_t)(sin(2 * M_PI * (colorPhase + 0.33)) * 127 + 128);
            uint8_t blue = (uint8_t)(sin(2 * M_PI * (colorPhase + 0.66)) * 127 + 128);

            ledBuffer[i][0] = red;
            ledBuffer[i][1] = green;
            ledBuffer[i][2] = blue;
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
