#ifndef RIPPLE_H
#define RIPPLE_H

#include "SPIController.h"

// Ripple effect setup function
void Ripple_setup(SPIController& spiController);

// Ripple effect loop function to create wave or ripple effect
void Ripple_loop(SPIController& spiController, int numLEDs);

#endif // RIPPLE_H
