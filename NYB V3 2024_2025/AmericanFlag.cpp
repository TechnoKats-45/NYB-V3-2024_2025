#include "AmericanFlag.h"
#include "PanelMapping.h"
#include "SPIController.h"
#include "globals.h"
#include <Arduino.h>

#define LEDDelay 100         // Delay between lighting each LED in milliseconds
#define StarFlashDuration 200 // Duration in milliseconds for each star flash (increased duration)
#define NumStars 2           // Reduced number of stars flashing at a time

// Row-by-row setup function
void AmericanFlag_setup(SPIController& spiController)
{
    spiController.begin();
}

// Helper function to flash random white stars in the blue area
void flashStars(uint8_t ledBuffer[][3], SPIController& spiController)
{
    // Select random LEDs in the first 6 rows (blue area) to flash as stars
    for (int star = 0; star < NumStars; star++)
    {
        int randomRow = random(0, 6);  // Choose a random row within the blue area
        int randomLEDIndex = random(0, rowSizes[randomRow]);  // Choose a random LED in that row
        int ledNumber = panelMapping[randomRow][randomLEDIndex];

        // Temporarily set the LED to white (flash effect)
        ledBuffer[ledNumber][0] = 255;  // Red
        ledBuffer[ledNumber][1] = 255;  // Green
        ledBuffer[ledNumber][2] = 255;  // Blue
    }

    // Send the frame with flashing stars
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(StarFlashDuration);  // Hold the star flash longer

    // Reset stars to blue after flash duration
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

    // Set the first 6 rows to blue
    for (int row = 1; row < 7; row++)
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

    // Send the frame without stars
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++)
    {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(LEDDelay);  // Delay before the next frame update

    // Flash stars in the blue area with fewer stars and a longer flash duration
    flashStars(ledBuffer, spiController);
}
