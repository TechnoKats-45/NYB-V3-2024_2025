#include "TriadVortex.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define VORTEX_DELAY 100 // Delay in milliseconds between updates

static float brightnessScaling = 1; // Adjust the overall brightness
static int rotationPhase = 0; // Current phase for vortex rotation
static int colorOffset = 0; // Offset to shift through the color palette

void TriadVortex_setup(SPIController& spiController) {
    spiController.begin();
}

void TriadVortex_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Determine rotation direction: alternate clockwise and counterclockwise per panel
        bool clockwise = (panel % 2 == 0);

        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Calculate the spiral position based on index and rotation phase
            int spiralIndex = clockwise ? (i + rotationPhase) % numLEDsInPanel
                : (numLEDsInPanel + i - rotationPhase) % numLEDsInPanel;

            // Calculate dynamic color based on the spiral index and color offset
            int hue = (spiralIndex * 10 + colorOffset) % 360; // Hue shifts per spiral step
            uint8_t red, green, blue;
            // Convert hue to RGB (simplified HSV to RGB)
            if (hue < 120) {
                red = (uint8_t)((120 - hue) * MAX_BRIGHTNESS / 120);
                green = (uint8_t)(hue * MAX_BRIGHTNESS / 120);
                blue = 0;
            }
            else if (hue < 240) {
                hue -= 120;
                red = 0;
                green = (uint8_t)((120 - hue) * MAX_BRIGHTNESS / 120);
                blue = (uint8_t)(hue * MAX_BRIGHTNESS / 120);
            }
            else {
                hue -= 240;
                red = (uint8_t)(hue * MAX_BRIGHTNESS / 120);
                green = 0;
                blue = (uint8_t)((120 - hue) * MAX_BRIGHTNESS / 120);
            }

            // Set the color for the current LED
            ledBuffer[ledNumber][0] = (uint8_t)(brightnessScaling * red);
            ledBuffer[ledNumber][1] = (uint8_t)(brightnessScaling * green);
            ledBuffer[ledNumber][2] = (uint8_t)(brightnessScaling * blue);
        }
    }

    // Send the LED data to the SPI controller
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(brightnessScaling * MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(VORTEX_DELAY); // Delay for smooth animation

    // Update the rotation phase and color offset
    rotationPhase = (rotationPhase + 1) % totalPanels; // Rotate through the vortex
    colorOffset = (colorOffset + 5) % 360; // Shift the color palette
}
