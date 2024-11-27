#ifndef COUNTDOWN_H
#define COUNTDOWN_H

#include "SPIController.h"

// Duration of the countdown in seconds (optional to expose)
extern const unsigned long CountdownDuration;

// Function prototypes for the Countdown effect
void CountdownEffect_setup(SPIController& spiController);
void CountdownEffect_loop(SPIController& spiController);

#endif // COUNTDOWN_H
