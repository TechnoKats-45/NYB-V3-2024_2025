#include "GalaxySwirl.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define GalaxySwirlDelay 50 // Delay between frame updates in milliseconds
#define SwirlSpeed .25      // Speed of the rotation
#define SwirlRadius 10      // Radius of the swirling effect

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Variables for the swirl effect
float swirlAngle = 0.0;       // Current angle of the swirl
const int centerX = totalRows / 2; // Center row of the panel
const int centerY = rowSizes[centerX] / 2; // Center column of the panel

// Setup function for Galaxy Swirl mode
void GalaxySwirl_setup(SPIController& spiController) {
    spiController.begin();
    swirlAngle = 0.0; // Initialize the swirl angle
}

// Loop function for Galaxy Swirl mode
void GalaxySwirl_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // RGB buffer for LED colors

    // Update the swirl angle
    swirlAngle += SwirlSpeed;
    if (swirlAngle >= TWO_PI) {
        swirlAngle -= TWO_PI; // Wrap the angle to stay within 0 to 2*PI
    }

    // Calculate the swirl pattern
    for (int r = 0; r < totalRows; r++) {
        for (int c = 0; c < rowSizes[r]; c++) {
            int ledNumber = panelMapping[r][c];

            // Calculate the distance and angle relative to the center
            float dx = r - centerX;
            float dy = c - centerY;
            float distance = sqrt(dx * dx + dy * dy);
            float angle = atan2(dy, dx) + swirlAngle;

            // Calculate the brightness based on distance
            float brightness = max(0.0, SwirlRadius - distance) / SwirlRadius;

            // Assign colors based on the angle
            uint8_t red = (sin(angle) + 1) * 127 * brightness;
            uint8_t green = (sin(angle + TWO_PI / 3) + 1) * 127 * brightness;
            uint8_t blue = (sin(angle + 2 * TWO_PI / 3) + 1) * 127 * brightness;

            // Set the LED color in the buffer
            ledBuffer[ledNumber][0] = red;
            ledBuffer[ledNumber][1] = green;
            ledBuffer[ledNumber][2] = blue;
        }
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(GalaxySwirlDelay); // Wait before the next frame
}
