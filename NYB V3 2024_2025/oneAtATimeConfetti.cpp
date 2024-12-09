#include "oneAtATimeConfetti.h"
#include <Arduino.h>
#include "Globals.h"  // Include Globals to access MAX_BRIGHTNESS

#define confettiDelay 5  // Delay between confetti steps in milliseconds

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Array to store the current color of each LED
struct LEDColor {
    uint8_t red;
    uint8_t green;
    uint8_t blue;
};

LEDColor* ledColors = nullptr;

// Confetti setup function
void OneAtATimeConfetti_setup(SPIController& spiController, int numLEDs)
{
    spiController.begin();
    randomSeed(analogRead(A0));  // Seed random generator

    // Initialize LED color array
    ledColors = new LEDColor[numLEDs];
    for (int i = 0; i < numLEDs; i++) {
        ledColors[i] = { 0, 0, 0 };  // Start with LEDs off
    }
}

// Confetti loop function
void OneAtATimeConfetti_loop(SPIController& spiController, int numLEDs)
{
    spiController.sendStartFrame();

    // Choose a random LED to update
    int ledIndex = random(0, numLEDs);

    // Update the chosen LED with a new color
    if (random(0, 4) == 0) {
        ledColors[ledIndex] = { 0, 0, 0 };  // 25% chance of no color
    }
    else {
        ledColors[ledIndex].red = random(0, 256);
        ledColors[ledIndex].green = random(0, 256);
        ledColors[ledIndex].blue = random(0, 256);  // 75% chance of random color
    }

    // Send color data for each LED, keeping their previous state
    for (int i = 0; i < numLEDs; i++) {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledColors[i].red, ledColors[i].green, ledColors[i].blue);  // Use MAX_BRIGHTNESS
    }

    spiController.sendEndFrame(numLEDs);
    delay(confettiDelay);  // Delay for stability
}
