#include "SPIController.h"
#include <SPI.h>

// Constructor
SPIController::SPIController() {}

// Function to initialize SPI (call this in setup)
void SPIController::begin() {
    SPI.begin();  // Initialize SPI
}

// Function to send start frame
void SPIController::sendStartFrame() {
    for (int i = 0; i < 4; i++) {
        SPI.transfer(0x00); // Send 32 bits of zeros as the start frame
    }
}

// Function to send end frame
void SPIController::sendEndFrame(int numLEDs) 
{
    for (int i = 0; i < (numLEDs / 2); i++) 
    {
        SPI.transfer(0xFF); // Send enough bits to latch
    }
}

// Function to send color data to one LED (APA102C format includes brightness)
void SPIController::sendColor(uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue) 
{
    SPI.transfer(0xE0 | (brightness & 0x1F));  // Brightness control
    SPI.transfer(blue);  // Send Blue component
    SPI.transfer(green); // Send Green component
    SPI.transfer(red);   // Send Red component
}

// Function to convert HSV to RGB
void SPIController::hsvToRgb(float hue, float sat, float val, uint8_t& red, uint8_t& green, uint8_t& blue) 
{
    int i = int(hue * 6);
    float f = hue * 6 - i;
    float p = val * (1 - sat);
    float q = val * (1 - f * sat);
    float t = val * (1 - (1 - f) * sat);

    switch (i % 6) 
    {
    case 0: red = val * 255; green = t * 255; blue = p * 255; break;
    case 1: red = q * 255; green = val * 255; blue = p * 255; break;
    case 2: red = p * 255; green = val * 255; blue = t * 255; break;
    case 3: red = p * 255; green = q * 255; blue = val * 255; break;
    case 4: red = t * 255; green = p * 255; blue = val * 255; break;
    case 5: red = val * 255; green = p * 255; blue = q * 255; break;
    }
}
