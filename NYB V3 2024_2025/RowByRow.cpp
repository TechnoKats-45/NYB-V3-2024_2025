#include "RowByRow.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 100  // Delay between lighting each LED in milliseconds

// Row-by-row setup function
void RowByRow_setup(SPIController& spiController)
{
    spiController.begin();
}

// Row-by-row loop function
void RowByRow_loop(SPIController& spiController)
{
    // Define the total number of LEDs based on the mapping
    const int totalLEDs = 546;  // Total number of LEDs (adjust to match your actual LED count)

    // Define and initialize the buffer for RGB data of each LED
    uint8_t ledBuffer[totalLEDs][3] = { {0, 0, 0} };  // Each entry has [R, G, B], initialized to off

    // Iterate over each row in the panel mapping
    for (int row = 0; row < totalRows; row++)
    {
        // Iterate over each LED in the current row
        for (int i = 0; i < rowSizes[row]; i++)
        {
            // Get the LED number from panelMapping for the current LED
            int ledNumber = panelMapping[row][i];

            // Clear buffer (turn off all LEDs)
            for (int j = 0; j < totalLEDs; j++)
            {
                ledBuffer[j][0] = 0;  // Red
                ledBuffer[j][1] = 0;  // Green
                ledBuffer[j][2] = 0;  // Blue
            }

            // Set the target LED to white
            ledBuffer[ledNumber][0] = 255;  // Red
            ledBuffer[ledNumber][1] = 255;  // Green
            ledBuffer[ledNumber][2] = 255;  // Blue

            // Send the entire frame based on the buffer
            spiController.sendStartFrame();
            for (int j = 0; j < totalLEDs; j++)
            {
                spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
            }
            spiController.sendEndFrame(totalLEDs);  // End frame for the entire strip

            delay(LEDDelay);  // Delay before lighting the next LED
        }
    }
}
