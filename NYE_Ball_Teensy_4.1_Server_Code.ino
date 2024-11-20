#include <QNEthernet.h>
using namespace qindesign::network;

#define TX_PIN 21  
#define RX_PIN 20  

uint8_t mode = 0x00;
uint8_t brightness = 3;
bool isSendingData = false; // Flag to prevent overlapping transmissions
EthernetServer server(80); // Port 80 for HTTP

void setup() 
{
    Serial1.begin(115200, SERIAL_8N1);  // Begin UART without custom pins on Teensy
    Serial.begin(115200);
    
    // Start Ethernet with a static IP
    byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
    IPAddress ip(192, 168, 1, 50);
    Ethernet.begin(mac, ip);

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
        Serial.println("Ethernet shield was not found.");
        while (true);
    }
    if (Ethernet.linkStatus() == LinkOFF) {
        Serial.println("Ethernet cable is not connected.");
    }

    server.begin();
    Serial.println("Ethernet server started at: http://192.168.1.50");
}

void loop() 
{
    EthernetClient client = server.available();
    if (client) {
        Serial.println("New client connected");

        // Read the HTTP request
        String request = client.readStringUntil('\r');
        Serial.print("Request: ");
        Serial.println(request);
        client.flush();

        // Handle requests to change mode and brightness
        if (request.indexOf("GET /setMode?mode=") >= 0) {
            int modeStart = request.indexOf("mode=") + 5;
            mode = request.substring(modeStart).toInt();
            sendUARTData();
            client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            client.print(String(mode));
            client.stop();
            return;
        }

        if (request.indexOf("GET /setBrightness?brightness=") >= 0) {
            int brightnessStart = request.indexOf("brightness=") + 11;
            brightness = request.substring(brightnessStart).toInt();
            sendUARTData();
            client.print("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n");
            client.print(String(brightness));
            client.stop();
            return;
        }

        // Serve the HTML page
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println("Connection: close");
        client.println();
        client.println("<!DOCTYPE HTML>");
        client.println("<html><head><style>");
        
        // CSS for selected button
        client.println(".selected { background-color: green; color: white; }");
        
        client.println("</style></head><body><h1>Teensy Mode Control</h1>");
        client.println("<p>Current Mode: <span id='currentMode'>" + String(mode) + "</span></p>");
        client.println("<p>Brightness Level: <span id='brightnessValue'>" + String(brightness) + "</span></p>");
        client.println("<h3>Select Mode</h3>");
        
        // Mode buttons with names
        client.println("<button id='btn0' onclick=\"setMode(0)\">Confetti Mode</button><br>");
        client.println("<button id='btn1' onclick=\"setMode(1)\">Color Cascade Mode</button><br>");
        client.println("<button id='btn2' onclick=\"setMode(2)\">One At A Time Confetti Mode</button><br>");
        client.println("<button id='btn3' onclick=\"setMode(3)\">Twinkling Stars Mode</button><br>");
        client.println("<button id='btn4' onclick=\"setMode(4)\">Ripple Mode</button><br>");
        client.println("<button id='btn5' onclick=\"setMode(5)\">Sparkle Mode</button><br>");
        client.println("<button id='btn6' onclick=\"setMode(6)\">Fire Effect Mode</button><br>");
        client.println("<button id='btn7' onclick=\"setMode(7)\">CheckerBoard Mode</button><br>");
        client.println("<button id='btn8' onclick=\"setMode(8)\">Meteor Mode</button><br>"); 
        client.println("<button id='btn9' onclick=\"setMode(9)\">Color Fade Mode</button><br>");
        client.println("<button id='btn10' onclick=\"setMode(10)\">FireWorks Mode</button><br>");
        client.println("<button id='btn11' onclick=\"setMode(11)\">Row By Row Mode</button><br>");
        client.println("<button id='btn12' onclick=\"setMode(12)\">American Flag</button><br>");
        client.println("<button id='btn13' onclick=\"setMode(13)\">SnowFall</button><br>");
        client.println("<button id='btn14' onclick=\"setMode(14)\">WaveEffect</button><br>");
        client.println("<button id='btn15' onclick=\"setMode(15)\">CountCircle</button><br>");
        client.println("<button id='btn16' onclick=\"setMode(16)\">HypnoticSpiral</button><br>");
        client.println("<button id='btn17' onclick=\"setMode(17)\">BouncingWave</button><br>");
        client.println("<button id='btn18' onclick=\"setMode(18)\">Aurora</button><br>");
        client.println("<button id='btn19' onclick=\"setMode(19)\">Test19</button><br>");
        client.println("<button id='btn20' onclick=\"setMode(20)\">Test20</button><br>");
        
        // Brightness slider and adjustment buttons
        client.println("<h3>Adjust Brightness</h3>");
        client.println("<button onclick='adjustBrightness(-1)'>&#8595;</button>"); // Down arrow
        client.println("<input type='range' min='0' max='31' value='" + String(brightness) + "' id='brightnessSlider' oninput='setBrightness(this.value)' />");
        client.println("<button onclick='adjustBrightness(1)'>&#8593;</button>");  // Up arrow

        // JavaScript for mode and brightness adjustments
        client.println("<script>");
        client.println("function setMode(m) {");
        client.println("  fetch('/setMode?mode=' + m)");
        client.println("    .then(response => response.text())");
        client.println("    .then(text => {");
        client.println("      document.getElementById('currentMode').innerText = text;");
        client.println("      highlightButton(m);");
        client.println("    }).catch(error => console.error('Error:', error));");
        client.println("}");

        client.println("function setBrightness(b) {");
        client.println("  fetch('/setBrightness?brightness=' + b)");
        client.println("    .then(response => response.text())");
        client.println("    .then(text => {");
        client.println("      document.getElementById('brightnessValue').innerText = text;");
        client.println("    }).catch(error => console.error('Error:', error));");
        client.println("}");

        client.println("function adjustBrightness(change) {");
        client.println("  let slider = document.getElementById('brightnessSlider');");
        client.println("  let newValue = parseInt(slider.value) + change;");
        client.println("  newValue = Math.max(0, Math.min(31, newValue));"); // Ensure within range
        client.println("  slider.value = newValue;");
        client.println("  setBrightness(newValue);");
        client.println("}");

        // JavaScript function to highlight selected mode button
        client.println("function highlightButton(mode) {");
        client.println("  for (let i = 0; i <= 20; i++) {");
        client.println("    document.getElementById('btn' + i).classList.remove('selected');");
        client.println("  }");
        client.println("  document.getElementById('btn' + mode).classList.add('selected');");
        client.println("}");
        
        client.println("</script>");
        client.println("</body></html>");
        delay(1);
        client.stop();
        Serial.println("Client disconnected");
    }
}

// Send data over UART with safety checks
void sendUARTData() 
{
    if (isSendingData) return;

    isSendingData = true;
    Serial1.write(0xFF);         // Start byte for synchronization
    Serial1.write(mode);         // Send mode
    Serial1.write(brightness);   // Send brightness
    Serial1.write(0x00);         // Send config or extra data if needed
    delay(50);
    isSendingData = false;
}
