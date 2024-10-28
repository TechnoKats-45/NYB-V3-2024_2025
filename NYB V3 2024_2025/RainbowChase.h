#ifndef RAINBOWCHASE_H
#define RAINBOWCHASE_H

#include "SPIController.h"

// Rainbow chase setup function
void RainbowChase_setup(SPIController& spiController);

// Rainbow chase loop function to create a rainbow chasing effect
void RainbowChase_loop(SPIController& spiController, int numLEDs);

#endif // RAINBOWCHASE_H
