#include "Ripple.h"
#include <Arduino.h>

void Ripple_setup(SPIController& spiController) {
    spiController.begin();
}

void Ripple_loop(SPIController& spiController, int numLEDs) {
    static int center = 0;
    static int expanding = 1;

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        int distance = abs(center - i);
        uint8_t brightness = max(31 - (distance * 5), 0);  // Brightness decreases with distance
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 255;  // Blue ripple

        spiController.sendColor(brightness, red, green, blue);
    }

    spiController.sendEndFrame(numLEDs);

    // Update the ripple center
    center += expanding;
    if (center >= numLEDs || center <= 0) {
        expanding = -expanding;  // Reverse direction when hitting the edge
    }

    delay(50);
}
