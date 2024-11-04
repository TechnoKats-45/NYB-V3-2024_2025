#ifndef ColorFade_H
#define ColorFade_H

#include "SPIController.h"

// Color fade setup function
void ColorFade_setup(SPIController& spiController);

// Color fade loop function to fade through colors on all LEDs
void ColorFade_loop(SPIController& spiController, int numLEDs);

#endif // ColorFade_H
