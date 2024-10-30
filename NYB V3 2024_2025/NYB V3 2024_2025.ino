/*
    Name:       NYB V3 2024_2025.ino
    Created:	10/23/2024 9:25:43 AM
    Author:     AM\jonathanl
    Purpose:    For use on the NYB-V3
*/

#include "SPIController.h"      // For LEDs
#include "globals.h"            // For global variables
#include "UARTController.h"

// LED Patterns:
#include "ColorFade.h"
#include "Confetti.h"
#include "TwinklingStars.h"
#include "Ripple.h"
#include "Sparkle.h"
#include "RainbowChase.h"
#include "Breathing.h"
#include "Meteor.h"
#include "OneAtATimeConfetti.h"
#include "FillPanel.h"

// Variables to control the mode
uint8_t currentMode = 255;  // Initialize to an invalid value so the setup is called at the start
uint8_t lastMode = 255;     // Keep track of the previous mode

SPIController spiController;
UARTController uartController(Serial1, 9600);   // Use Serial1 for UART communication

// The setup() function runs once each time the micro-controller starts
void setup() 
{
	spiController.begin();
    uartController.begin();
    Serial.begin(9600);

    //currentMode = 0;    // For setting the mode for testing
}

// Add the main program code into the continuous loop() function
void loop() 
{
    if (uartController.readMode()) 
    {
        currentMode = uartController.getCurrentMode(); 
        Serial1.write(currentMode);  // Echo received mode back on Serial1
		Serial.println(currentMode);
    }
    // If the mode has changed, call the corresponding setup function
    if (currentMode != lastMode)
    {
        lastMode = currentMode;  // Update lastMode to prevent re-initialization

        // Switch setup for each mode
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
        case 6:
            RainbowChase_setup(spiController);
            break;
        case 7:
            Breathing_setup(spiController);
            break;
        case 8:
            Meteor_setup(spiController);
            break;
        default:
            Confetti_setup(spiController);  // Default to Confetti if mode is unrecognized  // TODO - Probably change this?
            break;
        }
    }

    // Execute the current pattern's loop based on the mode
    switch (currentMode)
    {
    case 0:
        Confetti_loop(spiController, NUM_LEDS);
        Serial.println("Confetti Mode");
        break;
    case 1:
        ColorFade_loop(spiController, NUM_LEDS);
		Serial.println("Color Fade Mode");
        break;
	case 2:
		OneAtATimeConfetti_loop(spiController, NUM_LEDS);
		Serial.println("One At A Time Confetti Mode");
		break;
    case 3:
        TwinklingStars_loop(spiController, NUM_LEDS);
		Serial.println("Twinkling Stars Mode");
        break;
    case 4:
        Ripple_loop(spiController, NUM_LEDS);
		Serial.println("Ripple Mode");
        break;
    case 5:
        Sparkle_loop(spiController, NUM_LEDS);
		Serial.println("Sparkle Mode");
        break;
    case 6:
        RainbowChase_loop(spiController, NUM_LEDS);
		Serial.println("Rainbow Chase Mode");
        break;
    case 7:
        Breathing_loop(spiController, NUM_LEDS);
		Serial.println("Breathing Mode");
        break;
    case 8:
        Meteor_loop(spiController, NUM_LEDS);
		Serial.println("Meteor Mode");
        break;
    case 255:
		// Do nothing if the mode is invalid
		Serial.println("Invalid Mode");
		break;
    default:
        Confetti_loop(spiController, NUM_LEDS);  // Default to Confetti loop if mode is unrecognized    // TODO - Probably change this?
		Serial.println("Default Mode");
        break;
    }
}
