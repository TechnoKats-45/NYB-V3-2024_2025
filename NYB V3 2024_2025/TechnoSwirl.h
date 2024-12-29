#ifndef TECHNOSWIRL_H
#define TECHNOSWIRL_H

#include <Arduino.h>
#include "SPIController.h"

/**
 * @brief Sets up the TechnoSwirl pattern (e.g., initializes LED positions).
 *
 * @param spiController Reference to the SPI controller used for driving LEDs.
 */
void TechnoSwirl_setup(SPIController& spiController);

/**
 * @brief Main loop function for the TechnoSwirl pattern (updates LED colors and sends data).
 *
 * @param spiController Reference to the SPI controller used for driving LEDs.
 */
void TechnoSwirl_loop(SPIController& spiController);

#endif // TECHNOSWIRL_H
