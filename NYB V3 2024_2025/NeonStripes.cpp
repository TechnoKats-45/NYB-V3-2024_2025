#include "NeonStripes.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 50  // Delay between frame updates in milliseconds

// Number of stripes and their width
#define NumStripes 5
#define StripeWidth 3  // Number of rows per stripe

static float brightnessScaling = 0.25; // Brightness scaling factor for the wave effect

// Colors for the neon stripes (RGB values)
const uint8_t NeonColors[NumStripes][3] = {
    {255, 0, 255},   // Magenta
    {0, 255, 255},   // Cyan
    {255, 255, 0},   // Yellow
    {0, 255, 128},   // Neon Green
    {255, 128, 0}    // Neon Orange
};

// Position offset for the stripes
static int stripeOffset = 0;

void NeonStripes_setup(SPIController& spiController)
{
    spiController.begin();
}

void NeonStripes_loop(SPIController& spiController)
{
    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[NUM_LEDS][3];  // Declare without initializer

    // Explicitly set each LED color to off (0, 0, 0)
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0;  // Red
        ledBuffer[i][1] = 0;  // Green
        ledBuffer[i][2] = 0;  // Blue
    }

    // Set the first LED (status LED) to green
    int statusLED = panelMapping[0][0];
    ledBuffer[statusLED][0] = 0;   // Red
    ledBuffer[statusLED][1] = 255; // Green
    ledBuffer[statusLED][2] = 0;   // Blue

    // Calculate total number of rows excluding the status LED row
    int totalPatternRows = totalRows - 1;

    // Calculate the total height of the pattern (number of rows per cycle)
    int patternHeight = NumStripes * StripeWidth;

    // Loop through each row (excluding status LED row)
    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];

        // Determine which stripe this row belongs to
        int adjustedRow = (row - 1 + stripeOffset) % patternHeight;
        int stripeIndex = adjustedRow / StripeWidth;

        // Get the color for the current stripe
        uint8_t red = NeonColors[stripeIndex][0];
        uint8_t green = NeonColors[stripeIndex][1];
        uint8_t blue = NeonColors[stripeIndex][2];

        // Set the LEDs in the row to the stripe color
        for (int i = 0; i < numLEDsInRow; i++) {
            int ledNumber = panelMapping[row][i];
            ledBuffer[ledNumber][0] = red;   // Red
            ledBuffer[ledNumber][1] = green; // Green
            ledBuffer[ledNumber][2] = blue;  // Blue
        }
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(brightnessScaling*MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    // Update the stripe offset to create the sliding effect
    stripeOffset = (stripeOffset + 1) % patternHeight;

    delay(LEDDelay);  // Delay before the next frame update
}
