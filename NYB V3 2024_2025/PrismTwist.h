#ifndef PRISM_TWIST_H
#define PRISM_TWIST_H

#include "SPIController.h"

// Function to initialize the Prism Twist effect
void PrismTwist_setup(SPIController& spiController);

// Function to run the Prism Twist effect
void PrismTwist_loop(SPIController& spiController);

#endif // PRISM_TWIST_H
