#ifndef BOUNCING_WAVE_EFFECT_H
#define BOUNCING_WAVE_EFFECT_H

#include "SPIController.h"

// Delay between wave updates in milliseconds
#define BounceWaveDelay 50

// Amplitude of the wave (number of rows up/down)
#define BounceWaveAmplitude 3

// Frequency of the wave (controls how many LEDs fit in one cycle)
#define BounceWaveFrequency 0.5

// Speed of the wave (how fast it moves)
#define BounceWaveSpeed 0.2

// Number of simultaneous bouncing waves
#define NumBouncingWaves 2

// Struct to represent a bouncing wave
struct BouncingWave {
    float position;    // Current position in the wave
    uint8_t color[3];  // RGB color of the wave
    int direction;     // 1 for downward, -1 for upward
};

// Externally accessible setup and loop functions for the bouncing wave effect
void BouncingWaveEffect_setup(SPIController& spiController);
void BouncingWaveEffect_loop(SPIController& spiController);

#endif // BOUNCING_WAVE_EFFECT_H
