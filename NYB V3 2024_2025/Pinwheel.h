#ifndef Pinwheel_H
#define Pinwheel_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void Pinwheel_setup(SPIController& spiController);
void Pinwheel_loop(SPIController& spiController);

#endif // Pinwheel
#pragma once
