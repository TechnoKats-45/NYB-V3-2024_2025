#ifndef TIDALWAVE_H
#define TIDALWAVE_H

#include "SPIController.h"

// Configuration for Tidal Wave effect
#define WaveSpeed 50  // Delay between wave steps in milliseconds
#define WaveWidth 20  // Width of the wave in LEDs

// Function prototypes for Tidal Wave effect
void TidalWave_setup(SPIController& spiController);
void TidalWave_loop(SPIController& spiController);

#endif // TIDALWAVE_H
