#include "CountDownTimer.h"
#include <Arduino.h>
#include <math.h>
#include "SPIController.h"
#include "Confetti.h"

// Variables
int countDownTime = 10;  // Starting countdown time in seconds
unsigned long lastSecondTime = 0;    // For tracking elapsed seconds
unsigned long fadeOutStartTime = 0;  // For fade-out timing
const int numLEDs = 60;              // Number of LEDs in your panel

SPIController spiController;

enum State { COUNTDOWN, FADEOUT, EXPLOSION };
State currentState = COUNTDOWN;

void CountDownTimer_setup()
{
    spiController.begin();
    lastSecondTime = millis();
    Confetti_setup(spiController);
}

void CountDownTimer_loop()
{
    unsigned long currentTime = millis();

    switch (currentState)
    {
    case COUNTDOWN:
        // Decrease the countdown timer every second
        if (currentTime - lastSecondTime >= 1000)
        {
            lastSecondTime += 1000;
            countDownTime--;
            if (countDownTime <= 0)
            {
                currentState = FADEOUT;
                fadeOutStartTime = currentTime;
            }
        }

        // Pulsing brightness effect
        {
            float timeInSecond = (currentTime % 1000) / 1000.0;  // Fraction of the current second
            uint8_t minBrightness = 5;
            uint8_t maxBrightness = 31;
            float brightnessFloat = minBrightness + (maxBrightness - minBrightness) *
                (0.5 * (1 + cos(2 * PI * timeInSecond)));
            uint8_t brightness = (uint8_t)brightnessFloat;

            // Update LEDs with pulsing brightness
            spiController.sendStartFrame();
            for (int i = 0; i < numLEDs; i++)
            {
                spiController.sendColor(brightness, 255, 255, 255);  // White color
            }
            spiController.sendEndFrame(numLEDs);
        }
        break;

    case FADEOUT:
        // Fade out brightness over 2 seconds
    {
        unsigned long elapsedFadeTime = currentTime - fadeOutStartTime;
        if (elapsedFadeTime >= 2000)
        {
            currentState = EXPLOSION;
        }
        else
        {
            uint8_t brightness = (uint8_t)(31 * (1 - elapsedFadeTime / 2000.0));
            spiController.sendStartFrame();
            for (int i = 0; i < numLEDs; i++)
            {
                spiController.sendColor(brightness, 255, 255, 255);  // White color fading out
            }
            spiController.sendEndFrame(numLEDs);
        }
    }
    break;

    case EXPLOSION:
        // Explosion effect using Confetti pattern
        Confetti_loop(spiController, numLEDs);
        break;
    }
}
