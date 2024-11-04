#ifndef RowByRow_H
#define RowByRow_H

#include "SPIController.h"

// Row-by-row setup function
void RowByRow_setup(SPIController& spiController);

// Row-by-row loop function to light up rows one at a time
void RowByRow_loop(SPIController& spiController);

#endif // RowByRow_H
