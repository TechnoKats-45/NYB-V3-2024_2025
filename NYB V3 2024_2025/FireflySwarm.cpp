#include "FireflySwarm.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define FIREFLY_DELAY 100     // Delay in milliseconds between updates
#define NUM_FIREFLIES 20     // Number of fireflies active at a time
#define FLICKER_CHANCE 20    // Percentage chance of flicker per update
#define FIREFLY_SPEED 3      // Speed of firefly movement

static float brightnessScaling = 1; // Adjust the overall brightness

struct Firefly {
    int ledIndex;
    int brightness;
    bool flickering;
};

Firefly fireflies[NUM_FIREFLIES];

// Function to randomly place fireflies
void initializeFireflies() {
    for (int i = 0; i < NUM_FIREFLIES; i++) {
        fireflies[i].ledIndex = random(0, NUM_LEDS);
        fireflies[i].brightness = random(100, MAX_BRIGHTNESS);
        fireflies[i].flickering = false;
    }
}

void FireflySwarm_setup(SPIController& spiController) {
    spiController.begin();
    initializeFireflies();
}

void FireflySwarm_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Update firefly positions and states
    for (int i = 0; i < NUM_FIREFLIES; i++) {
        // Random chance to flicker
        if (random(100) < FLICKER_CHANCE) {
            fireflies[i].flickering = !fireflies[i].flickering;
            fireflies[i].brightness = random(100, MAX_BRIGHTNESS);
        }

        // If flickering, dim the brightness
        int brightness = fireflies[i].flickering ? fireflies[i].brightness / 2 : fireflies[i].brightness;

        // Set firefly color (yellowish)
        ledBuffer[fireflies[i].ledIndex][0] = (uint8_t)(brightnessScaling * brightness);
        ledBuffer[fireflies[i].ledIndex][1] = (uint8_t)(brightnessScaling * brightness * 0.8);
        ledBuffer[fireflies[i].ledIndex][2] = 0;

        // Move firefly to a new position
        fireflies[i].ledIndex = (fireflies[i].ledIndex + random(-FIREFLY_SPEED, FIREFLY_SPEED)) % NUM_LEDS;
        if (fireflies[i].ledIndex < 0) {
            fireflies[i].ledIndex += NUM_LEDS;
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(FIREFLY_DELAY); // Delay for smooth animation
}
