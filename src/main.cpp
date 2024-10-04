#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include "connect-wifi.h" // Wi-Fi credentials
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h" // Wi-Fi credentials

#define LED_PIN 16
#define NUM_LEDS 30
#define BRIGHTNESS 200
#define IDLE_TIMEOUT 30000  // 30 seconds idle time
#define WAVE_DURATION 10000 // wage checkered flag 10 seconds

CRGB leds[NUM_LEDS];

WebServer server(80);

unsigned long lastMessageTime = 0;

// Set all LEDs to the specified color
void setLEDs(CRGB color)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = color;
    }
    FastLED.show();
}

void waveChequeredFlag(CRGB color) {
    int wavePosition = 0;

    unsigned long startTime = millis(); // Get the starting time

    while (millis() - startTime < WAVE_DURATION)
    {
        // Loop through the LEDs and set the chequered pattern with wave effect
        for (int i = 0; i < NUM_LEDS; i++)
        {
            if ((i + wavePosition) % 2 == 0) {
                leds[i] = color;
            } else {
                leds[i] = CRGB::Black;
            }
        }
        FastLED.show();
        delay(200);

        // Increment wavePosition to shift the wave across the strip
        wavePosition++;
        if (wavePosition >= NUM_LEDS) {
            wavePosition = 0;  // Reset wavePosition when it completes a full cycle
        }
    }
}

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
    if (doc.containsKey("PilotName") && doc.containsKey("LapNumber"))
    {
        const char *pilotName = doc["PilotName"];
        int lapNumber = doc["LapNumber"];

        Serial.print("PilotName: ");
        Serial.println(pilotName);
        Serial.print("LapNumber: ");
        Serial.println(lapNumber);
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
            Serial.println("State: End -> Setting LEDs to Red");
            setLEDs(CRGB(255, 0, 0)); // Red color
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
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);

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

    // Check for idle condition (no message received in the last 60 seconds)
    if (millis() - lastMessageTime > IDLE_TIMEOUT) {
        setLEDs(CRGB::Black);  // Turn off LEDs
    }
}
