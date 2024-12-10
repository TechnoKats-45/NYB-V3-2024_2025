#ifndef FRACTAL_BLOOM_H
#define FRACTAL_BLOOM_H

#include "SPIController.h"

// Function to initialize the Fractal Bloom effect
void FractalBloom_setup(SPIController& spiController);

// Function to run the Fractal Bloom effect
void FractalBloom_loop(SPIController& spiController);

#endif // FRACTAL_BLOOM_H
