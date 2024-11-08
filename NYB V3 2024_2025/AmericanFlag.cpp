#include "AmericanFlag.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 100  // Delay between lighting each LED in milliseconds

// Row-by-row setup function
void AmericanFlag_setup(SPIController& spiController)
{
    spiController.begin();
}

// Row-by-row loop function
void AmericanFlag_loop(SPIController& spiController)
{
    // Define the total number of LEDs based on the mapping
    const int totalLEDs = 546;  // Total number of LEDs (adjust to match your actual LED count)

    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[totalLEDs][3] = { {0, 0, 0} };  // Each entry has [R, G, B], initialized to off

    // Set the first LED (row 0 status LED) to green
    int statusLED = panelMapping[0][0];
    ledBuffer[statusLED][0] = 0;   // Red
    ledBuffer[statusLED][1] = 255; // Green
    ledBuffer[statusLED][2] = 0;   // Blue

    // Set the first 6 rows to blue
    for (int row = 0; row < 6; row++)
    {
        for (int i = 0; i < rowSizes[row]; i++)
        {
            int ledNumber = panelMapping[row][i];
            ledBuffer[ledNumber][0] = 0;   // Red
            ledBuffer[ledNumber][1] = 0;   // Green
            ledBuffer[ledNumber][2] = 255; // Blue
        }
    }

    // Set the remaining rows with alternating red and white colors in groups of 3 rows, starting from row 7
    bool isRed = true;  // Start with red
    for (int row = 7; row < totalRows; row++)
    {
        uint8_t red = isRed ? 255 : 255;     // Red component is 255 for both red and white
        uint8_t green = isRed ? 0 : 255;     // Green component is 0 for red, 255 for white
        uint8_t blue = isRed ? 0 : 255;      // Blue component is 0 for red, 255 for white

        for (int i = 0; i < rowSizes[row]; i++)
        {
            int ledNumber = panelMapping[row][i];
            ledBuffer[ledNumber][0] = red;   // Red
            ledBuffer[ledNumber][1] = green; // Green
            ledBuffer[ledNumber][2] = blue;  // Blue
        }

        // Toggle color every 3 rows
        if ((row - 7 + 1) % 3 == 0)
        {
            isRed = !isRed;
        }
    }

    // Send the entire frame based on the buffer
    spiController.sendStartFrame();
    for (int j = 0; j < totalLEDs; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(totalLEDs);  // End frame for the entire strip

    delay(LEDDelay);  // Delay before the next frame update (if any)
}
