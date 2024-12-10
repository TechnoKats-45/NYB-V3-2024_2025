#include "RadiatingRainbow.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

static float brightnessScaling = 0.25; // Adjust brightness scaling for the effect
static float animationSpeed = 0.05;    // Speed of the outward hue movement
static float frameOffset = 0;          // Tracks the frame for movement

// Setup function for the Radiating Rainbow effect
void RadiatingRainbow_setup(SPIController& spiController) {
    spiController.begin();
}

// Function to calculate the distance of an LED from the center
float calculateDistance(int x1, int y1, int x2, int y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// Function to convert HSV to RGB
void hsvToRgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    int i = floor(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    switch (i % 6) {
    case 0: r = v * 255; g = t * 255; b = p * 255; break;
    case 1: r = q * 255; g = v * 255; b = p * 255; break;
    case 2: r = p * 255; g = v * 255; b = t * 255; break;
    case 3: r = p * 255; g = q * 255; b = v * 255; break;
    case 4: r = t * 255; g = p * 255; b = v * 255; break;
    case 5: r = v * 255; g = p * 255; b = q * 255; break;
    }
}

// Loop function for the Radiating Rainbow effect
void RadiatingRainbow_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3];

    // Define the center of the panel
    int centerX = totalRows / 2;
    int centerY = rowSizes[centerX] / 2;

    // Calculate the maximum distance from the center
    float maxDistance = 0;
    for (int row = 0; row < totalRows; row++) {
        for (int col = 0; col < rowSizes[row]; col++) {
            float dist = calculateDistance(centerX, centerY, row, col);
            if (dist > maxDistance) maxDistance = dist;
        }
    }

    // Set the LED colors based on distance and frame offset for animation
    for (int row = 0; row < totalRows; row++) {
        for (int col = 0; col < rowSizes[row]; col++) {
            int ledNumber = panelMapping[row][col];
            float dist = calculateDistance(centerX, centerY, row, col);

            // Map the distance and frame offset to a hue (0-1)
            float hue = fmod(dist / maxDistance + frameOffset, 1.0);

            // Convert HSV to RGB
            uint8_t r, g, b;
            hsvToRgb(hue, 1.0, 1.0, r, g, b);

            // Apply brightness scaling
            ledBuffer[ledNumber][0] = r;
            ledBuffer[ledNumber][1] = g;
            ledBuffer[ledNumber][2] = b;
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int i = 0; i < NUM_LEDS; i++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[i][0], ledBuffer[i][1], ledBuffer[i][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    // Increment the frame offset for outward animation
    frameOffset += animationSpeed;
    if (frameOffset >= 1.0) frameOffset -= 1.0; // Wrap frame offset for seamless looping

    delay(50); // Adjust delay for animation speed
}
