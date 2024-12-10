#include "Fire.h"
#include "globals.h"  // Include globals file to access MAX_BRIGHTNESS
#include <Arduino.h>

#define FireEffectDelay 20  // Delay between fire effect steps in milliseconds

static float brightnessScaling = 0.5; // Brightness scaling factor for the wave effect

// Fire effect setup function
void FireEffect_setup(SPIController& spiController)
{
    spiController.begin();
}

// Fire effect loop function
void FireEffect_loop(SPIController& spiController, int numLEDs)
{
    static float baseHue = 0.05f;  // Adjusted hue for warm colors
    static float hueOffset = 0.0f;
    static float brightnessFactor = 0.8f;  // Starting brightness factor for fire

    spiController.sendStartFrame();

    for (int i = 0; i < numLEDs; i++)
    {
        uint8_t red, green, blue;
        
        // Create a varying hue within the red-orange-yellow range for each LED
        float hue = baseHue + (float)i / (numLEDs * 10) + hueOffset;
        if (hue > 1.0f) hue -= 1.0f;

        // Vary the brightness slightly for each LED to create a flickering effect
        float brightness = brightnessScaling*MAX_BRIGHTNESS * (brightnessFactor + (float)(random(-10, 10)) / 100.0);

        // Convert HSV to RGB with a fixed saturation for warm colors
        spiController.hsvToRgb(hue, 1.0, brightness, red, green, blue);
        
        spiController.sendColor((uint8_t)brightness, red, green, blue);
    }

    spiController.sendEndFrame(numLEDs);

    // Increment the hue offset to create a dynamic flow of colors
    hueOffset += 0.02f;
    if (hueOffset >= 1.0f)
    {
        hueOffset -= 1.0f;
    }

    // Flicker by randomly varying brightness
    brightnessFactor = 0.7f + (float)(random(-10, 10)) / 100.0;

    delay(FireEffectDelay);  // Control the speed of the Fire effect
}
