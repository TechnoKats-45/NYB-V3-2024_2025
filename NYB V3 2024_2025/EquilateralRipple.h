#ifndef EquilateralRipple_H
#define EquilateralRipple_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void EquilateralRipple_setup(SPIController& spiController);
void EquilateralRipple_loop(SPIController& spiController);

#endif // EquilateralRipple_H
