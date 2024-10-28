#ifndef BREATHING_H
#define BREATHING_H

#include "SPIController.h"

// Breathing effect setup function
void Breathing_setup(SPIController& spiController);

// Breathing effect loop function to create smooth pulsing of LEDs
void Breathing_loop(SPIController& spiController, int numLEDs);

#endif // BREATHING_H
