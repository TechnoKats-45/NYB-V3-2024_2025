#ifndef METEOR_H
#define METEOR_H

#include "SPIController.h"

// Meteor shower setup function
void Meteor_setup(SPIController& spiController);

// Meteor shower loop function to create a falling meteor effect
void Meteor_loop(SPIController& spiController, int numLEDs);

#endif // METEOR_H
