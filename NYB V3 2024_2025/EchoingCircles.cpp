#include "EchoingCircles.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

#define LEDDelay 50  // Delay between frame updates in milliseconds
#define MaxCircles 3 // Maximum number of circles active at once

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Structure to represent an expanding circle
struct Circle {
    float radius;
    float speed;
    bool active;
};

// Array to hold circles
static Circle circles[MaxCircles];

// Variables for center position
static float centerX = 0.0f;
static float centerY = 0.0f;

// Static arrays to hold LED positions
static float ledPositionsX[475];  // Statically allocated array for LED X positions
static float ledPositionsY[475];  // Statically allocated array for LED Y positions


void EchoingCircles_setup(SPIController& spiController)
{
    spiController.begin();

    // Initialize circles
    for (int i = 0; i < MaxCircles; i++) {
        circles[i].radius = -i * 5.0f;  // Start circles at different negative radii
        circles[i].speed = 0.5f;        // Speed at which circles expand
        circles[i].active = true;
    }

    // Calculate center and LED positions
    int maxRowSize = 0;
    for (int row = 1; row < totalRows; row++) {
        if (rowSizes[row] > maxRowSize)
            maxRowSize = rowSizes[row];
    }

    int ledCount = 0;

    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];
        float y = (float)row;  // Use row number as y-coordinate

        for (int i = 0; i < numLEDsInRow; i++) {
            int ledNumber = panelMapping[row][i];
            float x = (float)(i - (numLEDsInRow - 1) / 2.0f);  // Center x at 0

            ledPositionsX[ledNumber] = x;
            ledPositionsY[ledNumber] = y;

            // Accumulate center position
            centerX += x;
            centerY += y;
            ledCount++;
        }
    }

    // Calculate average center position
    centerX /= ledCount;
    centerY /= ledCount;
}

void EchoingCircles_loop(SPIController& spiController)
{
    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[NUM_LEDS][3];

    // Explicitly set each LED color to off (0, 0, 0)
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0;  // Red
        ledBuffer[i][1] = 0;  // Green
        ledBuffer[i][2] = 0;  // Blue
    }

    // Set the first LED (status LED) to green
    int statusLED = panelMapping[0][0];
    ledBuffer[statusLED][0] = 0;   // Red
    ledBuffer[statusLED][1] = 255; // Green
    ledBuffer[statusLED][2] = 0;   // Blue

    // Update circles
    for (int c = 0; c < MaxCircles; c++) {
        if (circles[c].active) {
            circles[c].radius += circles[c].speed;

            // Reset circle if it exceeds maximum radius
            if (circles[c].radius > 20.0f) {  // Adjust based on panel size
                circles[c].radius = 0.0f;
            }
        }
    }

    // Set LED colors based on circle radii
    for (int i = 0; i < NUM_LEDS; i++) {
        // Skip the status LED
        if (i == statusLED) continue;

        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;
        float distance = sqrt(dx * dx + dy * dy);

        float brightness = 0.0f;

        // Check each circle
        for (int c = 0; c < MaxCircles; c++) {
            if (circles[c].active) {
                float diff = fabs(distance - circles[c].radius);
                if (diff < 1.0f) {
                    brightness += (1.0f - diff);  // Peak brightness at the circle line
                }
            }
        }

        // Clamp brightness between 0 and 1
        if (brightness > 1.0f) brightness = 1.0f;

        // Set color (e.g., blue)
        uint8_t intensity = (uint8_t)(brightness * 255);
        ledBuffer[i][0] = 0;          // Red
        ledBuffer[i][1] = 0;          // Green
        ledBuffer[i][2] = intensity;  // Blue
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);  // Delay before the next frame update
}

