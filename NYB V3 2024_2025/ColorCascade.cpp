#include "ColorCascade.h"
#include "globals.h"  // Include globals file to access MAX_BRIGHTNESS
#include <Arduino.h>

#define ColorCascadeDelay 20  // Delay between color Cascade steps in milliseconds

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect

// Color Cascade setup function
void ColorCascade_setup(SPIController& spiController)
{
    spiController.begin();
}

// Color Cascade loop function
void ColorCascade_loop(SPIController& spiController, int numLEDs)
{
    static float hue = 0.0f;

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++)
    {
        uint8_t red, green, blue;
        spiController.hsvToRgb(hue + (float)i / numLEDs, 1.0, 1.0, red, green, blue);
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, red, green, blue);  // Use MAX_BRIGHTNESS for adjustable brightness
    }

    spiController.sendEndFrame(numLEDs);

    hue += 0.01f;  // Increment hue to create the Cascade effect
    if (hue >= 1.0f)
    {
        hue -= 1.0f;
    }

	delay(ColorCascadeDelay);  // Control the speed of the Cascade effect
}
