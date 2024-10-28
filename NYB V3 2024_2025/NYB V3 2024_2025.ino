/*
    Name:       NYB V3 2024_2025.ino
    Created:	10/23/2024 9:25:43 AM
    Author:     AM\jonathanl
    Purpose:    For use on the NYB-V3
*/

// Define User Types below here or use a .h file
//
#include "SPIController.h"      // For LEDs
#include "UARTController.h"     // For RX/TX mode from ESP32 and other Teensys
#include <iostream>
#include "CountDownTimer.h"
#include "Confetti.h"
#include "ColorFade.h"
#include "SPIController.h"
#include "Fireworks.h"
#include "TwinklingStars.h"
#include "Ripple.h"
#include "Sparkle.h"
#include "RainbowChase.h"
#include "Breathing.h"
#include "Meteor.h"
#include "Globals.h"

UARTController uartController;
SPIController spiController;

// Variables to control the mode
uint8_t currentMode = 255;  // Initialize to an invalid value so the setup is called at the start
uint8_t lastMode = 255;     // Keep track of the previous mode

// The setup() function runs once each time the micro-controller starts
void setup() 
{
    uartController.begin(9600);  // Initialize UART with a baud rate of 9600

	currentMode = 1;  // For Testing Only
}

// Add the main program code into the continuous loop() function
void loop() 
{
    // Check for incoming UART data
    uartController.checkForData();
    currentMode = uartController.getMode();  // Get the mode from the UART

	currentMode = 9;  // For Testing Only // TODO - Remove this line

    // If the mode has changed, call the corresponding setup function
    if (currentMode != lastMode) 
    {
        lastMode = currentMode;  // Update lastMode to prevent re-initialization

        // Switch setup for each mode
        switch (currentMode) 
        {
        case '0':
            Confetti_setup(spiController);
            break;
        case '1':
            ColorFade_setup(spiController);
            break;
        case '2':
            Fireworks_setup(spiController);
            break;
        case '3':
            TwinklingStars_setup(spiController);
            break;
        case '4':
            Ripple_setup(spiController);
            break;
        case '5':
            Sparkle_setup(spiController);
            break;
        case '6':
            RainbowChase_setup(spiController);
            break;
        case '7':
            Breathing_setup(spiController);
            break;
        case '8':
            Meteor_setup(spiController);
            break;      
		case '9':
            CountDownTimer_setup(spiController);
			break;
        
        default:
            Confetti_setup(spiController);  // Default to Confetti if mode is unrecognized  // TODO - Probably change this?
            break;
        }
    }

    // Execute the current pattern's loop based on the mode
    switch (currentMode) 
    {
    case '0':
        Confetti_loop(spiController, NUM_LEDS);
        break;
    case '1':
        ColorFade_loop(spiController, NUM_LEDS);
        break;
    case '2':
        Fireworks_loop(spiController, NUM_LEDS);
        break;
    case '3':
        TwinklingStars_loop(spiController, NUM_LEDS);
        break;
    case '4':
        Ripple_loop(spiController, NUM_LEDS);
        break;
    case '5':
        Sparkle_loop(spiController, NUM_LEDS);
        break;
    case '6':
        RainbowChase_loop(spiController, NUM_LEDS);
        break;
    case '7':
        Breathing_loop(spiController, NUM_LEDS);
        break;
    case '8':
        Meteor_loop(spiController, NUM_LEDS);
        break;
	case '9':
		CountDownTimer_loop(spiController);
		break;
    default:
        Confetti_loop(spiController, NUM_LEDS);  // Default to Confetti loop if mode is unrecognized    // TODO - Probably change this?
        break;
    }
}
