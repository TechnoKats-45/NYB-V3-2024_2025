#ifndef UARTCONTROLLER_H
#define UARTCONTROLLER_H

#include <Arduino.h>

class UARTController
{
public:
    UARTController(HardwareSerial& serialPort = Serial1, uint32_t baudRate = 9600);

    void begin();
    bool readMode();
    uint8_t getCurrentMode();

private:
    HardwareSerial& serial;
    uint32_t baud;
    uint8_t currentMode;
};

#endif // UARTCONTROLLER_H
