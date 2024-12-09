#include "LaserSweep.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 50      // Delay between frames in milliseconds
#define BeamColorR 255   // Red component of the beam color
#define BeamColorG 0     // Green component of the beam color
#define BeamColorB 0     // Blue component of the beam color

// Static variables to keep track of the beam position and direction
static int beamPosition = 0;
static int beamDirection = 1; // 1 for moving right, -1 for moving left

void LaserSweep_setup(SPIController& spiController)
{
    spiController.begin();
}

void LaserSweep_loop(SPIController& spiController)
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

    // Determine the maximum row size to calculate columns
    int maxRowSize = 0;
    for (int row = 1; row < totalRows; row++) {
        if (rowSizes[row] > maxRowSize)
            maxRowSize = rowSizes[row];
    }

    // For each row, set the LED corresponding to the current beam position
    for (int row = 1; row < totalRows; row++) {
        int numLEDsInRow = rowSizes[row];
        int columnOffset = (maxRowSize - numLEDsInRow) / 2;

        int ledIndexInRow = beamPosition - columnOffset;

        if (ledIndexInRow >= 0 && ledIndexInRow < numLEDsInRow) {
            int ledNumber = panelMapping[row][ledIndexInRow];

            ledBuffer[ledNumber][0] = BeamColorR;  // Red
            ledBuffer[ledNumber][1] = BeamColorG;  // Green
            ledBuffer[ledNumber][2] = BeamColorB;  // Blue
        }
    }

    // Send the frame
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    // Update beam position
    beamPosition += beamDirection;

    // Reverse direction if beam reaches the edge
    if (beamPosition >= maxRowSize) {
        beamPosition = maxRowSize - 1;
        beamDirection = -1;
    }
    else if (beamPosition < 0) {
        beamPosition = 0;
        beamDirection = 1;
    }

    delay(LEDDelay);
}
