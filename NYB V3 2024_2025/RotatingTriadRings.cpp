#include "RotatingTriadRings.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LED_DELAY 50        // Delay between frame updates in milliseconds
#define RING_SPEED 0.5f     // Speed of ring rotation
#define NUM_RINGS 6         // Number of rings for rotation (adjustable)

static float brightnessScaling = 1; // Brightness scaling factor

// Structure for each ring
struct Ring {
    float angle;  // Current angle of the ring (in radians)
    float speed;  // Angular speed of the ring
};

// Array of rings
static Ring rings[NUM_RINGS];

// Converts a hue value to RGB
static void setRGBFromHue(uint8_t hue, uint8_t& red, uint8_t& green, uint8_t& blue) {
    if (hue < 85) {
        red = hue * 3;
        green = 255 - hue * 3;
        blue = 0;
    }
    else if (hue < 170) {
        hue -= 85;
        red = 255 - hue * 3;
        green = 0;
        blue = hue * 3;
    }
    else {
        hue -= 170;
        red = 0;
        green = hue * 3;
        blue = 255 - hue * 3;
    }
}

// Initialize the rings
void RotatingTriadRings_setup(SPIController& spiController) {
    spiController.begin();

    // Initialize rings with increasing speeds
    for (int i = 0; i < NUM_RINGS; i++) {
        rings[i].angle = i * (TWO_PI / NUM_RINGS);  // Spread out angles
        rings[i].speed = RING_SPEED * (i + 1);      // Each ring has a unique speed
    }
}

void RotatingTriadRings_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Iterate over each ring and calculate positions
    for (int i = 0; i < NUM_RINGS; i++) {
        float angle = rings[i].angle;

        // For each ring, light up LEDs in corresponding rows
        if (i + 1 < totalRows) { // Ensure the ring corresponds to a valid row
            int numLEDsInRow = rowSizes[i + 1];
            for (int j = 0; j < numLEDsInRow; j++) {
                int ledIndex = panelMapping[i + 1][j];

                // Calculate color based on angle and LED position
                float hue = fmod((angle + j * (360.0f / numLEDsInRow)), 360.0f); // Circular hue mapping
                uint8_t red, green, blue;
                uint8_t hueAsInt = (uint8_t)(hue / 360.0f * 255);
                setRGBFromHue(hueAsInt, red, green, blue);

                // Set LED colors
                ledBuffer[ledIndex][0] = (uint8_t)(brightnessScaling * red);
                ledBuffer[ledIndex][1] = (uint8_t)(brightnessScaling * green);
                ledBuffer[ledIndex][2] = (uint8_t)(brightnessScaling * blue);
            }
        }

        // Update ring angle for rotation
        rings[i].angle += rings[i].speed * LED_DELAY / 1000.0f; // Adjust speed by delay
        if (rings[i].angle >= TWO_PI) {
            rings[i].angle -= TWO_PI;
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int k = 0; k < NUM_LEDS; k++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[k][0], ledBuffer[k][1], ledBuffer[k][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LED_DELAY); // Delay for smooth animation
}
