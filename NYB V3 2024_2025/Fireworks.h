#ifndef FIREWORKS_H
#define FIREWORKS_H

#include "SPIController.h"

// Fireworks setup function
void Fireworks_setup(SPIController& spiController);

// Fireworks loop function to create random fireworks effects
void Fireworks_loop(SPIController& spiController, int numLEDs);

#endif // FIREWORKS_H
