#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "XIAO_ESP32C3_AP";
const char* password = "12345678";

#define TX_PIN 21  
#define RX_PIN 20  

uint8_t mode = 0x00;
uint8_t brightness = 3;
bool isSendingData = false; // Flag to prevent overlapping transmissions
AsyncWebServer server(80);

void setup() 
{
    Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    WiFi.softAP(ssid, password);
    Serial.begin(115200);
    Serial.println("Access point started.");

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) 
    {
        String html = "<html><body><h1>ESP32-C3 Mode Control</h1>";
        html += "<p>Current Mode: <span id='currentMode'>" + String(mode) + "</span></p>";
        html += "<p>Brightness Level: <span id='brightnessValue'>" + String(brightness) + "</span></p>";
        html += "<h3>Select Mode</h3>";

        // Mode buttons
        html += "<button id='btn0' onclick=\"setMode(0)\">Confetti Mode</button><br>";
        html += "<button id='btn1' onclick=\"setMode(1)\">Color Cascade Mode</button><br>";
        html += "<button id='btn2' onclick=\"setMode(2)\">One At A Time Confetti Mode</button><br>";
        html += "<button id='btn3' onclick=\"setMode(3)\">Twinkling Stars Mode</button><br>";
        html += "<button id='btn4' onclick=\"setMode(4)\">Ripple Mode</button><br>";
        html += "<button id='btn5' onclick=\"setMode(5)\">Sparkle Mode</button><br>";
        html += "<button id='btn6' onclick=\"setMode(6)\">Fire Effect Mode</button><br>";
        html += "<button id='btn7' onclick=\"setMode(7)\">Breathing Mode</button><br>";
        html += "<button id='btn8' onclick=\"setMode(8)\">Meteor Mode</button><br>";
        html += "<button id='btn9' onclick=\"setMode(9)\">Color Fade Mode</button><br>";
        html += "<button id='btn10' onclick=\"setMode(10)\">Startup Sequence Mode (TEST)</button><br>";
        html += "<button id='btn11' onclick=\"setMode(11)\">Row By Row Mode (TEST)</button><br>";

        // Brightness slider and adjustment buttons
        html += "<h3>Adjust Brightness</h3>";
        html += "<input type='range' min='0' max='31' value='" + String(brightness) + "' id='brightnessSlider' oninput='setBrightness(this.value)' />";
        
        // Container for up and down buttons
        html += "<div style='display: inline-block; text-align: center;'>";
        html += "<button onmousedown='startIncrement()' onmouseup='stopAdjustment()' onmouseleave='stopAdjustment()'>&#9650;</button>"; // Up arrow ▲
        html += "<button onmousedown='startDecrement()' onmouseup='stopAdjustment()' onmouseleave='stopAdjustment()'>&#9660;</button>"; // Down arrow ▼
        html += "</div>";

        // JavaScript functions for mode and brightness adjustments
        html += "<script>";
        html += "let adjustmentInterval;"; // Interval ID for continuous adjustment

        html += "function setMode(m) {";
        html += "  fetch('/setMode?mode=' + m).then(response => response.text()).then(text => {";
        html += "    document.getElementById('currentMode').innerText = text;";
        html += "    highlightModeButton(m);";
        html += "  });";
        html += "}";

        html += "function setBrightness(b) {";
        html += "  fetch('/setBrightness?brightness=' + b).then(response => response.text()).then(text => {";
        html += "    document.getElementById('brightnessValue').innerText = text;";
        html += "  });";
        html += "}";

        html += "function incrementBrightness() {";
        html += "  let currentBrightness = parseInt(document.getElementById('brightnessValue').innerText);";
        html += "  if (currentBrightness < 31) {";
        html += "    currentBrightness++;";
        html += "    setBrightness(currentBrightness);";
        html += "    document.getElementById('brightnessSlider').value = currentBrightness;";
        html += "  }";
        html += "}";

        html += "function decrementBrightness() {";
        html += "  let currentBrightness = parseInt(document.getElementById('brightnessValue').innerText);";
        html += "  if (currentBrightness > 0) {";
        html += "    currentBrightness--;";
        html += "    setBrightness(currentBrightness);";
        html += "    document.getElementById('brightnessSlider').value = currentBrightness;";
        html += "  }";
        html += "}";

        // Start continuous increment on hold
        html += "function startIncrement() {";
        html += "  incrementBrightness();";
        html += "  adjustmentInterval = setInterval(incrementBrightness, 200);"; // Adjust brightness every 200ms
        html += "}";

        // Start continuous decrement on hold
        html += "function startDecrement() {";
        html += "  decrementBrightness();";
        html += "  adjustmentInterval = setInterval(decrementBrightness, 200);"; // Adjust brightness every 200ms
        html += "}";

        // Stop adjustment
        html += "function stopAdjustment() {";
        html += "  clearInterval(adjustmentInterval);";
        html += "}";

        // Highlight the active mode button
        html += "function highlightModeButton(m) {";
        html += "  for (let i = 0; i <= 11; i++) {";
        html += "    if (document.getElementById('btn' + i)) {";
        html += "      document.getElementById('btn' + i).style.backgroundColor = (i === m) ? 'lightgreen' : '';";
        html += "    }";
        html += "  }";
        html += "}";

        // Initial highlight for current mode on page load
        html += "highlightModeButton(" + String(mode) + ");";
        html += "</script>";

        html += "</body></html>";
        request->send(200, "text/html", html);
    });

    server.on("/setMode", HTTP_GET, [](AsyncWebServerRequest *request) 
    {
        if (request->hasParam("mode")) 
        {
            mode = request->getParam("mode")->value().toInt();
            sendUARTData(); // Ensure data is sent safely
            request->send(200, "text/plain", String(mode));
        } 
        else 
        {
            request->send(400, "text/plain", "Missing mode parameter");
        }
    });

    server.on("/setBrightness", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        if (request->hasParam("brightness")) 
        {
            brightness = request->getParam("brightness")->value().toInt();
            sendUARTData(); // Ensure data is sent safely
            request->send(200, "text/plain", String(brightness));
        } 
        else 
        {
            request->send(400, "text/plain", "Missing brightness parameter");
        }
    });

    server.begin();
    Serial.println("Web server started.");
}

void loop() 
{
    delay(1000);
}

// Send data over UART with safety checks
void sendUARTData() 
{
    if (isSendingData) return; // Exit if a transmission is already in progress

    isSendingData = true; // Set flag to indicate transmission is in progress

    Serial1.write(mode);         // Send mode
    delay(10);                   // Small delay to ensure data stability
    Serial1.write(brightness);   // Send brightness
    Serial1.write(0x00);
    Serial1.write(0x00);

    delay(50);                   // Wait for data transmission to complete
    isSendingData = false;       // Reset flag after transmission
}
