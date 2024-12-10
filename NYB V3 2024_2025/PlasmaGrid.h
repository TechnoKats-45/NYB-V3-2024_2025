#ifndef PLASMA_GRID_H
#define PLASMA_GRID_H

#include "SPIController.h"

// Function to initialize the Plasma Grid effect
void PlasmaGrid_setup(SPIController& spiController);

// Function to run the Plasma Grid effect
void PlasmaGrid_loop(SPIController& spiController);

#endif // PLASMA_GRID_H
