#ifndef DIAMOND_WEAVE_H
#define DIAMOND_WEAVE_H

#include "SPIController.h"

// Function to initialize the Diamond Weave effect
void DiamondWeave_setup(SPIController& spiController);

// Function to run the Diamond Weave effect
void DiamondWeave_loop(SPIController& spiController);

#endif // DIAMOND_WEAVE_H
