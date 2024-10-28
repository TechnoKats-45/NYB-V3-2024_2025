#ifndef UARTCONTROLLER_H
#define UARTCONTROLLER_H

#include <Arduino.h>

class UARTController
{
public:
    // Constructor
    UARTController();

    // Method to initialize UART
    void begin(long baudRate);

    // Method to check for incoming data and read 1 byte
    void checkForData();

    // Method to return the last mode byte received
    uint8_t getMode() const;

private:
    uint8_t modeByte;  // Store the received mode byte
};

#endif // UARTCONTROLLER_H
