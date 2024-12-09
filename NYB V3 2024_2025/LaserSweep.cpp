#include "LaserSweep.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <EEPROM.h>
#include <Arduino.h>

#define LEDDelay 50 // Delay between frames in milliseconds
#define COLOR_INDEX_ADDR 0 // EEPROM address to store the current color index

// Static variables to keep track of the beam position and direction
static int beamPosition = 0;
static int beamDirection = 1; // 1 for moving right, -1 for moving left
static float brightnessScaling = 1; // Brightness scaling factor for the wave effect

// Beam color components (initialized to 0, set at startup)
static uint8_t BeamColorR = 0;
static uint8_t BeamColorG = 0;
static uint8_t BeamColorB = 0;

// Function to set the beam color based on the color index
void setBeamColor(int colorIndex)
{
    switch (colorIndex)
    {
    case 0: // Red
        BeamColorR = 255;
        BeamColorG = 0;
        BeamColorB = 0;
        break;
    case 1: // Orange
        BeamColorR = 255;
        BeamColorG = 127;
        BeamColorB = 0;
        break;
    case 2: // Yellow
        BeamColorR = 255;
        BeamColorG = 255;
        BeamColorB = 0;
        break;
    case 3: // Green
        BeamColorR = 0;
        BeamColorG = 255;
        BeamColorB = 0;
        break;
    case 4: // Blue
        BeamColorR = 0;
        BeamColorG = 0;
        BeamColorB = 255;
        break;
    case 5: // Indigo
        BeamColorR = 75;
        BeamColorG = 0;
        BeamColorB = 130;
        break;
    case 6: // Violet
        BeamColorR = 148;
        BeamColorG = 0;
        BeamColorB = 211;
        break;
    }
}

void LaserSweep_setup(SPIController& spiController)
{
    spiController.begin();

    // Read the last color index from EEPROM
    int colorIndex = EEPROM.read(COLOR_INDEX_ADDR);
    if (colorIndex < 0 || colorIndex > 6) {
        colorIndex = 0; // Default to red if value is invalid
    }

    // Increment the color index and wrap around after violet
    colorIndex = (colorIndex + 1) % 7;

    // Save the new color index to EEPROM
    EEPROM.write(COLOR_INDEX_ADDR, colorIndex);

    // Set the beam color based on the updated index
    setBeamColor(colorIndex);

    // Print selected beam color for debugging
    Serial.print("Selected Beam Color Index: ");
    Serial.println(colorIndex);
    Serial.print("Selected Beam Color: R=");
    Serial.print(BeamColorR);
    Serial.print(", G=");
    Serial.print(BeamColorG);
    Serial.print(", B=");
    Serial.println(BeamColorB);
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
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
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
