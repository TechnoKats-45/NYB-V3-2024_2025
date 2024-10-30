#include "Meteor.h"
#include <Arduino.h>

void Meteor_setup(SPIController& spiController) 
{
    spiController.begin();
}

void Meteor_loop(SPIController& spiController, int numLEDs) 
{
    static int meteorPosition = numLEDs - 1;

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) 
    {
        if (i == meteorPosition) 
        {
            spiController.sendColor(31, 255, 255, 255);  // Bright white meteor
        }
        else if (i > meteorPosition) {
            spiController.sendColor(15, 255, 255, 255);  // Fading tail
        }
        else {
            spiController.sendColor(1, 0, 0, 0);  // Off
        }
    }

    spiController.sendEndFrame(numLEDs);

    // Move meteor downwards
    meteorPosition--;
    if (meteorPosition < 0) {
        meteorPosition = numLEDs - 1;  // Restart meteor from the top
    }

    delay(100);
}
