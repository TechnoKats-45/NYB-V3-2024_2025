#include <QNEthernet.h>
#include <Arduino.h>
using namespace qindesign::network;

#define TX_PIN 21  
#define RX_PIN 20  

uint8_t mode = 0x00;
uint8_t brightness = 3;
bool isSendingData = false;
bool ledOutputEnabled = true;
bool randomModeEnabled = false;

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
    if (!client) return;

    Serial.println("New client connected");
    String request = client.readStringUntil('\r');
    client.flush();

    // Handle specific requests
    if (request.indexOf("GET /setMode?mode=") >= 0) {
        int modeStart = request.indexOf("mode=") + 5;
        mode = request.substring(modeStart).toInt();
        sendUARTData();
        sendResponse(client, "Mode set to " + String(mode));
        return;
    }
    if (request.indexOf("GET /setBrightness?brightness=") >= 0) {
        int brightnessStart = request.indexOf("brightness=") + 11;
        brightness = request.substring(brightnessStart).toInt();
        sendUARTData();
        sendResponse(client, "Brightness set to " + String(brightness));
        return;
    }
    if (request.indexOf("GET /toggleLEDOutput") >= 0) {
        ledOutputEnabled = !ledOutputEnabled;
        sendUARTData();
        sendResponse(client, ledOutputEnabled ? "Enabled" : "Disabled");
        return;
    }
    if (request.indexOf("GET /toggleRandomMode") >= 0) {
        randomModeEnabled = !randomModeEnabled;
        lastModeChangeTime = millis(); // Reset timer when toggling random mode
        sendResponse(client, randomModeEnabled ? "Enabled" : "Disabled");
        return;
    }
    if (request.indexOf("GET /setInterval?interval=") >= 0) {
        int intervalStart = request.indexOf("interval=") + 9;
        modeChangeInterval = request.substring(intervalStart).toFloat() * 60.0;
        lastModeChangeTime = millis(); // Reset timer for the new interval
        sendResponse(client, "Interval set to " + String(modeChangeInterval / 60.0, 2) + " minutes");
        return;
    }
    if (request.indexOf("GET /status") >= 0) {
        sendStatus(client);
        return;
    }

    // Serve HTML Page
    serveHTML(client);
}

void sendResponse(EthernetClient &client, const String &message) 
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/plain");
    client.println("Connection: close");
    client.println();
    client.println(message);
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
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.println();
    client.println("{");
    client.println("  \"mode\": " + String(mode) + ",");
    client.println("  \"brightness\": " + String(brightness) + ",");
    client.println("  \"ledOutputEnabled\": " + String(ledOutputEnabled ? "true" : "false") + ",");
    client.println("  \"randomModeEnabled\": " + String(randomModeEnabled ? "true" : "false") + ",");
    client.println("  \"timeRemaining\": " + String(timeRemaining) + ",");
    client.println("  \"modeChangeInterval\": " + String(modeChangeInterval));  // Added line
    client.println("}");
    delay(1);
    client.stop();
}

void serveHTML(EthernetClient &client)
{
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println("<!DOCTYPE HTML><html><head><style>");
    client.println(".enabled { background-color: green; color: white; }");
    client.println(".disabled { background-color: red; color: white; }");
    client.println(".selected { background-color: green; color: white; }");
    client.println("</style></head><body><h1>Teensy Mode Control</h1>");
    client.println("<p>Current Mode: <span id='currentMode'></span></p>");
    client.println("<p>Brightness Level: <span id='brightnessValue'></span></p>");
    client.println("<p>LED Output: <span id='ledOutputState'></span></p>");
    client.println("<p>Mode Change Interval: <span id='modeInterval'></span></p>");
    client.println("<p id='timeUntilNextModeChange' style='display:none;'>Time Until Next Mode Change: <span id='timeLeft'></span></p>");

    client.println("<h3>Set Mode Change Interval</h3>");
    client.println("<input type='number' id='intervalInput' step='0.1' min='0.1' value='" + String(modeChangeInterval / 60.0, 2) + "'>");
    client.println("<button onclick='setIntervalTime()'>Set Interval</button>");

    client.println("<h3>Select Mode</h3>");
    for (int i = 0; i <= 20; ++i) {
        String modeName;
        switch (i) {
            case 0: modeName = "Confetti Mode"; break;
            case 1: modeName = "Color Cascade Mode"; break;
            case 2: modeName = "One At A Time Confetti Mode"; break;
            case 3: modeName = "Twinkling Stars Mode"; break;
            case 4: modeName = "Ripple Mode"; break;
            case 5: modeName = "Sparkle Mode"; break;
            case 6: modeName = "Fire Effect Mode"; break;
            case 7: modeName = "CheckerBoard Mode"; break;
            case 8: modeName = "Meteor Mode"; break;
            case 9: modeName = "Color Fade Mode"; break;
            case 10: modeName = "FireWorks Mode"; break;
            case 11: modeName = "Row By Row Mode"; break;
            case 12: modeName = "American Flag"; break;
            case 13: modeName = "SnowFall"; break;
            case 14: modeName = "WaveEffect"; break;
            case 15: modeName = "CountCircle"; break;
            case 16: modeName = "HypnoticSpiral"; break;
            case 17: modeName = "BouncingWave"; break;
            case 18: modeName = "Aurora"; break;
            case 19: modeName = "Test19"; break;
            case 20: modeName = "Test20"; break;
        }
        client.println("<button id='btn" + String(i) + "' onclick=\"setMode(" + String(i) + ")\">" + modeName + "</button><br>");
    }

    client.println("<h3>Adjust Brightness</h3>");
    client.println("<button onclick='adjustBrightness(-1)'>&#8595;</button>");
    client.println("<input type='range' min='0' max='31' id='brightnessSlider' oninput='setBrightness(this.value)' />");
    client.println("<button onclick='adjustBrightness(1)'>&#8593;</button>");

    client.println("<h3>Enable LED Output</h3>");
    client.println("<button id='ledToggleButton' onclick='toggleLEDOutput()' class='" + String(ledOutputEnabled ? "enabled" : "disabled") + "'>" + String(ledOutputEnabled ? "Disable LED Output" : "Enable LED Output") + "</button>");

    client.println("<h3>Enable Random Mode Switching</h3>");
    client.println("<button id='randomModeButton' onclick='toggleRandomMode()' class='" + String(randomModeEnabled ? "enabled" : "disabled") + "'>" + String(randomModeEnabled ? "Disable Random Mode" : "Enable Random Mode") + "</button>");

    // JavaScript Code
    client.println("<script>");
    client.println("var timeRemaining = 0;");
    client.println("var lastUpdateTime = Date.now();");
    client.println("var randomModeEnabled = false;");

    client.println("function updateStatus() {");
    client.println("  fetch('/status').then(response => response.json()).then(data => {");
    client.println("    document.getElementById('currentMode').innerText = data.mode;");
    client.println("    document.getElementById('brightnessValue').innerText = data.brightness;");
    client.println("    document.getElementById('ledOutputState').innerText = data.ledOutputEnabled ? 'Enabled' : 'Disabled';");
    client.println("    document.getElementById('modeInterval').innerText = (data.modeChangeInterval / 60).toFixed(2) + ' minutes';");
    client.println("    timeRemaining = data.timeRemaining;");
    client.println("    lastUpdateTime = Date.now();");
    client.println("    randomModeEnabled = data.randomModeEnabled;");
    client.println("    document.querySelectorAll('button[id^=btn]').forEach(btn => btn.classList.remove('selected'));");
    client.println("    document.getElementById('btn' + data.mode).classList.add('selected');");
    client.println("    document.getElementById('ledToggleButton').className = data.ledOutputEnabled ? 'enabled' : 'disabled';");
    client.println("    document.getElementById('ledToggleButton').innerText = data.ledOutputEnabled ? 'Disable LED Output' : 'Enable LED Output';");
    client.println("    document.getElementById('randomModeButton').className = data.randomModeEnabled ? 'enabled' : 'disabled';");
    client.println("    document.getElementById('randomModeButton').innerText = data.randomModeEnabled ? 'Disable Random Mode' : 'Enable Random Mode';");

    // Show or hide the countdown timer
    client.println("    if (randomModeEnabled) {");
    client.println("      document.getElementById('timeUntilNextModeChange').style.display = 'block';");
    client.println("    } else {");
    client.println("      document.getElementById('timeUntilNextModeChange').style.display = 'none';");
    client.println("      document.getElementById('timeLeft').innerText = '';");
    client.println("    }");
    client.println("  });");
    client.println("}");

    client.println("// Update status every 1 second");
    client.println("setInterval(updateStatus, 1000);");

    client.println("// Initial call to updateStatus");
    client.println("updateStatus();");

    client.println("// Update the countdown timer every second");
    client.println("setInterval(function() {");
    client.println("  if (randomModeEnabled) {");
    client.println("    var elapsed = (Date.now() - lastUpdateTime) / 1000;");
    client.println("    var displayTime = Math.max(0, timeRemaining - elapsed);");
    client.println("    document.getElementById('timeLeft').innerText = Math.floor(displayTime) + ' seconds';");
    client.println("  } else {");
    client.println("    document.getElementById('timeLeft').innerText = '';");
    client.println("  }");
    client.println("}, 1000);");

    client.println("function setMode(m) {");
    client.println("  fetch('/setMode?mode=' + m).then(() => updateStatus());");
    client.println("}");
    client.println("function setIntervalTime() {");
    client.println("  let interval = document.getElementById('intervalInput').value;");
    client.println("  fetch('/setInterval?interval=' + interval).then(() => updateStatus());");
    client.println("}");
    client.println("function setBrightness(b) {");
    client.println("  fetch('/setBrightness?brightness=' + b).then(() => updateStatus());");
    client.println("}");
    client.println("function adjustBrightness(change) {");
    client.println("  let slider = document.getElementById('brightnessSlider');");
    client.println("  slider.value = parseInt(slider.value) + change;");
    client.println("  setBrightness(slider.value);");
    client.println("}");
    client.println("function toggleLEDOutput() {");
    client.println("  fetch('/toggleLEDOutput').then(() => updateStatus());");
    client.println("}");
    client.println("function toggleRandomMode() {");
    client.println("  fetch('/toggleRandomMode').then(() => updateStatus());");
    client.println("}");
    client.println("</script></body></html>");
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

///////////////////////////////////////////////////