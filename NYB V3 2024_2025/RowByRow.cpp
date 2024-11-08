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

// Row-by-row loop function (non-blocking)
void RowByRow_loop(SPIController& spiController, uint8_t currentMode)
{
    // Static variables to track the current position in the sequence
    static int currentRow = 0;
    static int currentLED = 0;
    static unsigned long lastUpdate = 0;

    // Define the buffer for RGB data of each LED without initializer
    uint8_t ledBuffer[NUM_LEDS][3];

    // Explicitly initialize the buffer to zero (turn off all LEDs)
    for (int j = 0; j < NUM_LEDS; j++)
    {
        ledBuffer[j][0] = 0;  // Red
        ledBuffer[j][1] = 0;  // Green
        ledBuffer[j][2] = 0;  // Blue
    }

    // Check for mode switch
    if (currentMode != 11)  // Exit if mode has changed
    {
        currentRow = 0;
        currentLED = 0;
        return;
    }

    // Only proceed if enough time has passed
    if (millis() - lastUpdate < LEDDelay)
    {
        return;  // Exit if delay not yet completed
    }
    lastUpdate = millis();  // Update the last update time

    // Clear previous LED and set the current LED to white
    for (int j = 0; j < NUM_LEDS; j++)
    {
        ledBuffer[j][0] = 0;  // Red
        ledBuffer[j][1] = 0;  // Green
        ledBuffer[j][2] = 0;  // Blue
    }

    int ledNumber = panelMapping[currentRow][currentLED];
    ledBuffer[ledNumber][0] = 255;  // Red
    ledBuffer[ledNumber][1] = 255;  // Green
    ledBuffer[ledNumber][2] = 255;  // Blue

    // Send the entire frame based on the buffer
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);  // End frame for the entire strip

    // Move to the next LED
    currentLED++;
    if (currentLED >= rowSizes[currentRow])
    {
        currentLED = 0;
        currentRow++;
    }

    // Reset when the last row is reached
    if (currentRow >= totalRows)
    {
        currentRow = 0;
        currentLED = 0;
    }
}
