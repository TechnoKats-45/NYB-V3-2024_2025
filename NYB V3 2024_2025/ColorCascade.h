#ifndef ColorCascade_H
#define ColorCascade_H

#include "SPIController.h"

// Color fade setup function
void ColorCascade_setup(SPIController& spiController);

// Color fade loop function to cycle through the rainbow
void ColorCascade_loop(SPIController& spiController, int numLEDs);

#endif // ColorCascade_H
