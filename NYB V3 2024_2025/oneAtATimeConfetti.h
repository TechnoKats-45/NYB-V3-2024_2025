#ifndef ONEATATIMECONFETTI_H
#define ONEATATIMECONFETTI_H

#include "SPIController.h"

// One-at-a-time confetti effect setup function
void OneAtATimeConfetti_setup(SPIController& spiController, int NUM_LEDS);

// One-at-a-time confetti effect loop function to update a single random LED
void OneAtATimeConfetti_loop(SPIController& spiController, int numLEDs);

#endif // ONEATATIMECONFETTI_H
