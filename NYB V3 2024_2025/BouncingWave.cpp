#include "BouncingWave.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Array for ROYGBIV colors (Red, Orange, Yellow, Green, Blue, Indigo, Violet)
const uint8_t ROYGBIV[7][3] = {
    {255, 0, 0},     // Red
    {255, 127, 0},   // Orange
    {255, 255, 0},   // Yellow
    {0, 255, 0},     // Green
    {0, 0, 255},     // Blue
    {75, 0, 130},    // Indigo
    {148, 0, 211}    // Violet
};

BouncingWave bouncingWaves[NumBouncingWaves];

// Setup function for BouncingWaveEffect mode
void BouncingWaveEffect_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize bouncing waves with initial color and direction
    for (int i = 0; i < NumBouncingWaves; i++) {
        bouncingWaves[i] = {
            random(0, totalRows),         // Random initial position
            {ROYGBIV[i % 7][0], ROYGBIV[i % 7][1], ROYGBIV[i % 7][2]}, // Initialize with ROYGBIV colors
            random(0, 2) == 0 ? 1 : -1   // Random direction
        };
    }
}

// Helper function to move to the next color in ROYGBIV
void nextColor(BouncingWave& wave, int& currentColorIndex) {
    currentColorIndex = (currentColorIndex + 1) % 7; // Cycle through the 7 colors
    wave.color[0] = ROYGBIV[currentColorIndex][0];
    wave.color[1] = ROYGBIV[currentColorIndex][1];
    wave.color[2] = ROYGBIV[currentColorIndex][2];
}

// Loop function for BouncingWaveEffect mode
void BouncingWaveEffect_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // RGB buffer for LED colors

    // Store current color index for each wave
    static int currentColorIndex[NumBouncingWaves] = { 0 };

    // Process each bouncing wave
    for (int i = 0; i < NumBouncingWaves; i++) {
        BouncingWave& wave = bouncingWaves[i];

        // Update the wave position
        wave.position += wave.direction * BounceWaveSpeed;

        // Bounce the wave off the top and bottom, cycling its color on bounce
        if (wave.position >= totalRows - 1) {
            wave.position = totalRows - 1;
            wave.direction = -1; // Reverse direction to bounce upward
            nextColor(wave, currentColorIndex[i]); // Change to the next color
        }
        else if (wave.position <= 0) {
            wave.position = 0;
            wave.direction = 1; // Reverse direction to bounce downward
            nextColor(wave, currentColorIndex[i]); // Change to the next color
        }

        // Draw the wave on the LEDs
        for (int row = 0; row < totalRows; row++) {
            float distance = fabs(row - wave.position); // Distance from the wave's center
            if (distance <= BounceWaveAmplitude) {
                float intensity = cos((distance / BounceWaveAmplitude) * PI / 2); // Smooth fade using cosine
                for (int col = 0; col < rowSizes[row]; col++) {
                    int ledNumber = panelMapping[row][col];
                    ledBuffer[ledNumber][0] = min(ledBuffer[ledNumber][0] + wave.color[0] * intensity, 255.0f);
                    ledBuffer[ledNumber][1] = min(ledBuffer[ledNumber][1] + wave.color[1] * intensity, 255.0f);
                    ledBuffer[ledNumber][2] = min(ledBuffer[ledNumber][2] + wave.color[2] * intensity, 255.0f);
                }
            }
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(BounceWaveDelay); // Wait before the next frame
}