#include "PulsingWaves.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Configuration
#define LEDDelay 50       // Delay between frame updates in milliseconds
#define WaveSpeed 0.25    // Speed of wave propagation
#define MaxRadius 15.0    // Maximum radius of the wave
#define ColorCycleSpeed 0.01 // Rate of color cycling

static float wavePhase = 0;   // Phase of the wave
static float centerX = 0.0f;  // Center x-coordinate of the triangle
static float centerY = 0.0f;  // Center y-coordinate of the triangle

static float brightnessScaling = .25; // Brightness scaling factor for the wave effect

// Static arrays to hold LED positions
static float ledPositionsX[475];  // Statically allocated array for LED X positions
static float ledPositionsY[475];  // Statically allocated array for LED Y positions


void PulsingWaves_setup(SPIController& spiController)
{
    spiController.begin();

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

void PulsingWaves_loop(SPIController& spiController)
{
    uint8_t ledBuffer[NUM_LEDS][3];

    // Set all LEDs to off initially
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Update wave phase
    wavePhase += WaveSpeed;
    if (wavePhase > 2 * M_PI) {
        wavePhase -= 2 * M_PI;
    }

    // Set LED colors based on pulsing wave
    for (int i = 0; i < NUM_LEDS; i++) {
        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;
        float distance = sqrt(dx * dx + dy * dy);

        // Calculate brightness as a sinusoidal function of distance and wave phase
        float waveValue = sin(2 * M_PI * (distance / MaxRadius) + wavePhase);
        waveValue = (waveValue + 1) / 2; // Normalize to [0, 1]

        // Cycle through colors
        uint8_t red = (uint8_t)(waveValue * 255);
        uint8_t green = (uint8_t)((1.0 - waveValue) * 255);
        uint8_t blue = (uint8_t)(sin(2 * M_PI * (waveValue + ColorCycleSpeed)) * 127 + 128);

        ledBuffer[i][0] = red;
        ledBuffer[i][1] = green;
        ledBuffer[i][2] = blue;
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);
}
