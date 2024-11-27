#ifndef COUNTDOWNCIRCLE_H
#define COUNTDOWNCIRCLE_H

#include "SPIController.h"

// Setup function for CountdownCircle mode
void CountdownCircle_setup(SPIController& spiController);

// Loop function for CountdownCircle mode
void CountdownCircle_loop(SPIController& spiController);

#endif // COUNTDOWNCIRCLE_H
