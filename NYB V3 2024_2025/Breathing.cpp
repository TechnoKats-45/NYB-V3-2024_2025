#include "Breathing.h"
#include <Arduino.h>
#include <math.h>

void Breathing_setup(SPIController& spiController) {
    spiController.begin();
}

void Breathing_loop(SPIController& spiController, int numLEDs) {
    static float breathPhase = 0.0f;

    float brightnessFactor = (sin(breathPhase) + 1) / 2.0f;  // Generate sine wave [0,1]
    uint8_t brightness = brightnessFactor * 31;  // Map to brightness scale

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) {
        spiController.sendColor(brightness, 255, 0, 0);  // Red color pulse
    }

    spiController.sendEndFrame(numLEDs);

    breathPhase += 0.05f;  // Adjust breathing speed
    if (breathPhase > TWO_PI) {
        breathPhase -= TWO_PI;
    }

    delay(30);
}
