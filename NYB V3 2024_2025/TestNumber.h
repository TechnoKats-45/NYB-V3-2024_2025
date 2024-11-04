#ifndef TESTNUMBER_H
#define TESTNUMBER_H

#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

void TestNumber_setup(SPIController& spiController);  // Setup function for TestNumber
void TestNumber_loop(SPIController& spiController, int numLEDs);  // Loop function for TestNumber

#endif // TESTNUMBER_H
