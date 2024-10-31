#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Define Wi-Fi settings
const char* ssid = "XIAO_ESP32C3_AP";
const char* password = "12345678"; // Minimum 8 characters for WPA2 encryption

// Define UART settings
#define TX_PIN 21  // Transmit pin for UART on XIAO ESP32-C3
#define RX_PIN 20  // Receive pin for UART on XIAO ESP32-C3

uint8_t mode = 0x00; // Current mode
AsyncWebServer server(80); // Web server on port 80

void setup() {
  // Initialize Serial1 for UART communication at 9600 baud
  Serial1.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  // Start Wi-Fi as an access point
  WiFi.softAP(ssid, password);
  Serial.begin(115200);
  Serial.println("Access point started.");

  // Define the root webpage
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><body><h1>ESP32-C3 Mode Control</h1>";
    html += "<p>Current Mode: " + String(mode) + "</p>";
    html += "<p>Wi-Fi Status: Connected</p>";
    html += "<h3>Select Mode</h3>";

    // Buttons for each mode with corresponding names
    html += "<button onclick=\"setMode(0)\">Confetti Mode</button><br>";
    html += "<button onclick=\"setMode(1)\">Color Fade Mode</button><br>";
    html += "<button onclick=\"setMode(2)\">One At A Time Confetti Mode</button><br>";
    html += "<button onclick=\"setMode(3)\">Twinkling Stars Mode</button><br>";
    html += "<button onclick=\"setMode(4)\">Ripple Mode</button><br>";
    html += "<button onclick=\"setMode(5)\">Sparkle Mode</button><br>";
    html += "<button onclick=\"setMode(6)\">Rainbow Chase Mode</button><br>";
    html += "<button onclick=\"setMode(7)\">Breathing Mode</button><br>";
    html += "<button onclick=\"setMode(8)\">Meteor Mode</button><br>";

    // JavaScript function to handle mode changes
    html += "<script>";
    html += "function setMode(m) {";
    html += "  fetch('/setMode?mode=' + m).then(response => {";
    html += "    if (response.ok) return response.text();";
    html += "  }).then(text => {";
    html += "    alert('Mode set to ' + text);";
    html += "    location.reload();";
    html += "  });";
    html += "}";
    html += "</script>";
    html += "</body></html>";
    request->send(200, "text/html", html);
  });

  // Endpoint to set the mode
  server.on("/setMode", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("mode")) {
      mode = request->getParam("mode")->value().toInt();
      Serial1.write(mode); // Send mode over UART
      request->send(200, "text/plain", String(mode)); // Send response with the set mode
    } else {
      request->send(400, "text/plain", "Missing mode parameter");
    }
  });

  // Start server
  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  // Transmit the mode periodically for any connected microcontroller
  Serial1.write(mode);
  delay(1000); 
}
