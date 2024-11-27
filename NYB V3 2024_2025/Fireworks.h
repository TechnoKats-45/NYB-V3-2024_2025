#ifndef FIREWORKS_H
#define FIREWORKS_H

#include "SPIController.h"

// Setup function for Fireworks mode
void Fireworks_setup(SPIController& spiController);

// Loop function for Fireworks mode
void Fireworks_loop(SPIController& spiController);

#endif // FIREWORKS_H
