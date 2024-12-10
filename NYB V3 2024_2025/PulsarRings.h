#ifndef PULSAR_RINGS_H
#define PULSAR_RINGS_H

#include "SPIController.h"

// Function to initialize the Pulsar Rings effect
void PulsarRings_setup(SPIController& spiController);

// Function to run the Pulsar Rings effect
void PulsarRings_loop(SPIController& spiController);

#endif // PULSAR_RINGS_H
