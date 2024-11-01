#include "TwinklingStars.h"
#include <Arduino.h>
#include "Globals.h"  // Include Globals to access MAX_BRIGHTNESS

void TwinklingStars_setup(SPIController& spiController) 
{
    spiController.begin();
}

void TwinklingStars_loop(SPIController& spiController, int numLEDs) 
{
    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) 
    {
        if (random(100) < 25) {  // Approximately 50% chance to set LED to blank
            spiController.sendColor(0, 0, 0, 0);  // Blank LED
        }
        else 
        {
            int brightness = random(0.5 * MAX_BRIGHTNESS, 1.0 * MAX_BRIGHTNESS);  // Scale brightness with MAX_BRIGHTNESS
            uint8_t red = random(10, 255);    // Maximize red to bring out purple
            uint8_t green = random(0, 10);    // Almost no green for a pure purple tone
            uint8_t blue = random(10, 255);   // High blue values for vibrant purple

            spiController.sendColor(brightness, red, green, blue);
        }
    }

    spiController.sendEndFrame(numLEDs);
    delay(200);  // Update twinkles every 200ms
}
