#ifndef SNOWFALL_H
#define SNOWFALL_H

#include "SPIController.h"

// Setup function for Snowfall mode
void Snowfall_setup(SPIController& spiController);

// Loop function for Snowfall mode
void Snowfall_loop(SPIController& spiController);

#endif // SNOWFALL_H
