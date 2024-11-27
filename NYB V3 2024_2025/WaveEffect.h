#ifndef WAVEEFFECT_H
#define WAVEEFFECT_H

#include "SPIController.h"

// Setup function for WaveEffect mode
void WaveEffect_setup(SPIController& spiController);

// Loop function for WaveEffect mode
void WaveEffect_loop(SPIController& spiController);

#endif // WAVEEFFECT_H
