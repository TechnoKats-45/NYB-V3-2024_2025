#include "AmericanFlag.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 100         // Delay between lighting each LED in milliseconds
#define StarFlashDuration 200 // Duration in milliseconds for each star flash
#define NumStars 2           // Number of stars flashing at a time

static float brightnessScaling = 0.25; // Brightness scaling factor for the American Flag effect

// Row-by-row setup function
void AmericanFlag_setup(SPIController& spiController)
{
    spiController.begin();
}

// Helper function to flash random white stars in the blue areas
void flashStars(uint8_t ledBuffer[][3], SPIController& spiController)
{
    // Define blue LED ranges
    int blueRanges[][2] = { {1, 26}, {175, 200}, {321, 346} };
    int numBlueRanges = sizeof(blueRanges) / sizeof(blueRanges[0]);

    // Collect all blue area LEDs
    int blueLEDs[NUM_LEDS];  // Assuming NUM_LEDS is large enough
    int numBlueLEDs = 0;

    for (int i = 0; i < numBlueRanges; i++)
    {
        int rangeStart = blueRanges[i][0];
        int rangeEnd = blueRanges[i][1];

        for (int ledNumber = rangeStart; ledNumber <= rangeEnd; ledNumber++)
        {
            blueLEDs[numBlueLEDs++] = ledNumber;
        }
    }

    // Flash random white stars in the blue area
    for (int star = 0; star < NumStars; star++)
    {
        // Choose a random LED from blueLEDs
        int ledIndex = random(0, numBlueLEDs);
        int ledNumber = blueLEDs[ledIndex];

        // Temporarily set the LED to white (flash effect)
        ledBuffer[ledNumber][0] = 255;  // Red
        ledBuffer[ledNumber][1] = 255;  // Green
        ledBuffer[ledNumber][2] = 255;  // Blue
    }

    // Send the frame with flashing stars
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(StarFlashDuration);  // Hold the star flash longer

    // Reset the blue areas after flash duration
    for (int i = 0; i < numBlueLEDs; i++)
    {
        int ledNumber = blueLEDs[i];
        ledBuffer[ledNumber][0] = 0;   // Red
        ledBuffer[ledNumber][1] = 0;   // Green
        ledBuffer[ledNumber][2] = 255; // Blue
    }
}

// Row-by-row loop function
void AmericanFlag_loop(SPIController& spiController)
{
    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[NUM_LEDS][3];  // Declare without initializer

    // Explicitly set each LED color to off (0, 0, 0)
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0;  // Red
        ledBuffer[i][1] = 0;  // Green
        ledBuffer[i][2] = 0;  // Blue
    }

    // Set the first LED (row 0 status LED) to green
    int statusLED = panelMapping[0][0];
    ledBuffer[statusLED][0] = 0;   // Red
    ledBuffer[statusLED][1] = 255; // Green
    ledBuffer[statusLED][2] = 0;   // Blue

    // Define blue LED ranges
    int blueRanges[][2] = { {1, 26}, {175, 200}, {321, 346} };
    int numBlueRanges = sizeof(blueRanges) / sizeof(blueRanges[0]);

    // Set blue LEDs
    for (int i = 0; i < numBlueRanges; i++)
    {
        int rangeStart = blueRanges[i][0];
        int rangeEnd = blueRanges[i][1];

        for (int ledNumber = rangeStart; ledNumber <= rangeEnd; ledNumber++)
        {
            ledBuffer[ledNumber][0] = 0;    // Red
            ledBuffer[ledNumber][1] = 0;    // Green
            ledBuffer[ledNumber][2] = 255;  // Blue
        }
    }

    // Set the remaining LEDs with alternating red and white stripes
    // For each row
    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];
        // Determine stripe color based on row number
        int stripeNumber = (row - 1) / 3;
        bool isRedStripe = (stripeNumber % 2 == 0);

        for (int i = 0; i < numLEDsInRow; i++) {
            int ledNumber = panelMapping[row][i];

            // Skip LEDs in blue ranges
            bool isBlue = false;
            for (int j = 0; j < numBlueRanges; j++)
            {
                if (ledNumber >= blueRanges[j][0] && ledNumber <= blueRanges[j][1])
                {
                    isBlue = true;
                    break;
                }
            }
            if (isBlue)
                continue;

            // Set stripe color
            if (isRedStripe) {
                ledBuffer[ledNumber][0] = 255;  // Red
                ledBuffer[ledNumber][1] = 0;    // Green
                ledBuffer[ledNumber][2] = 0;    // Blue
            }
            else {
                ledBuffer[ledNumber][0] = 255;  // Red
                ledBuffer[ledNumber][1] = 255;  // Green
                ledBuffer[ledNumber][2] = 255;  // Blue
            }
        }
    }

    // Send the frame without stars
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);  // Delay before the next frame update

    // Flash stars in the blue area
    flashStars(ledBuffer, spiController);
}
