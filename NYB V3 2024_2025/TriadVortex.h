#ifndef TRIAD_VORTEX_H
#define TRIAD_VORTEX_H

#include "SPIController.h"

// Function to initialize the Triad Vortex effect
void TriadVortex_setup(SPIController& spiController);

// Function to run the Triad Vortex effect
void TriadVortex_loop(SPIController& spiController);

#endif // TRIAD_VORTEX_H
