#include "SPIController.h"
#include "globals.h"
#include <EEPROM.h>

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
#include "CheckerBoard.h"
#include "Fireworks.h"
#include "Snowfall.h"
#include "WaveEffect.h"
#include "CountdownCircle.h"
#include "HypnoticSpiral.h"
#include "BouncingWave.h"
#include "Aurora.h"
#include "Countdown.h"
#include "GalaxySwirl.h"
#include "TidalWave.h"
#include "LaserSweep.h"
#include "SpectrumBars.h"
#include "MosaicMorph.h"
#include "NeonStripes.h"
#include "EchoingCircles.h"
#include "TurnOffLEDs.h"
#include "RadiatingRainbow.h"
#include "DiamondWeave.h"
#include "TriadVortex.h"
#include "PlasmaGrid.h"
#include "KaleidoPulse.h"
#include "FractalBloom.h"
#include "DragonsBreath.h"
#include "PulsarRings.h"
#include "PrismTwist.h"
#include "FireflySwarm.h"
#include "TechnologicalStreaks.h"
#include "InversePinwheel.h"
#include "Pinwheel.h"
#include "PulsingWaves.h"
#include "EquilateralRipple.h"
#include "TriangularCheckerboard.h"
#include "TriangularSpiral.h"
#include "InwardCollapse.h"
#include "OrbitingPoints.h"
#include "TechnoSwirl.h"

uint8_t currentMode = 253;
uint8_t lastMode = 255;
uint8_t brightness = 0;
uint8_t color = 0;
uint8_t config = 0;
SPIController spiController;

#define DEBUGSERIAL // Only have commented in for DEBUG Statements to be ENABLED

bool modeChanged = false;  // Flag to indicate if the mode has changed at least once
const unsigned long timeout = 100;      // Timeout in milliseconds

void setup()
{
    spiController.begin();
    Serial1.begin(115200);
	EEPROM.write(0, 0);  // Reset the color index to red on startup

    #ifdef DEBUGSERIAL
        Serial.begin(9600);
    	Serial.println("Starting NYB V3 2024/2025");
    #endif

}

void loop()
{
    static uint8_t uartBuffer[4];
    static uint8_t bytesRead = 0;
    static unsigned long lastByteTime = 0;  // Time of last byte received

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
            #ifdef DEBUGSERIAL
                Serial.println("---------------------------------");
                Serial.print("Mode: ");
                Serial.println(currentMode);
                Serial.print("Brightness: ");
                Serial.println(MAX_BRIGHTNESS);
                Serial.println("---------------------------------");
            #endif  

            // Update pattern if mode has changed
            if (currentMode != lastMode)
            {
                lastMode = currentMode;
                modeChanged = true;  // Set the flag when mode changes for the first time
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

    if (!modeChanged)
    {
        // If mode hasn't changed, stay in mode 253
        TurnOffLEDs_setup(spiController);
        return;
    }

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
        Checkerboard_setup(spiController);
        break;
    case 8:
        Meteor_setup(spiController);
        break;
    case 9:
        ColorFade_setup(spiController);
        break;
    case 10:
        Fireworks_setup(spiController);
        break;
    case 11:
        RowByRow_setup(spiController);
        break;
    case 12:
        AmericanFlag_setup(spiController);
        break;
    case 13:
		Snowfall_setup(spiController);
		break;
    case 14:
        WaveEffect_setup(spiController);
        break;
    case 15:
        CountdownCircle_setup(spiController);
        break;
    case 16:
        HypnoticSpiral_setup(spiController);
        break;
    case 17:
		BouncingWaveEffect_setup(spiController);
		break;
    case 18:
		AuroraEffect_setup(spiController);
		break;
    case 19:
		GalaxySwirl_setup(spiController);
        break;
    case 20:
		TidalWave_setup(spiController);
		break;
    case 21:
		LaserSweep_setup(spiController);
		break;
    case 22:
		SpectrumBars_setup(spiController);
		break;
    case 23:
		MosaicMorph_setup(spiController);
		break;
    case 24:
		NeonStripes_setup(spiController);
		break;
    case 25:
		EchoingCircles_setup(spiController);
		break;
    case 26:
		RadiatingRainbow_setup(spiController);
		break;
    case 27:
		DiamondWeave_setup(spiController);
		break;
    case 28:
		TriadVortex_setup(spiController);
		break;
    case 29:
		PlasmaGrid_setup(spiController);
		break;
    case 30:
		KaleidoPulse_setup(spiController);
		break;
    case 31:
		FractalBloom_setup(spiController);
		break;
    case 32:
		DragonsBreath_setup(spiController);
		break;
    case 33:
		PulsarRings_setup(spiController);
		break;
    case 34:
		PrismTwist_setup(spiController);
		break;
    case 35:
		FireflySwarm_setup(spiController);
		break;
    case 36:
		TechnologicalStreaks_setup(spiController);
		break;
    case 37:
        InversePinwheel_setup(spiController);
		break;
    case 38:
		Pinwheel_setup(spiController);
		break;
    case 39:
		PulsingWaves_setup(spiController);
		break;
    case 40:
		EquilateralRipple_setup(spiController);
		break;
    case 41:
		TriangularCheckerboard_setup(spiController);
		break;
    case 42:
		TriangularSpiral_setup(spiController);
		break;
    case 43:
		InwardCollapse_setup(spiController);
		break;
    case 44:
		OrbitingPoints_setup(spiController);
		break;
    case 45:
		TechnoSwirl_setup(spiController);
		break;
    case 253:
		TurnOffLEDs_setup(spiController);
		break;
	case 254:
		CountdownEffect_setup(spiController);
        break;
    default:
        Breathing_setup(spiController);
        break;
    }
}

void loopPattern()
{
    if (!modeChanged)
    {
        // If mode hasn't changed, stay in mode 253
        TurnOffLEDs_loop(spiController);
        return;
    }

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
		Checkerboard_loop(spiController);
        break;
    case 8:
        Meteor_loop(spiController, NUM_LEDS);
        break;
    case 9:
        ColorFade_loop(spiController, NUM_LEDS);
        break;
    case 10:
        Fireworks_loop(spiController);
        break;
    case 11:
        RowByRow_loop(spiController, currentMode);
        break;
    case 12:
        AmericanFlag_loop(spiController);
        break;
	case 13:
		Snowfall_loop(spiController);
		break;
    case 14:
        WaveEffect_loop(spiController);
        break;
    case 15:
        CountdownCircle_loop(spiController);
        break;
    case 16:
        HypnoticSpiral_loop(spiController);
        break;
	case 17:
		BouncingWaveEffect_loop(spiController);
		break;
    case 18:
		AuroraEffect_loop(spiController);
		break;
    case 19:
		GalaxySwirl_loop(spiController);
		break;
    case 20:
		TidalWave_loop(spiController);
		break;
    case 21:
		LaserSweep_loop(spiController);
		break;
    case 22:
		SpectrumBars_loop(spiController);
		break;
    case 23:
		MosaicMorph_loop(spiController);
		break;
    case 24:
		NeonStripes_loop(spiController);
		break;
    case 25:
		EchoingCircles_loop(spiController);
		break;
    case 26:
		RadiatingRainbow_loop(spiController);
		break;
	case 27:
		DiamondWeave_loop(spiController);
		break;
    case 28:
		TriadVortex_loop(spiController);
		break;
    case 29:
		PlasmaGrid_loop(spiController);
		break;
    case 30:
		KaleidoPulse_loop(spiController);
		break;
    case 31:
		FractalBloom_loop(spiController);
		break;
    case 32:
		DragonsBreath_loop(spiController);
		break;
    case 33:
		PulsarRings_loop(spiController);
		break;
    case 34:
		PrismTwist_loop(spiController);
        break;
	case 35:
		FireflySwarm_loop(spiController);
		break;
    case 36:
		TechnologicalStreaks_loop(spiController);
		break;
    case 37:
        InversePinwheel_loop(spiController);
		break;
    case 38:
		Pinwheel_loop(spiController);
		break;
    case 39:
		PulsingWaves_loop(spiController);
		break;
    case 40:
		EquilateralRipple_loop(spiController);
		break;
    case 41:
		TriangularCheckerboard_loop(spiController);
		break;
    case 42:
		TriangularSpiral_loop(spiController);
		break;
    case 43:
		InwardCollapse_loop(spiController);
		break;
    case 44:
		OrbitingPoints_loop(spiController);
		break;
    case 45:
		TechnoSwirl_loop(spiController);
		break;
    case 253:
		TurnOffLEDs_loop(spiController);
		break;
	case 254:
		CountdownEffect_loop(spiController);
        break;
    case 255:
        #ifdef DEBUGSERIAL
            Serial.println("Invalid Mode");
        #endif
        Breathing_loop(spiController, NUM_LEDS);
        break;
    default:
        Breathing_loop(spiController, NUM_LEDS);
        break;
    }
}
