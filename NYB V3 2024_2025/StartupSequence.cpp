#include "StartupSequence.h"
#include "globals.h"  // Include globals file to access MAX_BRIGHTNESS
#include <Arduino.h>

#define StartupDelay 50  // Delay between startup sequence steps in milliseconds

// Startup sequence setup function
void StartupSequence_setup(SPIController& spiController)
{
    spiController.begin();
}

// Startup sequence loop function
void StartupSequence_loop(SPIController& spiController, int numLEDs)
{
    // Phase 1: LEDs light up one by one in a "powering on" sequence
    for (int i = 0; i < numLEDs; i++)
    {
        spiController.sendStartFrame();

        for (int j = 0; j < numLEDs; j++)
        {
            uint8_t brightness = (j <= i) ? MAX_BRIGHTNESS : 0;  // Turn on LEDs one by one
            uint8_t red = 255, green = 255, blue = 255;          // Set color to white for startup

            spiController.sendColor(brightness, red, green, blue);
        }

        spiController.sendEndFrame(numLEDs);
        delay(StartupDelay);
    }

    // Phase 2: Pulse effect to indicate the system is ready
    for (int pulse = 0; pulse < 3; pulse++)  // Number of pulses
    {
        for (int brightness = MAX_BRIGHTNESS; brightness >= 10; brightness -= 5)
        {
            spiController.sendStartFrame();

            for (int i = 0; i < numLEDs; i++)
            {
                uint8_t red = 255, green = 255, blue = 255;
                spiController.sendColor(brightness, red, green, blue);  // Gradually decrease brightness
            }

            spiController.sendEndFrame(numLEDs);
            delay(StartupDelay);
        }

        for (int brightness = 10; brightness <= MAX_BRIGHTNESS; brightness += 5)
        {
            spiController.sendStartFrame();

            for (int i = 0; i < numLEDs; i++)
            {
                uint8_t red = 255, green = 255, blue = 255;
                spiController.sendColor(brightness, red, green, blue);  // Gradually increase brightness
            }

            spiController.sendEndFrame(numLEDs);
            delay(StartupDelay);
        }
    }
}
