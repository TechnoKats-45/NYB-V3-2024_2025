#include "TwinklingStars.h"
#include <Arduino.h>

void TwinklingStars_setup(SPIController& spiController) {
    spiController.begin();
}

void TwinklingStars_loop(SPIController& spiController, int numLEDs) {
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        int brightness = random(40, 80);   // Higher brightness range for intense colors
        uint8_t red = random(180, 255);    // Maximize red to bring out purple
        uint8_t green = random(0, 10);     // Almost no green for a pure purple tone
        uint8_t blue = random(200, 255);   // High blue values for vibrant purple

        spiController.sendColor(brightness, red, green, blue);
    }

    spiController.sendEndFrame(numLEDs);
    delay(200);  // Update twinkles every 200ms
}
