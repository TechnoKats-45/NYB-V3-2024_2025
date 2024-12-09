#ifndef TURNOFFLEDS_H
#define TURNOFFLEDS_H

#include "SPIController.h"
#include "globals.h"

// Setup function for the TurnOffLEDs mode
void TurnOffLEDs_setup(SPIController& spiController);

// Loop function for the TurnOffLEDs mode
void TurnOffLEDs_loop(SPIController& spiController);

#endif // TURNOFFLEDS_H
