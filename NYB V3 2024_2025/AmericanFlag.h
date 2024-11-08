#ifndef AMERICAN_FLAG_H
#define AMERICAN_FLAG_H

#include "SPIController.h"

// Setup function for the American flag LED pattern
void AmericanFlag_setup(SPIController& spiController);

// Loop function for the American flag LED pattern
void AmericanFlag_loop(SPIController& spiController);

#endif
