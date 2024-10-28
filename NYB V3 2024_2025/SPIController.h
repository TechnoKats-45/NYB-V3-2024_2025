#ifndef SPICONTROLLER_H
#define SPICONTROLLER_H

#include <SPI.h>

class SPIController {
public:
    SPIController();

    // Function to initialize SPI (call this in setup)
    void begin();

    // Function to send start frame
    void sendStartFrame();

    // Function to send end frame
    void sendEndFrame(int numLEDs);

    // Function to send color data to one LED (APA102C format includes brightness)
    void sendColor(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue);

    // Function to convert HSV to RGB
    void hsvToRgb(float hue, float sat, float val, uint8_t& red, uint8_t& green, uint8_t& blue);
};

#endif // SPICONTROLLER_H
