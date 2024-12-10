#ifndef TriangularCheckerboard_H
#define TriangularCheckerboard_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void TriangularCheckerboard_setup(SPIController& spiController);
void TriangularCheckerboard_loop(SPIController& spiController);

#endif // TriangularCheckerboard_H
