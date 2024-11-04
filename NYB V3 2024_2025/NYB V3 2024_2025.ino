#include "SPIController.h"
#include "globals.h"

#include "ColorFade.h"
#include "Confetti.h"
#include "TwinklingStars.h"
#include "Ripple.h"
#include "Sparkle.h"
#include "Breathing.h"
#include "Meteor.h"
#include "OneAtATimeConfetti.h"

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

    while (Serial1.available() > 0)
    {
        uartBuffer[bytesRead++] = Serial1.read();

        if (bytesRead == 4)
        {
            currentMode = uartBuffer[0];
            MAX_BRIGHTNESS = uartBuffer[1];

            config = uartBuffer[3];
            uartBuffer[3] = uartBuffer[3] + 1;  // Increment config to indicate data received

            Serial1.write(uartBuffer, 4);   // Echoes data to the next panel in the chain

            bytesRead = 0;

            Serial.println("---------------------------------");
            Serial.print("Mode: ");
            Serial.println(currentMode);
            Serial.print("Brightness: ");
            Serial.println(MAX_BRIGHTNESS);
            Serial.println("---------------------------------");

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
    switch (currentMode)
    {
    case 0:
        Confetti_setup(spiController);
        break;
    case 1:
        ColorFade_setup(spiController);
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
    case 7:
        Breathing_setup(spiController);
        break;
    case 8:
        Meteor_setup(spiController);
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
        ColorFade_loop(spiController, NUM_LEDS);
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
    case 7:
        Breathing_loop(spiController, NUM_LEDS);
        break;
    case 8:
        Meteor_loop(spiController, NUM_LEDS);
        break;
    case 255:
        Serial.println("Invalid Mode");
        break;
    default:
        Breathing_loop(spiController, NUM_LEDS);
        break;
    }
}
