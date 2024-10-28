#include "RainbowChase.h"
#include <Arduino.h>

void RainbowChase_setup(SPIController& spiController) {
    spiController.begin();
}

void RainbowChase_loop(SPIController& spiController, int numLEDs) {
    static float hueOffset = 0.0f;

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        uint8_t red, green, blue;
        spiController.hsvToRgb(hueOffset + (float)i / numLEDs, 1.0, 1.0, red, green, blue);
        spiController.sendColor(31, red, green, blue);
    }

    spiController.sendEndFrame(numLEDs);

    hueOffset += 0.02f;
    if (hueOffset >= 1.0f) {
        hueOffset -= 1.0f;  // Loop back the hue
    }

    delay(50);  // Speed of the chase
}
