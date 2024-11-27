#ifndef AURORA_EFFECT_H
#define AURORA_EFFECT_H

#include "SPIController.h"

// Aurora effect parameters
#define AuroraSpeed 0.05         // Speed of the aurora motion
#define AuroraIntensity 1.5      // Controls the brightness intensity
#define AuroraWaveFrequency 0.2  // Frequency of color waves
#define AuroraFadeSpeed 0.01     // Rate at which colors fade in and out

// Setup and loop functions for Aurora effect
void AuroraEffect_setup(SPIController& spiController);
void AuroraEffect_loop(SPIController& spiController);

#endif // AURORA_EFFECT_H
