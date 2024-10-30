#include "UARTController.h"

// Constructor
UARTController::UARTController(HardwareSerial& serialPort, uint32_t baudRate)
    : serial(serialPort), baud(baudRate), currentMode(255) {}

// Begin serial communication
void UARTController::begin()
{
    serial.begin(baud);
}

// Read mode from UART if available
bool UARTController::readMode()
{
    if (serial.available() > 0)
    {
        uint8_t mode = serial.read();
        if (mode != currentMode)
        {
            currentMode = mode;
            return true;
        }
    }
    return false;
}

// Get the current mode
uint8_t UARTController::getCurrentMode()
{
    return currentMode;
}
