#ifndef TriangularSpiral_H
#define TriangularSpiral_H

#include "SPIController.h"
#include "PanelMapping.h"
#include <Arduino.h>
#include <math.h>

// Function prototypes
void TriangularSpiral_setup(SPIController& spiController);
void TriangularSpiral_loop(SPIController& spiController);

#endif // TriangularSpiral_H
