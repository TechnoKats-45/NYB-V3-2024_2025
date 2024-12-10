#ifndef FIREFLY_SWARM_H
#define FIREFLY_SWARM_H

#include "SPIController.h"

// Function to initialize the Firefly Swarm effect
void FireflySwarm_setup(SPIController& spiController);

// Function to run the Firefly Swarm effect
void FireflySwarm_loop(SPIController& spiController);

#endif // FIREFLY_SWARM_H
