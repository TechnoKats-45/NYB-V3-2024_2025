#ifndef InversePinwheel_H
#define InversePinwheel_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void InversePinwheel_setup(SPIController& spiController);
void InversePinwheel_loop(SPIController& spiController);

#endif
