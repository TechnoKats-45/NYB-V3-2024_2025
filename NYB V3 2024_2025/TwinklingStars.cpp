#include "TwinklingStars.h"
#include <Arduino.h>

void TwinklingStars_setup(SPIController& spiController) {
    spiController.begin();
}

void TwinklingStars_loop(SPIController& spiController, int numLEDs) {
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        int brightness = random(1, 31);  // Random brightness for twinkle effect
        uint8_t red = random(0, 50);     // Subtle white or light blue twinkles
        uint8_t green = random(0, 50);
        uint8_t blue = random(128, 255);

        spiController.sendColor(brightness, red, green, blue);
    }

    spiController.sendEndFrame(numLEDs);
    delay(200);  // Update twinkles every 200ms
}
