#ifndef KALEIDO_PULSE_H
#define KALEIDO_PULSE_H

#include "SPIController.h"

// Function to initialize the KaleidoPulse effect
void KaleidoPulse_setup(SPIController& spiController);

// Function to run the KaleidoPulse effect
void KaleidoPulse_loop(SPIController& spiController);

#endif // KALEIDO_PULSE_H
