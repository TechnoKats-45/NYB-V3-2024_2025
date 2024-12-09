#include "TurnOffLEDs.h"
#include <Arduino.h>

// Setup function for TurnOffLEDs mode
void TurnOffLEDs_setup(SPIController& spiController) {
    spiController.begin();

    // Ensure all LEDs are turned off during setup
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 };
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    Serial.println("TurnOffLEDs mode setup complete. All LEDs are off.");
}

// Loop function for TurnOffLEDs mode
void TurnOffLEDs_loop(SPIController& spiController) 
{
    // Continuously refresh the "off" state to ensure no external factors turn LEDs on
    uint8_t ledBuffer[NUM_LEDS][3] = { 0 };
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    delay(1000); // Add a delay to prevent excessive refreshes
}
