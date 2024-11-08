#include "SPIController.h"
#include "globals.h"
#include "ColorCascade.h"
#include "Confetti.h"
#include "TwinklingStars.h"
#include "Ripple.h"
#include "Sparkle.h"
#include "Breathing.h"
#include "Meteor.h"
#include "OneAtATimeConfetti.h"
#include "Fire.h"
#include "ColorFade.h"
#include "StartupSequence.h"
#include "RowByRow.h"
#include "AmericanFlag.h"

uint8_t currentMode = 255;
uint8_t lastMode = 255;
uint8_t brightness = 0;
uint8_t color = 0;
uint8_t config = 0;
SPIController spiController;

void setup()
{
    spiController.begin();
    Serial.begin(9600);
    Serial1.begin(115200);
}

void loop()
{
    static uint8_t uartBuffer[4];
    static uint8_t bytesRead = 0;
    static unsigned long lastByteTime = 0;  // Time of last byte received
    const unsigned long timeout = 100;      // Timeout in milliseconds

    while (Serial1.available() > 0)
    {
        uint8_t byte = Serial1.read();
        unsigned long currentTime = millis();

        // Reset if timeout occurred
        if (currentTime - lastByteTime > timeout)
        {
            bytesRead = 0;
        }
        lastByteTime = currentTime;

        // Check for start byte to re-synchronize if data is misaligned
        if (bytesRead == 0 && byte != 0xFF)
        {
            continue;  // Ignore bytes until the start byte (0xFF) is found
        }

        // Store the byte in the buffer
        uartBuffer[bytesRead++] = byte;

        if (bytesRead == 4)
        {
            // Parse the received data
            currentMode = uartBuffer[1];       // Mode is now the second byte
            MAX_BRIGHTNESS = uartBuffer[2];    // Brightness is the third byte
            config = uartBuffer[3];            // Configuration is the fourth byte

            // Increment config to indicate data received
            uartBuffer[3] = uartBuffer[3] + 1;

            // Send feedback to the next panel in the chain
            Serial1.write(uartBuffer, 4);

            bytesRead = 0;  // Reset bytesRead for the next packet

            // Debugging output to verify data
            Serial.println("---------------------------------");
            Serial.print("Mode: ");
            Serial.println(currentMode);
            Serial.print("Brightness: ");
            Serial.println(MAX_BRIGHTNESS);
            Serial.println("---------------------------------");

            // Update pattern if mode has changed
            if (currentMode != lastMode)
            {
                lastMode = currentMode;
                setupPattern();
            }
        }
    }

    loopPattern();
}

void setupPattern()
{
    // Reset SPI and clear LEDs when changing modes
    spiController.clear(NUM_LEDS);
    spiController.begin();

    switch (currentMode)
    {
    case 0:
        Confetti_setup(spiController);
        break;
    case 1:
        ColorCascade_setup(spiController);
        break;
    case 2:
        OneAtATimeConfetti_setup(spiController, NUM_LEDS);
        break;
    case 3:
        TwinklingStars_setup(spiController);
        break;
    case 4:
        Ripple_setup(spiController);
        break;
    case 5:
        Sparkle_setup(spiController);
        break;
    case 6:
        FireEffect_setup(spiController);
        break;
    case 7:
        Breathing_setup(spiController);
        break;
    case 8:
        Meteor_setup(spiController);
        break;
    case 9:
        ColorFade_setup(spiController);
        break;
        /*
    case 10:
        StartupSequence_setup(spiController);
        break;
        */
    case 11:
        RowByRow_setup(spiController);
        break;
    case 12:
        AmericanFlag_setup(spiController);
        break;
    default:
        Breathing_setup(spiController);
        break;
    }
}

void loopPattern()
{
    switch (currentMode)
    {
    case 0:
        Confetti_loop(spiController, NUM_LEDS);
        break;
    case 1:
        ColorCascade_loop(spiController, NUM_LEDS);
        break;
    case 2:
        OneAtATimeConfetti_loop(spiController, NUM_LEDS);
        break;
    case 3:
        TwinklingStars_loop(spiController, NUM_LEDS);
        break;
    case 4:
        Ripple_loop(spiController, NUM_LEDS);
        break;
    case 5:
        Sparkle_loop(spiController, NUM_LEDS);
        break;
    case 6:
        FireEffect_loop(spiController, NUM_LEDS);
        break;
    case 7:
        Breathing_loop(spiController, NUM_LEDS);
        break;
    case 8:
        Meteor_loop(spiController, NUM_LEDS);
        break;
    case 9:
        ColorFade_loop(spiController, NUM_LEDS);
        break;
        /*
    case 10:
        StartupSequence_loop(spiController, NUM_LEDS);
        break;
        */
    case 11:
        RowByRow_loop(spiController);
        break;
    case 12:
        AmericanFlag_loop(spiController);
        break;
    case 255:
        Serial.println("Invalid Mode");
        break;
    default:
        Breathing_loop(spiController, NUM_LEDS);
        break;
    }
}
