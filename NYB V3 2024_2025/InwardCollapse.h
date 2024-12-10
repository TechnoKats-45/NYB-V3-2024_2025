#ifndef InwardCollapse_H
#define InwardCollapse_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void InwardCollapse_setup(SPIController& spiController);
void InwardCollapse_loop(SPIController& spiController);

#endif // InwardCollapse_H
