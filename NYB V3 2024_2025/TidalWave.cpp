#include "TidalWave.h"
#include "globals.h"
#include <Arduino.h>

// Static variables for non-blocking behavior
static uint8_t ledBuffer[475][3];  // Statically allocated LED buffer
static int currentStep = 0;           // Current step in the wave animation
static unsigned long lastUpdateTime = 0; // Last time the wave was updated

static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Setup function for Tidal Wave
void TidalWave_setup(SPIController& spiController)
{
    spiController.begin();

    // Initialize all LEDs to off (no need for dynamic allocation)
    for (int i = 0; i < NUM_LEDS; i++)
    {
        ledBuffer[i][0] = 0;  // Red
        ledBuffer[i][1] = 0;  // Green
        ledBuffer[i][2] = 0;  // Blue
    }

    currentStep = 0;
    lastUpdateTime = millis();
}


// Loop function for Tidal Wave (non-blocking)
void TidalWave_loop(SPIController& spiController)
{
    unsigned long currentTime = millis();

    // Update only if WaveSpeed time has passed
    if (currentTime - lastUpdateTime >= WaveSpeed)
    {
        lastUpdateTime = currentTime;

        // Reset LED buffer to off
        for (int i = 0; i < NUM_LEDS; i++)
        {
            ledBuffer[i][0] = 0;  // Red
            ledBuffer[i][1] = 0;  // Green
            ledBuffer[i][2] = 0;  // Blue
        }

        // Create the wave effect for the current step
        for (int i = 0; i < WaveWidth; i++)
        {
            int ledIndex = currentStep - i;

            // Ensure index is within bounds
            if (ledIndex >= 0 && ledIndex < NUM_LEDS)
            {
                // Gradual brightness scaling based on distance from the wave front
                uint8_t intensity = (MAX_BRIGHTNESS * (WaveWidth - i)) / WaveWidth;
                ledBuffer[ledIndex][0] = 0;          // Red
                ledBuffer[ledIndex][1] = intensity; // Green
                ledBuffer[ledIndex][2] = MAX_BRIGHTNESS; // Blue
            }
        }

        // Send the current frame
        spiController.sendStartFrame();
        for (int j = 0; j < NUM_LEDS; j++)
        {
            spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
        }
        spiController.sendEndFrame(NUM_LEDS);

        // Increment the step and wrap around if necessary
        currentStep++;
        if (currentStep > NUM_LEDS + WaveWidth)
        {
            currentStep = 0;
        }
    }
}
