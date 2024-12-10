#include "GearburstPulse.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

#define LED_DELAY 50      // Delay between frame updates in milliseconds
#define GEAR_STEPS 10     // Number of steps for gear rotation
#define MAX_RADIUS 20.0f  // Maximum radius of gear pattern

static float* ledPositionsX = nullptr; // X-coordinates of LEDs
static float* ledPositionsY = nullptr; // Y-coordinates of LEDs
static float centerX = 0.0f;           // X-coordinate of center
static float centerY = 0.0f;           // Y-coordinate of center
static float rotationAngle = 0.0f;     // Current angle of rotation for symmetry

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect


void TechnoSwirl_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize LED positions
    ledPositionsX = new float[NUM_LEDS];
    ledPositionsY = new float[NUM_LEDS];

    // Calculate LED positions and center
    int ledCount = 0;
    for (int row = 0; row < totalRows; row++) {
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

    // Calculate average center position
    centerX /= ledCount;
    centerY /= ledCount;
}

void TechnoSwirl_loop(SPIController& spiController) {
    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[NUM_LEDS][3];
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 255; // Default to red background
        ledBuffer[i][1] = 0;   // Green off
        ledBuffer[i][2] = 0;   // Blue off
    }

    // Update rotation angle
    rotationAngle += (360.0f / GEAR_STEPS);
    if (rotationAngle >= 360.0f) rotationAngle -= 360.0f;

    // Calculate LED colors for the gear pattern
    for (int i = 0; i < NUM_LEDS; i++) {
        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;
        float distance = sqrt(dx * dx + dy * dy);

        // Check if the LED falls within the gear radius
        if (distance <= MAX_RADIUS) {
            // Calculate angular position for symmetry
            float angle = atan2(dy, dx) * (180.0f / PI);
            if (angle < 0) angle += 360.0f;

            // Apply rotation and symmetry
            angle = fmod(angle + rotationAngle, 360.0f);

            // Create gear teeth by segmenting angle
            if (fmod(angle, 30.0f) < 15.0f) { // Teeth pattern
                ledBuffer[i][0] = 0;   // Red off
                ledBuffer[i][1] = 0;   // Green off
                ledBuffer[i][2] = 255; // Blue on
            }
        }
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LED_DELAY); // Delay before the next frame update
}
