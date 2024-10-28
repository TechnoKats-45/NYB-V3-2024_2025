#include "LEDDisplay.h"

// Constructor
LEDDisplay::LEDDisplay() {
    initializePanels();
}

// Initialize the SPI controller and any other setup
void LEDDisplay::begin() {
    spiController.begin();
}

// Initialize panel configurations
void LEDDisplay::initializePanels() {
    // Initialize the panels array with panel numbers, number of LEDs, and LED indices
    panels[0] = { 1, 26, 0, 26 };
    panels[1] = { 2, 28, 26, 54 };
    panels[2] = { 3, 28, 54, 82 };
    panels[3] = { 4, 36, 82, 118 };
    panels[4] = { 5, 28, 118, 146 };
    panels[5] = { 6, 28, 146, 174 };
    panels[6] = { 7, 26, 174, 200 };
    panels[7] = { 8, 28, 200, 228 };
    panels[8] = { 9, 36, 228, 264 };
    panels[9] = { 10, 28, 264, 292 };
    panels[10] = { 11, 28, 292, 320 };
    panels[11] = { 12, 26, 320, 346 };
    panels[12] = { 13, 28, 346, 374 };
    panels[13] = { 14, 36, 374, 410 };
    panels[14] = { 15, 28, 410, 438 };
    panels[15] = { 16, 36, 438, 474 };

    // Initialize the ledData array to zeros (turn off all LEDs)
    for (int i = 0; i < numLEDs; i++) {
        ledData[i][0] = 0x00; // Brightness
        ledData[i][1] = 0x00; // Red
        ledData[i][2] = 0x00; // Green
        ledData[i][3] = 0x00; // Blue
    }
}

// Display a number (0-99) on the LED panels
void LEDDisplay::displayNumber(int number) {
    if (number < 0 || number > 99) return; // Out of range
    int tens = number / 10;
    int units = number % 10;

    // Clear ledData
    for (int i = 0; i < numLEDs; i++) {
        ledData[i][0] = 0x00;
        ledData[i][1] = 0x00;
        ledData[i][2] = 0x00;
        ledData[i][3] = 0x00;
    }

    // Light up segments for tens digit (if applicable)
    if (number >= 10) {
        int panelsToLight[16];
        int numPanels = 0;
        getPanelsForDigit(tens, panelsToLight, numPanels);
        for (int i = 0; i < numPanels; i++) {
            setPanelColor(panelsToLight[i], 31, 255, 0, 0); // Brightness 31, red color
        }
    }

    // Light up segments for units digit
    {
        int panelsToLight[16];
        int numPanels = 0;
        getPanelsForDigit(units, panelsToLight, numPanels);
        for (int i = 0; i < numPanels; i++) {
            setPanelColor(panelsToLight[i], 31, 0, 0, 255); // Brightness 31, blue color
        }
    }

    // Send frame to LEDs
    sendFrame();
}

// Set the color of a specific panel
void LEDDisplay::setPanelColor(int panelNumber, uint8_t brightness, uint8_t red, uint8_t green, uint8_t blue) {
    // Find the panel in the panels array
    int panelIndex = -1;
    for (int i = 0; i < 16; i++) {
        if (panels[i].panelNumber == panelNumber) {
            panelIndex = i;
            break;
        }
    }
    if (panelIndex == -1) return; // Panel not found

    // Set the color for each LED in the panel
    for (int i = panels[panelIndex].startLEDIndex; i < panels[panelIndex].endLEDIndex; i++) {
        ledData[i][0] = 0xE0 | (brightness & 0x1F); // Brightness
        ledData[i][1] = red;
        ledData[i][2] = green;
        ledData[i][3] = blue;
    }
}

// Send the LED data to the strip
void LEDDisplay::sendFrame() {
    spiController.sendStartFrame();
    for (int i = 0; i < numLEDs; i++) {
        spiController.sendColor(ledData[i][0] & 0x1F, ledData[i][1], ledData[i][2], ledData[i][3]);
    }
    spiController.sendEndFrame(numLEDs);
}

// Map digits to segments, and segments to panels
void LEDDisplay::getPanelsForDigit(int digit, int* panelsToLight, int& numPanels) {
    // Segment mapping for digits 0-9
    // Segments: A, B, C, D, E, F, G
    // Segments are represented by indices 0-6
    static const int segmentDigits[10][7] = {
        {1,1,1,1,1,1,0}, // 0
        {0,1,1,0,0,0,0}, // 1
        {1,1,0,1,1,0,1}, // 2
        {1,1,1,1,0,0,1}, // 3
        {0,1,1,0,0,1,1}, // 4
        {1,0,1,1,0,1,1}, // 5
        {1,0,1,1,1,1,1}, // 6
        {1,1,1,0,0,0,0}, // 7
        {1,1,1,1,1,1,1}, // 8
        {1,1,1,1,0,1,1}, // 9
    };

    // Map segments to panels (this mapping needs to be adjusted to your physical setup)
    // For example purposes, we're assigning arbitrary panels to segments
    static const int segmentToPanels[7][4] = {
        {1, -1, -1, -1},     // Segment A: Panel 1
        {3, 5, -1, -1},      // Segment B: Panels 3, 5
        {7, 6, -1, -1},      // Segment C: Panels 7, 6
        {9, 10, -1, -1},     // Segment D: Panels 9, 10
        {12, 11, -1, -1},    // Segment E: Panels 12, 11
        {15, 13, -1, -1},    // Segment F: Panels 15, 13
        {16, 14, 4, -1},     // Segment G: Panels 16, 14, 4
    };

    numPanels = 0;
    for (int seg = 0; seg < 7; seg++) {
        if (segmentDigits[digit][seg]) {
            for (int i = 0; i < 4; i++) {
                if (segmentToPanels[seg][i] != -1) {
                    panelsToLight[numPanels++] = segmentToPanels[seg][i];
                }
            }
        }
    }
}
