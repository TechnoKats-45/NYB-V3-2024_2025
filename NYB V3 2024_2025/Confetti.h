#ifndef CONFETTI_H
#define CONFETTI_H

#include "SPIController.h"

// Confetti setup function
void Confetti_setup(SPIController& spiController);

// Confetti loop function to generate random confetti patterns
void Confetti_loop(SPIController& spiController, int numLEDs);

#endif // CONFETTI_H
