#include "Countdown.h"
#include "SPIController.h"
#include "PanelMapping.h"
#include "globals.h"
#include <Arduino.h>
#include <math.h>

// Duration of the countdown in seconds
const unsigned long CountdownDuration = 10; // Last 10 seconds

// Global variable to store the start time of the countdown
unsigned long countdownStartTime = 0;

// Setup function for Countdown mode
void CountdownEffect_setup(SPIController& spiController) {
    spiController.begin();
    countdownStartTime = millis(); // Initialize the countdown start time
}

// Loop function for Countdown mode
void CountdownEffect_loop(SPIController& spiController) {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - countdownStartTime;

    // Check if the countdown is over
    if (elapsedTime >= CountdownDuration * 1000UL) {
        // Countdown is over, turn off LEDs and exit the function
        uint8_t ledBuffer[NUM_LEDS][3] = { 0 }; // Turn off all LEDs

        // Send the frame to the LEDs
        spiController.sendStartFrame();
        for (int j = 0; j < NUM_LEDS; j++) {
            spiController.sendColor(0, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
        }
        spiController.sendEndFrame(NUM_LEDS);
        return;
    }

    // Calculate the time within the current second
    unsigned long timeInSecond = elapsedTime % 1000UL; // Milliseconds within the current second

    // Use sine wave function for smooth pulsing effect
    float angle = (2.0f * PI * timeInSecond) / 1000.0f; // Ranges from 0 to 2*PI over one second
    float brightness = (sin(angle - PI / 2.0f) + 1.0f) / 2.0f; // Normalize brightness between 0 and 1

    // Apply brightness to full white color
    uint8_t red = (uint8_t)(brightness * 255.0f);
    uint8_t green = (uint8_t)(brightness * 255.0f);
    uint8_t blue = (uint8_t)(brightness * 255.0f);

    // Prepare LED buffer with the calculated brightness
    uint8_t ledBuffer[NUM_LEDS][3];

    for (int i = 0; i < NUM_LEDS; i++) {
        ledBuffer[i][0] = red;
        ledBuffer[i][1] = green;
        ledBuffer[i][2] = blue;
    }

    // Send the frame to the LEDs
    spiController.sendStartFrame();
    for (int j = 0; j < NUM_LEDS; j++) {
        spiController.sendColor(MAX_BRIGHTNESS, ledBuffer[j][0], ledBuffer[j][1], ledBuffer[j][2]);
    }
    spiController.sendEndFrame(NUM_LEDS);

    // Small delay to control frame rate (~50 FPS)
    delay(20);
}
