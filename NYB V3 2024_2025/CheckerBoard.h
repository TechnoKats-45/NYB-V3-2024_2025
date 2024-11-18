#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H

#include "SPIController.h"

// Setup function for Checkerboard pattern
void Checkerboard_setup(SPIController& spiController);

// Loop function for Checkerboard pattern
void Checkerboard_loop(SPIController& spiController);

#endif // CHECKERBOARD_H
