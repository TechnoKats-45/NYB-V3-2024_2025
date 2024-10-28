#ifndef TWINKLINGSTARS_H
#define TWINKLINGSTARS_H

#include "SPIController.h"

// Twinkling stars setup function
void TwinklingStars_setup(SPIController& spiController);

// Twinkling stars loop function to create random twinkling stars
void TwinklingStars_loop(SPIController& spiController, int numLEDs);

#endif // TWINKLINGSTARS_H
