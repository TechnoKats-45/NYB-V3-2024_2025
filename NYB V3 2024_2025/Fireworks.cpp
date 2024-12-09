#include "Fireworks.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define FireworkDelay 50          // Delay between frame updates
#define FireworkTrailLength 5     // Length of the fading trail
#define MaxFireworks 5            // Maximum number of simultaneous fireworks
#define ExplosionDuration 200     // Duration of explosion before fading

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Struct to represent a single firework
struct Firework {
    int startRow;
    int ledPosition;
    int trailLength;
    bool exploded;
    unsigned long explosionStartTime;
    uint8_t color[3]; // RGB color
};

// Firework instances
Firework fireworks[MaxFireworks];

// Setup function for Fireworks mode
void Fireworks_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize fireworks
    for (int i = 0; i < MaxFireworks; i++) {
        fireworks[i] = {
            random(0, totalRows),   // Random start row
            0,                     // Initial position
            FireworkTrailLength,   // Set trail length
            false,                 // Not exploded yet
            0,                     // Explosion start time
            {random(100, 255), random(100, 255), random(100, 255)} // Random color
        };
    }
}

// Loop function for Fireworks mode
void Fireworks_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // Buffer for LED colors

    // Process each firework
    for (int i = 0; i < MaxFireworks; i++) {
        Firework& firework = fireworks[i];

        if (!firework.exploded) {
            // Create the trail for the firework
            for (int t = 0; t <= firework.trailLength; t++) {
                int trailRow = firework.startRow - t;
                if (trailRow < 0) break; // Stop if out of bounds

                for (int j = 0; j < rowSizes[trailRow]; j++) {
                    int ledNumber = panelMapping[trailRow][j];
                    uint8_t fadeFactor = 255 / (t + 1); // Dimmer for trailing effect
                    ledBuffer[ledNumber][0] = min(ledBuffer[ledNumber][0] + (firework.color[0] / fadeFactor), 255);
                    ledBuffer[ledNumber][1] = min(ledBuffer[ledNumber][1] + (firework.color[1] / fadeFactor), 255);
                    ledBuffer[ledNumber][2] = min(ledBuffer[ledNumber][2] + (firework.color[2] / fadeFactor), 255);
                }
            }

            // Move the firework up
            firework.startRow--;
            if (firework.startRow < firework.trailLength) {
                firework.exploded = true; // Mark as exploded
                firework.explosionStartTime = millis();
            }
        }
        else {
            // Explosion effect
            if (millis() - firework.explosionStartTime < ExplosionDuration) {
                for (int row = max(0, firework.startRow - 2); row <= min(totalRows - 1, firework.startRow + 2); row++) {
                    for (int j = 0; j < rowSizes[row]; j++) {
                        int ledNumber = panelMapping[row][j];
                        ledBuffer[ledNumber][0] = firework.color[0];
                        ledBuffer[ledNumber][1] = firework.color[1];
                        ledBuffer[ledNumber][2] = firework.color[2];
                    }
                }
            }
            else {
                // Reset firework after explosion
                firework.startRow = random(totalRows / 2, totalRows); // Start from the bottom half
                firework.ledPosition = 0;
                firework.exploded = false;
                firework.color[0] = random(100, 255);
                firework.color[1] = random(100, 255);
                firework.color[2] = random(100, 255);
            }
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(FireworkDelay); // Wait before the next frame
}
