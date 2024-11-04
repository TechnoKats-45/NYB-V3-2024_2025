#include "AmericanFlag.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

#define LEDDelay 50  // Adjust delay as needed
#define MAX_BRIGHTNESS 31  // Maximum brightness level for APA102 LEDs

void AmericanFlag_setup(SPIController& spiController)
{
    spiController.begin();
    randomSeed(analogRead(0));  // Initialize random number generator
}

void AmericanFlag_loop(SPIController& spiController)
{
    const int totalLEDs = 546;  // Adjust to match your actual LED count

    // Static variables to maintain state across function calls
    static unsigned long lastUpdateTime = 0;
    const unsigned long updateInterval = 50;  // milliseconds

    static float wavePhase = 0.0;
    const float waveSpeed = 0.05;  // Slower wave effect

    unsigned long currentTime = millis();
    if (currentTime - lastUpdateTime >= updateInterval)
    {
        lastUpdateTime = currentTime;

        // Update wave phase
        wavePhase += waveSpeed;
        if (wavePhase > 2 * PI)
        {
            wavePhase -= 2 * PI;
        }

        // Send the start frame
        spiController.sendStartFrame();

        // Iterate over each row in the panel mapping
        for (int row = 0; row < totalRows; row++)
        {
            int rowLength = rowSizes[row];
            int stripeIndex = row * 13 / totalRows;  // Map rows to 13 stripes
            bool isRedStripe = (stripeIndex % 2 == 0);  // Even stripes are red

            // Determine if the row is in the blue area (top 6 rows)
            int blueAreaRows = 6;  // Top 6 rows
            bool isInBlueAreaRow = (row < blueAreaRows);

            for (int col = 0; col < rowLength; col++)
            {
                int ledNumber = panelMapping[row][col];

                // Ensure ledNumber is within valid range
                if (ledNumber < 0 || ledNumber >= totalLEDs)
                {
                    continue;  // Skip invalid LED numbers
                }

                uint8_t r = 0, g = 0, b = 0;

                if (isInBlueAreaRow)
                {
                    // Entire row is blue
                    r = 0;
                    g = 0;
                    b = 255;  // Blue

                    // Randomly flash white stars (slowed down significantly)
                    if (random(0, 1000) < 5)  // 0.5% chance to sparkle
                    {
                        r = 255;
                        g = 255;
                        b = 255;  // White star
                    }
                }
                else
                {
                    // Stripes with increased minimum brightness
                    float brightness = 0.85 + 0.15 * sin(wavePhase + row * 0.3);  // Varies between 0.7 and 1

                    if (isRedStripe)
                    {
                        r = 255 * brightness;
                        g = 0;
                        b = 0;
                    }
                    else
                    {
                        r = 255 * brightness;
                        g = 255 * brightness;
                        b = 255 * brightness;  // White stripe
                    }
                }

                // Send color data for the current LED
                spiController.sendColor(MAX_BRIGHTNESS, r, g, b);
            }
        }

        // Send the end frame
        spiController.sendEndFrame(totalLEDs);
    }
}
