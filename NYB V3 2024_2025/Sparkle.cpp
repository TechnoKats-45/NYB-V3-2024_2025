#include "Sparkle.h"
#include <Arduino.h>
#include "Globals.h"  // Include Globals to access MAX_BRIGHTNESS

void Sparkle_setup(SPIController& spiController) {
    spiController.begin();
}

void Sparkle_loop(SPIController& spiController, int numLEDs) {
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        if (random(0, 10) == 0) {
            spiController.sendColor(MAX_BRIGHTNESS, 255, 255, 255);  // Sparkling white
        }
        else {
            spiController.sendColor(0, 0, 0, 0);  // Off
        }
    }

    spiController.sendEndFrame(numLEDs);
    delay(100);
}
