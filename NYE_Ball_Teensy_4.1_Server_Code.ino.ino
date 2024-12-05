// 12-3-24
#include <QNEthernet.h>
#include <Arduino.h>
#include <TimeLib.h>  // Added for RTC
using namespace qindesign::network;

#define TX_PIN 21  
#define RX_PIN 20  

uint8_t mode = 0x00;
uint8_t brightness = 3;
bool isSendingData = false;
bool ledOutputEnabled = true;
bool randomModeEnabled = false;

struct Mode 
{
    int number; // Arbitrary mode number
    String name; // Mode name
};

// Define the modes
const int NUM_MODES = 10; // Adjust based on the number of modes
Mode modes[NUM_MODES] = 
{
    {1, "Favorite 1"},
    {2, "Favorite 2"},
    {3, "Favorite 3"},
    {4, "Favorite 4"},
    {5, "Favorite 5"},
    {6, "Favorite 6"},
    {7, "Favorite 7"},
    {8, "Favorite 8"},
    {9, "Favorite 9"},
    {10, "Favorite 10"}
};

EthernetServer server(80);

float modeChangeInterval = 60.0; // Seconds
unsigned long lastModeChangeTime = 0;

void setup() 
{
    Serial1.begin(115200, SERIAL_8N1);  // UART for Teensy
    Serial.begin(115200);

    // Ethernet Initialization
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip(192, 168, 1, 50);
    Ethernet.begin(mac, ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield not found. Halting.");
        while (true);
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    server.begin();
    Serial.println("Server started at: http://192.168.1.50");
}

void loop() 
{
    handleClient();

    // Random mode switching
    if (randomModeEnabled && millis() - lastModeChangeTime >= modeChangeInterval * 1000) {
        switchToRandomMode();
        lastModeChangeTime = millis();
    }
}

void handleClient()
{
    EthernetClient client = server.available();
    if (client) {
        Serial.println("New client connected");
        String requestLine = "";
        String currentLine = "";
        bool isRequestLine = true;

        // Read the HTTP headers
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // End of headers
                        break;
                    }
                    if (isRequestLine) {
                        requestLine = currentLine;
                        isRequestLine = false;
                    }
                    currentLine = "";
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }

        // Handle favicon request
        if (requestLine.startsWith("GET /favicon.ico")) {
            sendResponse(client, ""); // Send empty response for favicon requests
            return;
        }

        // Handle specific requests
        else if (requestLine.startsWith("GET /setMode?mode=")) {
            int modeStart = requestLine.indexOf("mode=") + 5;
            String modeString = requestLine.substring(modeStart, requestLine.indexOf(' ', modeStart));
            mode = modeString.toInt();

            // Disable random mode when mode 254 is selected
            if (mode == 254) {
                randomModeEnabled = false;
                lastModeChangeTime = millis(); // Reset the timer to prevent unexpected mode changes
            }

            sendUARTData();
            sendResponse(client, "Mode set to " + String(mode));
        }
        else if (requestLine.startsWith("GET /setBrightness?brightness=")) {
            int brightnessStart = requestLine.indexOf("brightness=") + 11;
            String brightnessString = requestLine.substring(brightnessStart, requestLine.indexOf(' ', brightnessStart));
            brightness = brightnessString.toInt();
            sendUARTData();
            sendResponse(client, "Brightness set to " + String(brightness));
        }
        else if (requestLine.startsWith("GET /toggleLEDOutput")) {
            ledOutputEnabled = !ledOutputEnabled;
            sendUARTData();
            sendResponse(client, ledOutputEnabled ? "Enabled" : "Disabled");
        }
        else if (requestLine.startsWith("GET /toggleRandomMode")) {
            randomModeEnabled = !randomModeEnabled;
            lastModeChangeTime = millis(); // Reset timer when toggling random mode
            sendResponse(client, randomModeEnabled ? "Enabled" : "Disabled");
        }
        else if (requestLine.startsWith("GET /setInterval?interval=")) {
            int intervalStart = requestLine.indexOf("interval=") + 9;
            String intervalString = requestLine.substring(intervalStart, requestLine.indexOf(' ', intervalStart));
            modeChangeInterval = intervalString.toFloat() * 60.0;
            lastModeChangeTime = millis(); // Reset timer for the new interval
            sendResponse(client, "Interval set to " + String(modeChangeInterval / 60.0, 2) + " minutes");
        }
        else if (requestLine.startsWith("GET /setTime?time=")) {
            int timeStart = requestLine.indexOf("time=") + 5;
            String timeString = requestLine.substring(timeStart, requestLine.indexOf(' ', timeStart));
            unsigned long epochTime = timeString.toInt();
            setTime(epochTime);
            sendResponse(client, "Time set to " + String(epochTime));
        }
        else if (requestLine.startsWith("GET /status")) {
            sendStatus(client);
        }
        else {
            // Serve the HTML Page
            serveHTML(client);
        }
    }
}

void sendResponse(EthernetClient &client, const String &message) 
{
    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/plain\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + String(message.length()) + "\r\n";
    response += "\r\n";
    response += message;

    client.print(response);
    delay(1);
    client.stop();
}

void sendStatus(EthernetClient &client) 
{
    unsigned long timeRemaining = 0;
    if (randomModeEnabled) {
        timeRemaining = (modeChangeInterval * 1000 > (millis() - lastModeChangeTime))
                            ? (modeChangeInterval * 1000 - (millis() - lastModeChangeTime)) / 1000
                            : 0;
    }
    String jsonResponse = "{";
    jsonResponse += "\"mode\": " + String(mode) + ",";
    jsonResponse += "\"brightness\": " + String(brightness) + ",";
    jsonResponse += "\"ledOutputEnabled\": " + String(ledOutputEnabled ? "true" : "false") + ",";
    jsonResponse += "\"randomModeEnabled\": " + String(randomModeEnabled ? "true" : "false") + ",";
    jsonResponse += "\"timeRemaining\": " + String(timeRemaining) + ",";
    jsonResponse += "\"modeChangeInterval\": " + String(modeChangeInterval) + ",";
    jsonResponse += "\"currentTime\": " + String(now());
    jsonResponse += "}";

    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: application/json\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + String(jsonResponse.length()) + "\r\n";
    response += "\r\n";
    response += jsonResponse;

    client.print(response);
    delay(1);
    client.stop();
}

void serveHTML(EthernetClient &client)
{
    String htmlContent = "<!DOCTYPE HTML><html><head><style>";
    htmlContent += ".enabled { background-color: green; color: white; }";
    htmlContent += ".disabled { background-color: red; color: white; }";
    htmlContent += ".selected { background-color: green; color: white; }";
    htmlContent += "</style></head><body><h1>Teensy Mode Control</h1>";
    htmlContent += "<p>Current Mode: <span id='currentMode'></span></p>";
    htmlContent += "<p>Brightness Level: <span id='brightnessValue'></span></p>";
    htmlContent += "<p>LED Output: <span id='ledOutputState'></span></p>";
    htmlContent += "<p>Mode Change Interval: <span id='modeInterval'></span></p>";
    htmlContent += "<p id='timeUntilNextModeChange' style='display:none;'>Time Until Next Mode Change: <span id='timeLeft'></span></p>";
    htmlContent += "<p>Current Teensy Time: <span id='teensyTime'></span></p>";
    htmlContent += "<button onclick='syncTime()'>Sync Time</button>";

    htmlContent += "<h3>Set Mode Change Interval</h3>";
    htmlContent += "<input type='number' id='intervalInput' step='0.1' min='0.1' value='" + String(modeChangeInterval / 60.0, 2) + "'>";
    htmlContent += "<button onclick='setIntervalTime()'>Set Interval</button>";

    // Generate HTML for mode selection
    htmlContent += "<h3>Select Mode</h3>";
    for (int i = 0; i < NUM_MODES; ++i) 
    {
      htmlContent += "<button id='btn" + String(modes[i].number) + "' onclick=\"setMode(" + String(modes[i].number) + ")\">" + modes[i].name + "</button><br>";
    }
    
    // Add the Countdown Mode button
    htmlContent += "<br><button id='btn254' onclick=\"setMode(254)\">Countdown Mode</button><br>";

    // Add the Skip Mode button

    htmlContent += "<h3>Adjust Brightness</h3>";
    htmlContent += "<button onclick='adjustBrightness(-1)'>&#8595;</button>";
    htmlContent += "<input type='range' min='0' max='31' id='brightnessSlider' oninput='setBrightness(this.value)' />";
    htmlContent += "<button onclick='adjustBrightness(1)'>&#8593;</button>";

    htmlContent += "<h3>Enable LED Output</h3>";
    htmlContent += "<button id='ledToggleButton' onclick='toggleLEDOutput()' class='" + String(ledOutputEnabled ? "enabled" : "disabled") + "'>" + String(ledOutputEnabled ? "Disable LED Output" : "Enable LED Output") + "</button>";

    htmlContent += "<h3>Enable Random Mode Switching</h3>";
    htmlContent += "<button id='randomModeButton' onclick='toggleRandomMode()' class='" + String(randomModeEnabled ? "enabled" : "disabled") + "'>" + String(randomModeEnabled ? "Disable Random Mode" : "Enable Random Mode") + "</button>";

    // JavaScript Code
    htmlContent += "<script>";
    htmlContent += "var timeRemaining = 0;";
    htmlContent += "var lastUpdateTime = Date.now();";
    htmlContent += "var randomModeEnabled = false;";

    htmlContent += "function updateStatus() {";
    htmlContent += "  fetch('/status').then(response => response.json()).then(data => {";
    htmlContent += "    document.getElementById('currentMode').innerText = data.mode;";
    htmlContent += "    document.getElementById('brightnessValue').innerText = data.brightness;";
    htmlContent += "    document.getElementById('ledOutputState').innerText = data.ledOutputEnabled ? 'Enabled' : 'Disabled';";
    htmlContent += "    document.getElementById('modeInterval').innerText = (data.modeChangeInterval / 60).toFixed(2) + ' minutes';";
    htmlContent += "    timeRemaining = data.timeRemaining;";
    htmlContent += "    lastUpdateTime = Date.now();";
    htmlContent += "    randomModeEnabled = data.randomModeEnabled;";
    htmlContent += "    document.querySelectorAll('button[id^=btn]').forEach(btn => btn.classList.remove('selected'));";
    htmlContent += "    document.getElementById('btn' + data.mode).classList.add('selected');";
    htmlContent += "    document.getElementById('ledToggleButton').className = data.ledOutputEnabled ? 'enabled' : 'disabled';";
    htmlContent += "    document.getElementById('ledToggleButton').innerText = data.ledOutputEnabled ? 'Disable LED Output' : 'Enable LED Output';";
    htmlContent += "    document.getElementById('randomModeButton').className = data.randomModeEnabled ? 'enabled' : 'disabled';";
    htmlContent += "    document.getElementById('randomModeButton').innerText = data.randomModeEnabled ? 'Disable Random Mode' : 'Enable Random Mode';";
    htmlContent += "    var teensyTime = new Date(data.currentTime * 1000);";
    htmlContent += "    document.getElementById('teensyTime').innerText = teensyTime.toLocaleString();";

    // Show or hide the countdown timer
    htmlContent += "    if (randomModeEnabled) {";
    htmlContent += "      document.getElementById('timeUntilNextModeChange').style.display = 'block';";
    htmlContent += "    } else {";
    htmlContent += "      document.getElementById('timeUntilNextModeChange').style.display = 'none';";
    htmlContent += "      document.getElementById('timeLeft').innerText = '';";
    htmlContent += "    }";
    htmlContent += "  });";
    htmlContent += "}";

    htmlContent += "setInterval(updateStatus, 1000);";
    htmlContent += "updateStatus();";

    htmlContent += "setInterval(function() {";
    htmlContent += "  if (randomModeEnabled) {";
    htmlContent += "    var elapsed = (Date.now() - lastUpdateTime) / 1000;";
    htmlContent += "    var displayTime = Math.max(0, timeRemaining - elapsed);";
    htmlContent += "    document.getElementById('timeLeft').innerText = Math.floor(displayTime) + ' seconds';";
    htmlContent += "  } else {";
    htmlContent += "    document.getElementById('timeLeft').innerText = '';";
    htmlContent += "  }";
    htmlContent += "}, 1000);";

    htmlContent += "function setMode(m) {";
    htmlContent += "  fetch('/setMode?mode=' + m).then(() => updateStatus());";
    htmlContent += "}";
    htmlContent += "function setIntervalTime() {";
    htmlContent += "  let interval = document.getElementById('intervalInput').value;";
    htmlContent += "  fetch('/setInterval?interval=' + interval).then(() => updateStatus());";
    htmlContent += "}";
    htmlContent += "function setBrightness(b) {";
    htmlContent += "  fetch('/setBrightness?brightness=' + b).then(() => updateStatus());";
    htmlContent += "}";
    htmlContent += "function adjustBrightness(change) {";
    htmlContent += "  let slider = document.getElementById('brightnessSlider');";
    htmlContent += "  slider.value = parseInt(slider.value) + change;";
    htmlContent += "  setBrightness(slider.value);";
    htmlContent += "}";
    htmlContent += "function toggleLEDOutput() {";
    htmlContent += "  fetch('/toggleLEDOutput').then(() => updateStatus());";
    htmlContent += "}";
    htmlContent += "function toggleRandomMode() {";
    htmlContent += "  fetch('/toggleRandomMode').then(() => updateStatus());";
    htmlContent += "}";
    htmlContent += "function syncTime() {";
    htmlContent += "  var currentTime = Math.floor(Date.now() / 1000);";
    htmlContent += "  fetch('/setTime?time=' + currentTime).then(() => updateStatus());";
    htmlContent += "}";
    htmlContent += "</script></body></html>";

    String response = "HTTP/1.1 200 OK\r\n";
    response += "Content-Type: text/html\r\n";
    response += "Connection: close\r\n";
    response += "Content-Length: " + String(htmlContent.length()) + "\r\n";
    response += "\r\n";
    response += htmlContent;

    client.print(response);
    delay(1);
    client.stop();
}

void switchToRandomMode()
{
    mode = random(0, 21); // Random mode between 0 and 20
    sendUARTData();
    Serial.println("Switched to random mode: " + String(mode));
}

void sendUARTData() 
{
    if (isSendingData) return;

    isSendingData = true;
    Serial1.write(0xFF);                 
    Serial1.write(mode);                 
    Serial1.write(ledOutputEnabled ? brightness : 0);
    Serial1.write(0x00);                 
    delay(50);
    isSendingData = false;
}
