#include "WaveEffect.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

#define WaveDelay 50           // Delay between wave updates in milliseconds
#define WaveAmplitude 3        // Amplitude of the wave (number of rows up/down)
#define WaveFrequency 0.5      // Frequency of the wave (controls how many LEDs fit in one cycle)
#define WaveSpeed 0.2          // Speed of the wave (how fast it moves)

float static brightnessScaling = 0.5; // Brightness scaling factor for the wave effect

// Struct to represent a wave
struct Wave {
    float position; // Current position in the wave
    uint8_t color[3]; // RGB color of the wave
    int direction; // 1 for right-to-left, -1 for left-to-right
};

// Number of simultaneous waves
#define NumWaves 2
Wave waves[NumWaves];

// Setup function for WaveEffect mode
void WaveEffect_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize waves with random colors and directions
    for (int i = 0; i < NumWaves; i++) {
        waves[i] = {
            0.0f, // Initial position
            {random(100, 255), random(100, 255), random(100, 255)}, // Random color
            random(0, 2) == 0 ? 1 : -1 // Random direction
        };
    }
}

// Loop function for WaveEffect mode
void WaveEffect_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // RGB buffer for LED colors

    // Process each wave
    for (int i = 0; i < NumWaves; i++) {
        Wave& wave = waves[i];

        // Update the wave position
        wave.position += wave.direction * WaveSpeed;

        // Wrap the wave position to keep it within the panel width
        if (wave.position >= totalRows) {
            wave.position = 0;
        }
        else if (wave.position < 0) {
            wave.position = totalRows - 1;
        }

        // Draw the wave on the LEDs
        for (int row = 0; row < totalRows; row++) {
            float distance = fabs(row - wave.position); // Distance from the wave's center
            if (distance <= WaveAmplitude) {
                float intensity = cos((distance / WaveAmplitude) * PI / 2); // Smooth fade using cosine
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

    delay(WaveDelay); // Wait before the next frame
}
