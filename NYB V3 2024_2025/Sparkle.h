#ifndef SPARKLE_H
#define SPARKLE_H

#include "SPIController.h"

// Sparkle effect setup function
void Sparkle_setup(SPIController& spiController);

// Sparkle effect loop function to create random sparkles
void Sparkle_loop(SPIController& spiController, int numLEDs);

#endif // SPARKLE_H
