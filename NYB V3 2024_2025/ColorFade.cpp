#include "ColorFade.h"
#include "globals.h"  // Include globals file to access MAX_BRIGHTNESS
#include <Arduino.h>

#define ColorFadeDelay 20  // Delay between color fade steps in milliseconds

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

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

    // Set all LEDs to the same color
    uint8_t red, green, blue;
    spiController.hsvToRgb(hue, 1.0, 1.0, red, green, blue);

    for (int i = 0; i < numLEDs; i++)
    {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, red, green, blue);  // Use MAX_BRIGHTNESS for adjustable brightness
    }

    spiController.sendEndFrame(numLEDs);

    // Increment hue to change the color gradually
    hue += 0.01f;  // Adjust this value to control the speed of the color fade
    if (hue >= 1.0f)
    {
        hue -= 1.0f;
    }

    delay(ColorFadeDelay);  // Control the speed of the color fade effect
}
