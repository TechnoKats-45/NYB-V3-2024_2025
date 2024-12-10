#include "DragonsBreath.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>

#define BREATH_DELAY 50 // Delay in milliseconds between updates
#define FLAME_SPEED 3 // Controls the speed of the flame movement
#define FLICKER_CHANCE 15 // Percentage chance of a flicker per LED
#define HUE_RANGE 40 // Wider hue range for more contrasting colors

static float brightnessScaling = .5; // Adjust the overall brightness
static int flamePhase = 0; // Tracks the current phase of the flame animation

// Converts a hue value to RGB
static void setRGBFromHue(uint8_t hue, uint8_t& red, uint8_t& green, uint8_t& blue) {
    if (hue < 85) {
        red = hue * 3;
        green = 255 - hue * 3;
        blue = 0;
    }
    else if (hue < 170) {
        hue -= 85;
        red = 255 - hue * 3;
        green = 0;
        blue = hue * 3;
    }
    else {
        hue -= 170;
        red = 0;
        green = hue * 3;
        blue = 255 - hue * 3;
    }
}

void DragonsBreath_setup(SPIController& spiController) {
    spiController.begin();
}

void DragonsBreath_loop(SPIController& spiController) {
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // LED buffer for RGB data

    // Loop through each panel
    for (int panel = 0; panel < totalPanels; panel++) {
        const int* panelLEDs = panelDefinitions[panel];
        int numLEDsInPanel = sizeof(panelDefinitions[panel]) / sizeof(panelDefinitions[panel][0]);

        // Generate fiery tendrils for the panel
        for (int i = 0; i < numLEDsInPanel; i++) {
            int ledNumber = panelLEDs[i];

            // Simulate tendrils using sine waves and random flickers
            float tendrilIntensity = 0.5 + 0.5 * sin((i + flamePhase) * 0.2);
            bool flicker = random(100) < FLICKER_CHANCE;

            // Calculate hue for fiery colors (expanded range for contrast)
            uint8_t hue = 10 + (uint8_t)(tendrilIntensity * HUE_RANGE); // Fire hues with higher contrast
            uint8_t red, green, blue;
            setRGBFromHue(hue, red, green, blue);

            // Apply flicker effect
            if (flicker) {
                red = min((int)red + 100, 255); // Brighten red
                green = min((int)green + 50, 255); // Slightly brighten green
                blue = max(blue - 50, 0); // Dim blue for more fiery contrast
            }

            // Scale brightness and set LED color
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

    delay(BREATH_DELAY); // Delay for smooth animation

    // Update the flame phase for dynamic motion
    flamePhase = (flamePhase + FLAME_SPEED) % 256;
}
