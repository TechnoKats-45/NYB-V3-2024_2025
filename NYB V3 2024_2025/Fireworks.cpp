#include "Fireworks.h"
#include <Arduino.h>

void Fireworks_setup(SPIController& spiController) {
    spiController.begin();
}

void Fireworks_loop(SPIController& spiController, int numLEDs) {
    int center = random(0, numLEDs);  // Random LED to act as the center of the firework
    uint8_t red = random(128, 256);   // Random bright colors for explosion
    uint8_t green = random(128, 256);
    uint8_t blue = random(128, 256);

    spiController.sendStartFrame();

    // Create the firework with expanding radius
    for (int radius = 0; radius < numLEDs / 2; radius++) {
        for (int i = 0; i < numLEDs; i++) {
            if (abs(center - i) == radius) {
                spiController.sendColor(31, red, green, blue);  // Bright explosion color
            }
            else {
                spiController.sendColor(1, 0, 0, 0);  // Off
            }
        }
        spiController.sendEndFrame(numLEDs);
        delay(50);  // Control the explosion speed
    }

    // Fade out the firework
    for (int brightness = 31; brightness > 0; brightness--) {
        spiController.sendStartFrame();
        for (int i = 0; i < numLEDs; i++) {
            spiController.sendColor(brightness, red, green, blue);  // Dimming explosion
        }
        spiController.sendEndFrame(numLEDs);
        delay(30);
    }
}
