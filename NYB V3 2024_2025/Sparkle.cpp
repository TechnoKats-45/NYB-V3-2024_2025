#include "Sparkle.h"
#include <Arduino.h>

void Sparkle_setup(SPIController& spiController) {
    spiController.begin();
}

void Sparkle_loop(SPIController& spiController, int numLEDs) {
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        if (random(0, 10) == 0) {
            spiController.sendColor(31, 255, 255, 255);  // Sparkling white
        }
        else {
            spiController.sendColor(1, 0, 0, 0);  // Off
        }
    }

    spiController.sendEndFrame(numLEDs);
    delay(100);
}
