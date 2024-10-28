#ifndef COLORFADE_H
#define COLORFADE_H

#include "SPIController.h"

// Color fade setup function
void ColorFade_setup(SPIController& spiController);

// Color fade loop function to cycle through the rainbow
void ColorFade_loop(SPIController& spiController, int numLEDs);

#endif // COLORFADE_H
