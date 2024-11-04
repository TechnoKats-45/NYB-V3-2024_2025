#ifndef StartupSequence_H
#define StartupSequence_H

#include "SPIController.h"

// Startup sequence setup function
void StartupSequence_setup(SPIController& spiController);

// Startup sequence loop function to create a startup effect
void StartupSequence_loop(SPIController& spiController, int numLEDs);

#endif // StartupSequence_H
