#include "TriangularCheckerboard.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Configuration
#define LEDDelay 50           // Delay between frame updates in milliseconds
#define CheckerboardSize 3.0  // Size of each "checker" in the board
#define RotationSpeed 0.05    // Speed of checkerboard rotation
#define ColorCycleSpeed 0.02  // Rate of color cycling

static float rotationAngle = 0; // Current rotation angle
static float centerX = 0.0f;    // Center x-coordinate of the triangle
static float centerY = 0.0f;    // Center y-coordinate of the triangle

// LED positions
static float* ledPositionsX = nullptr;
static float* ledPositionsY = nullptr;

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect

void TriangularCheckerboard_setup(SPIController& spiController)
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

void TriangularCheckerboard_loop(SPIController& spiController)
{
    uint8_t ledBuffer[NUM_LEDS][3];

    // Set all LEDs to off initially
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Update rotation angle
    rotationAngle += RotationSpeed;
    if (rotationAngle > 2 * M_PI) {
        rotationAngle -= 2 * M_PI;
    }

    // Exclude LEDs in the middle panel (panel 16)
    const int* middlePanelLEDs = panelDefinitions[15]; // Panel 16 corresponds to index 15
    int middlePanelLEDCount = sizeof(panelDefinitions[15]) / sizeof(panelDefinitions[15][0]);

    // Calculate checkerboard pattern
    for (int i = 0; i < NUM_LEDS; i++) {
        // Skip LEDs in the middle panel
        bool isMiddlePanelLED = false;
        for (int j = 0; j < middlePanelLEDCount; j++) {
            if (middlePanelLEDs[j] == i) {
                isMiddlePanelLED = true;
                break;
            }
        }
        if (isMiddlePanelLED) continue;

        // Calculate LED position relative to the triangle center
        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;

        // Rotate LED positions
        float rotatedX = dx * cos(rotationAngle) - dy * sin(rotationAngle);
        float rotatedY = dx * sin(rotationAngle) + dy * cos(rotationAngle);

        // Determine checkerboard color based on LED position
        int checkerX = (int)(rotatedX / CheckerboardSize);
        int checkerY = (int)(rotatedY / CheckerboardSize);
        bool isDark = (checkerX + checkerY) % 2 == 0;

        // Assign colors
        if (!isDark) {
            float colorPhase = (float)i / NUM_LEDS + rotationAngle * ColorCycleSpeed;
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
        spiController.sendColor(255, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);
}

