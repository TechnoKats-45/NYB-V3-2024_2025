#include "SpectrumBars.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 100   // Delay between frame updates in milliseconds
#define NumColumns 13  // Number of spectrum bars (columns)
#define MaxBarHeight (totalRows - 1) // Maximum height of the bars

// Array to hold the current height of each bar
static int barHeights[NumColumns];

// Array to hold the target height for smooth animation
static int targetBarHeights[NumColumns];

// Colors for the bars (you can customize these)
const uint8_t BarColors[3] = { 0, 255, 0 }; // Green bars

void SpectrumBars_setup(SPIController& spiController)
{
    spiController.begin();

    // Initialize bar heights
    for (int i = 0; i < NumColumns; i++) {
        barHeights[i] = 0;
        targetBarHeights[i] = random(1, MaxBarHeight);
    }
}

void SpectrumBars_loop(SPIController& spiController)
{
    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[NUM_LEDS][3];

    // Turn off all LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = 0;  // Red
        ledBuffer[i][1] = 0;  // Green
        ledBuffer[i][2] = 0;  // Blue
    }

    // Set the status LED (assuming the first LED) to green
    int statusLED = panelMapping[0][0];
    ledBuffer[statusLED][0] = 0;   // Red
    ledBuffer[statusLED][1] = 255; // Green
    ledBuffer[statusLED][2] = 0;   // Blue

    // Update bar heights towards their target heights
    for (int i = 0; i < NumColumns; i++) {
        if (barHeights[i] < targetBarHeights[i]) {
            barHeights[i]++;
        }
        else if (barHeights[i] > targetBarHeights[i]) {
            barHeights[i]--;
        }
        else {
            // Once the target is reached, set a new target
            targetBarHeights[i] = random(1, MaxBarHeight);
        }
    }

    // Map columns across the triangle
    // Since the triangle widens towards the bottom, columns are not uniform in width
    // We'll define the starting positions for each column based on the maximum row size
    int maxRowSize = 0;
    for (int row = 1; row < totalRows; row++) {
        if (rowSizes[row] > maxRowSize)
            maxRowSize = rowSizes[row];
    }

    // Calculate the horizontal position for each column
    for (int col = 0; col < NumColumns; col++) {
        // Calculate the horizontal position in the triangle
        float colPosition = (float)col / (NumColumns - 1) * (maxRowSize - 1);
        int colPositionInt = round(colPosition);

        // For each bar, light up LEDs up to the current height
        for (int row = 1; row <= barHeights[col]; row++) {
            int numLEDsInRow = rowSizes[row];
            int rowOffset = (maxRowSize - numLEDsInRow) / 2;

            int ledIndexInRow = colPositionInt - rowOffset;

            if (ledIndexInRow >= 0 && ledIndexInRow < numLEDsInRow) {
                int ledNumber = panelMapping[row][ledIndexInRow];

                ledBuffer[ledNumber][0] = BarColors[0];  // Red
                ledBuffer[ledNumber][1] = BarColors[1];  // Green
                ledBuffer[ledNumber][2] = BarColors[2];  // Blue
            }
        }
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);
}
