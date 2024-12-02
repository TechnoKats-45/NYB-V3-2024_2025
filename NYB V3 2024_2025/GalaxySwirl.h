#ifndef GALAXYSWIRL_H
#define GALAXYSWIRL_H

#include "SPIController.h"

// Setup function for Galaxy Swirl mode
void GalaxySwirl_setup(SPIController& spiController);

// Loop function for Galaxy Swirl mode
void GalaxySwirl_loop(SPIController& spiController);

#endif // GALAXYSWIRL_H
