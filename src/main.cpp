#include <WiFi.h>
#include <ESPmDNS.h>
#include "connect-wifi.h" // Wi-Fi credentials
#include "ledControl.h"  // LED Control methods
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h" // Wi-Fi credentials

#define IDLE_TIMEOUT 30000  // 30 seconds idle time

WebServer server(80);


unsigned long lastMessageTime = 0;

// Handle the JSON message
void handleMessage()
{
    // Buffer to store the incoming JSON
    const size_t capacity = JSON_OBJECT_SIZE(11) + 300;
    StaticJsonDocument<capacity> doc;

    String jsonMessage = server.arg("plain");                       // Get the incoming JSON message
    DeserializationError error = deserializeJson(doc, jsonMessage); // Parse the JSON

    if (error)
    {
        // If there's an error in JSON deserialization, respond with an error message and return
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        server.send(400, "text/plain", "Invalid JSON format");
        return;
    }

    // Display PilotName and LapNumber if available
    if (doc.containsKey("PilotName") && doc.containsKey("LapNumber") && doc.containsKey("IsRaceEnd"))
    {
        const char *pilotName = doc["PilotName"];
        int lapNumber = doc["LapNumber"];
        bool raceEnd = doc["IsRaceEnd"];

        Serial.print("PilotName: ");
        Serial.println(pilotName);
        Serial.print("LapNumber: ");
        Serial.println(lapNumber);

        if (raceEnd) {
            Serial.println("Pilot has finished -> Wave the checkered flag");
            waveChequeredFlag(CRGB::White); // Use White as the highlight color
        }
    }
    else
    {
        Serial.println("PilotName or LapNumber is missing.");
    }

    // Check if the "State" field is present
    if (doc.containsKey("State"))
    {
        lastMessageTime = millis();
        const char *state = doc["State"]; // Extract the "State" field

        // Handle different states and set LED colors accordingly
        if (strcmp(state, "Arm") == 0)
        {
            Serial.println("State: Arm -> Setting LEDs to RED");
            setLEDs(CRGB(255, 0, 0)); // RED color
        }
        else if (strcmp(state, "Start") == 0)
        {
            Serial.println("State: Start -> Setting LEDs to BLACK - turn them off");
            setLEDs(CRGB(0, 0, 0)); // Black color
        }
        else if (strcmp(state, "Stop") == 0)
        {
            Serial.println("State: Stop -> Setting LEDs to Red");
            setLEDs(CRGB(255, 0, 0)); // Red color
        }
        else if (strcmp(state, "End") == 0)
        {
            Serial.println("State: End -> Waving the checkered flag");
            waveChequeredFlag(CRGB::White); // Use White as the highlight color
        }
        else if (strcmp(state, "Times Up") == 0)
        {
            Serial.println("State: Times Up -> Waving the checkered flag");
            waveChequeredFlag(CRGB::White); // Use White as the highlight color
        }
        else
        {
            Serial.println("Unknown State -> No LED action");
            server.send(400, "text/plain", "Unknown state value");
            return;
        }
    }
    else
    {
        // If "State" is not present, fall back to using RGB values from the JSON
        if (doc.containsKey("ChannelColorR") && doc.containsKey("ChannelColorG") && doc.containsKey("ChannelColorB"))
        {
            lastMessageTime = millis();
            int r = doc["ChannelColorR"];
            int g = doc["ChannelColorG"];
            int b = doc["ChannelColorB"];

            Serial.print("Setting LEDs to RGB values -> R: ");
            Serial.print(r);
            Serial.print(", G: ");
            Serial.print(g);
            Serial.print(", B: ");
            Serial.println(b);

            setLEDs(CRGB(r, g, b)); // Set the LEDs to the specified RGB values
        }
        else
        {
            Serial.println("No valid color information found in JSON");
            server.send(400, "text/plain", "No valid state or color information");
            return;
        }
    }

    // Send a response
    server.send(200, "text/plain", "LED state updated!");
}

void setup()
{
    Serial.begin(115200);
    setupLEDs();

    WiFi.setHostname(hostname);
    // Scan for known wifi Networks
    int networks = scanForWifi();
    if (networks > 0)
    {
        String wifiName = connectToWifi();
        String wifiMessage = "Connected to: " + wifiName;
    }
    else
    {
        Serial.println(F("no networks found. Reset to try again"));
        while (true)
            ; // no need to go further, hang in there, will auto launch the Soft WDT reset
    }

    // To reach here we must have a WiFI connection. Set the LEDs purple
    setLEDs(CRGB::Purple);

    // Initialize mDNS
    if (!MDNS.begin(hostname))
    { // Set the hostname
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
    Serial.println("mDNS responder started");

    server.on("/led", HTTP_POST, handleMessage);
    server.begin();
    Serial.println("Server started");
}

void loop()
{
    server.handleClient();

    // Check for idle condition (no message received in the last X seconds)
    if (millis() - lastMessageTime > IDLE_TIMEOUT) {

        // Turn the LEDs off when the timeout is reached to save power
        setLEDs(CRGB::Black);  // Turn off LEDs
    }
}
