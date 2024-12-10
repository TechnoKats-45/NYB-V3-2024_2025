#include "EquilateralRipple.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Configuration
#define LEDDelay 50       // Delay between frame updates in milliseconds
#define RippleSpeed 0.2   // Speed at which ripples expand
#define MaxRadius 20.0    // Maximum radius for the ripple effect
#define FadeFactor 0.8    // Fade intensity of previous ripples
#define ColorCycleSpeed 0.02 // Rate of color cycling

static float ripplePhase = 0;  // Phase of the ripple animation
static float centerX = 0.0f;   // Center x-coordinate of the triangle
static float centerY = 0.0f;   // Center y-coordinate of the triangle

// LED positions
static float* ledPositionsX = nullptr;
static float* ledPositionsY = nullptr;

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect


void EquilateralRipple_setup(SPIController& spiController)
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

void EquilateralRipple_loop(SPIController& spiController)
{
    uint8_t ledBuffer[NUM_LEDS][3];

    // Set all LEDs to off initially
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0; // Red
        ledBuffer[i][1] = 0; // Green
        ledBuffer[i][2] = 0; // Blue
    }

    // Update ripple phase
    ripplePhase += RippleSpeed;
    if (ripplePhase > MaxRadius) {
        ripplePhase -= MaxRadius;
    }

    // Set LED colors based on equilateral ripple effect
    for (int i = 0; i < NUM_LEDS; i++) {
        float dx = ledPositionsX[i] - centerX;
        float dy = ledPositionsY[i] - centerY;
        float distance = sqrt(dx * dx + dy * dy);

        // Determine which "ring" the LED belongs to
        float ringPosition = fmod(distance - ripplePhase, MaxRadius / 3);
        if (ringPosition < 0) ringPosition += MaxRadius / 3;

        // Calculate brightness based on proximity to the ring
        float brightness = 1.0f - (ringPosition / (MaxRadius / 3));
        if (brightness < 0.0f) brightness = 0.0f;

        // Fade previous ripples
        brightness = pow(brightness, FadeFactor);

        // Cycle through colors
        uint8_t red = (uint8_t)(brightness * (sin(2 * M_PI * ripplePhase / MaxRadius + 0) * 127 + 128));
        uint8_t green = (uint8_t)(brightness * (sin(2 * M_PI * ripplePhase / MaxRadius + 0.33) * 127 + 128));
        uint8_t blue = (uint8_t)(brightness * (sin(2 * M_PI * ripplePhase / MaxRadius + 0.66) * 127 + 128));

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
