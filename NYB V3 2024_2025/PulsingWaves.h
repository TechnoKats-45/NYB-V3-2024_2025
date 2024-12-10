#ifndef PulsingWaves_H
#define PulsingWaves_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void PulsingWaves_setup(SPIController& spiController);
void PulsingWaves_loop(SPIController& spiController);

#endif // PulsingWaves_H
