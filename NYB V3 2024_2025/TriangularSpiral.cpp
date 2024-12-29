#include "TriangularSpiral.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Configuration
#define LEDDelay 50           // Delay between frame updates in milliseconds
#define SpiralSpeed 0.05      // Speed of the spiral motion
#define Loops 3               // Number of spiral loops
#define ColorCycleSpeed 0.02  // Rate of color cycling

static float spiralPhase = 0;  // Phase of the spiral animation
static float centerX = 0.0f;   // Center x-coordinate of the triangle
static float centerY = 0.0f;   // Center y-coordinate of the triangle

// Static arrays to hold LED positions
static float ledPositionsX[475];  // Statically allocated array for LED X positions
static float ledPositionsY[475];  // Statically allocated array for LED Y positions

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

void TriangularSpiral_setup(SPIController& spiController)
{
    spiController.begin();

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

void TriangularSpiral_loop(SPIController& spiController)
{
    uint8_t ledBuffer[NUM_LEDS][3];

    // Set all LEDs to off initially
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Update spiral phase
    spiralPhase += SpiralSpeed;
    if (spiralPhase > 2 * M_PI * Loops) {
        spiralPhase -= 2 * M_PI * Loops;
    }

    // Set LED colors based on spiral path
    for (int i = 0; i < NUM_LEDS; i++) {
        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;

        // Calculate distance and angle from the center
        float distance = sqrt(dx * dx + dy * dy);
        float angle = atan2(dy, dx);

        // Normalize angle to [0, 2π)
        while (angle < 0) angle += 2 * M_PI;

        // Calculate the position within the spiral
        float spiralPosition = fmod(angle + spiralPhase, 2 * M_PI);
        float expectedDistance = (spiralPosition / (2 * M_PI)) * (NUM_LEDS / Loops);

        // Check if the LED is near the spiral path
        if (fabs(distance - expectedDistance) < 1.0f) { // Tolerance for proximity
            float colorPhase = distance / (NUM_LEDS / Loops);
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
