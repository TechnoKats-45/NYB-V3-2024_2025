#include "Aurora.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Use a static array for phaseOffsets instead of dynamically allocating
static float phaseOffsets[475] = { 0 };  // Statically allocated phase offsets

static float brightnessScaling = 0.25; // Brightness scaling factor for the wave effect

// Gradient colors for the aurora (transitioning smoothly between green, blue, and purple)
const uint8_t AuroraColors[7][3] = {
    {0, 255, 0},     // Green
    {0, 191, 127},   // Light Green
    {0, 127, 255},   // Cyan
    {0, 0, 255},     // Blue
    {75, 0, 130},    // Indigo
    {148, 0, 211},   // Violet
    {255, 0, 127}    // Pink
};

// Setup function for AuroraEffect mode
void AuroraEffect_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize phase offsets for each LED (now in a static array)
    for (int i = 0; i < NUM_LEDS; i++) {
        phaseOffsets[i] = random(0, 1000) / 100.0; // Random initial phases for smooth animations
    }
}

// Loop function for AuroraEffect mode
void AuroraEffect_loop(SPIController& spiController) {
    static float time = 0; // Time variable to create smooth animations
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // RGB buffer for LED colors

    time += AuroraSpeed;

    // Generate undulating color ribbons
    for (int row = 0; row < totalRows; row++) {
        for (int col = 0; col < rowSizes[row]; col++) {
            int ledNumber = panelMapping[row][col];

            // Calculate the color intensity based on the sine wave
            float phase = phaseOffsets[ledNumber] + row * AuroraWaveFrequency + time;
            float intensity = (sin(phase) + 1.0) / 2.0; // Scales intensity between 0 and 1

            // Blend between two colors based on intensity
            int colorIndex = (int)(phase) % 7;
            int nextColorIndex = (colorIndex + 1) % 7;

            // Smoothly interpolate between colors
            for (int c = 0; c < 3; c++) {
                ledBuffer[ledNumber][c] = AuroraColors[colorIndex][c] * (1.0 - intensity)
                    + AuroraColors[nextColorIndex][c] * intensity;
                ledBuffer[ledNumber][c] = min(ledBuffer[ledNumber][c] * AuroraIntensity, 255.0f);
            }
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS,
            ledBuffer[j][0],
            ledBuffer[j][1],
            ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(10); // Slight delay for smooth animations
}

// Cleanup function (left empty since we no longer allocate dynamically)
void AuroraEffect_cleanup() {
    // Nothing to free: phaseOffsets is statically allocated
}
