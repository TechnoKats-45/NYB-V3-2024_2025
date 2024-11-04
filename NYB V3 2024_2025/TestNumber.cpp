#include "TestNumber.h"
#include "globals.h"  // Include globals file to access MAX_BRIGHTNESS
#include <Arduino.h>

// Setup function for TestNumber
void TestNumber_setup(SPIController& spiController)
{
    spiController.begin();
	for (int i = 0; i < NUM_LEDS; i++)
	{
        spiController.sendColor(0, 0, 0, 0);  // Set all LEDs to OFF
	}
}

// Loop function for TestNumber
void TestNumber_loop(SPIController& spiController, int numLEDs)
{
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++)
    {
        uint8_t red = 0;
        uint8_t green = 255;
        uint8_t blue = 0;
        spiController.sendColor(MAX_BRIGHTNESS, red, green, blue);  // Set LED color to green
    }

    spiController.sendEndFrame(numLEDs);

    delay(20);  // Control the speed of the loop if needed
}
