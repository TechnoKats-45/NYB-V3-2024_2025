#include "ColorFade.h"
#include <Arduino.h>

#define colorFadeDelay 20  // Delay between color fade steps in milliseconds

// Color fade setup function
void ColorFade_setup(SPIController& spiController) 
{
    spiController.begin();
}

// Color fade loop function
void ColorFade_loop(SPIController& spiController, int numLEDs) 
{
    static float hue = 0.0f;

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++) 
    {
        uint8_t red, green, blue;
        spiController.hsvToRgb(hue + (float)i / numLEDs, 1.0, 1.0, red, green, blue);
        spiController.sendColor(31, red, green, blue);
    }

    spiController.sendEndFrame(numLEDs);

    hue += 0.01f;  // Increment hue to create the fade effect
    if (hue >= 1.0f) 
    {
        hue -= 1.0f;
    }

    delay(colorFadeDelay);  // Control the speed of the fade
}
