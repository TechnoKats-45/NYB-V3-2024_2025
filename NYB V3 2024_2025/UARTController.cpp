#include "UARTController.h"

// Constructor
UARTController::UARTController() : modeByte(0) {}

// Method to initialize UART with a given baud rate
void UARTController::begin(long baudRate) 
{
    Serial.begin(baudRate);
}

// Method to check for incoming data and read 1 byte
void UARTController::checkForData() 
{
    if (Serial.available() > 0) 
    {
        modeByte = Serial.read();   // Read 1 byte from UART
        Serial.write(modeByte);     // Echo the byte back on the UART
    }
}

// Method to return the last mode byte received
uint8_t UARTController::getMode() const 
{
    return modeByte;
}
