#include "RowByRow.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define RowDelay 100  // Delay between lighting each row in milliseconds

// Row-by-row setup function
void RowByRow_setup(SPIController& spiController)
{
    spiController.begin();
}

// Row-by-row loop function
void RowByRow_loop(SPIController& spiController)
{
    for (int row = 0; row < totalRows; row++)
    {
        spiController.sendStartFrame();

        // Light up each LED in the current row
        for (int i = 0; i < rowSizes[row]; i++)
        {
            int ledNumber = panelMapping[row][i];
            uint8_t red = 255, green = 255, blue = 255;  // Set color to white for each LED
            spiController.sendColor(MAX_BRIGHTNESS, red, green, blue);
        }

        spiController.sendEndFrame(rowSizes[row]);

        delay(RowDelay);  // Wait before moving to the next row
    }
}
