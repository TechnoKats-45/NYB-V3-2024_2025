#ifndef Fire_H
#define Fire_H

#include "SPIController.h"

// Fire effect setup function
void FireEffect_setup(SPIController& spiController);

// Fire effect loop function to create a dynamic fire effect
void FireEffect_loop(SPIController& spiController, int numLEDs);

#endif // Fire_H
