#include "Confetti.h"
#include <Arduino.h>
#include "Globals.h"

#define confettiDelay 250  // Delay between confetti steps in milliseconds

// Confetti setup function
void Confetti_setup(SPIController& spiController) 
{
    spiController.begin();
    randomSeed(analogRead(A0));  // Seed random generator
}

// Confetti loop function
void Confetti_loop(SPIController& spiController, int numLEDs) 
{
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) 
    {
        if (random(0, 4) == 0) 
        {
            spiController.sendColor(global_brightness, 0, 0, 0);  // 25% chance of no color
        }
        else 
        {
            uint8_t red = random(0, 256);
            uint8_t green = random(0, 256);
            uint8_t blue = random(0, 256);
            spiController.sendColor(1, red, green, blue);  // 75% chance of random color
        }
    }

    spiController.sendEndFrame(numLEDs);
    delay(confettiDelay);  // Delay for stability
}
