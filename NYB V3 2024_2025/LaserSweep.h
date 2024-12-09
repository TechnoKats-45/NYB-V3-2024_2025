#ifndef LaserSweep_H
#define LaserSweep_H

#include "SPIController.h"

void LaserSweep_setup(SPIController& spiController);
void LaserSweep_loop(SPIController& spiController);

#endif // LaserSweep_H
