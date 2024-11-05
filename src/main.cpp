#include "connect-wifi.h" // Wi-Fi credentials
#include "ledControl.h"  // LED Control methods
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"

#define IDLE_TIMEOUT 30000  // 30 seconds idle time

WebServer server(80);


unsigned long lastMessageTime = 0;

// Handle the JSON message
void handleMessage()
{

    // Buffer to store the incoming JSON
    JsonDocument doc;

    String jsonMessage = server.arg("plain");                       // Get the incoming JSON message
    DeserializationError error = deserializeJson(doc, jsonMessage); // Parse the JSON

    // Log the values (Optional)
    Serial.println("Received JSON Data:");
    Serial.println(jsonMessage);

    bool raceEnd = false;
    const char * pilotName = "";
    int lapNumber = 0;

    if (error)
    {
        // If there's an error in JSON deserialization, respond with an error message and return
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        server.send(400, "text/plain", "Invalid JSON format");
        return;
    }

    // Check if the "State" field is present
    if (doc["State"].is<const char *>())
    {
        const char * state = doc["State"]; // Extract the "State" field
        
        // We have just received a valid message so reset the last message timer.
        lastMessageTime = millis();

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
            waveChequeredFlag(CRGB::Red); // Emergency Stop Flash Red
        }
        else if (strcmp(state, "Cancel") == 0)
        {
            Serial.println("State: Cancel -> Flashing LEDs to Red");
            waveChequeredFlag(CRGB::Red); // Emergency Stop Flash Red
        }
        else if (strcmp(state, "End") == 0)
        {
            Serial.println("State: End -> Waving the checkered flag");
            waveChequeredFlag(CRGB::White); // Use White as the highlight color
        }
        else if (strcmp(state, "Times Up") == 0)
        {
            Serial.println("State: Times Up -> Finish your lap and land");
            setLEDs(CRGB::Orange); // Set LEDs to orange light
        }
        else
        {
            Serial.println("Unknown State -> No LED action");
            server.send(400, "text/plain", "Unknown state value");
        }
        // Send a response
        server.send(200, "text/plain", "LED Race state updated!");
    }
    else if (doc["PilotName"].is<const char *>() && doc["LapNumber"].is<int>() && doc["Position"].is<int>() && doc["IsRaceEnd"].is<bool>() && doc["ChannelColorR"].is<int>() && doc["ChannelColorG"].is<int>() && doc["ChannelColorB"].is<int>())
    // No state, so we must have Pilot info. Therefore Display PilotName and LapNumber if available
    {
        // We have just received a valid message so reset the last message timer.
        lastMessageTime = millis();

        pilotName = doc["PilotName"];
        lapNumber = doc["LapNumber"];
        raceEnd = doc["IsRaceEnd"];
        int position = doc["Position"];

        Serial.print("PilotName: ");
        Serial.println(pilotName);
        Serial.print("LapNumber: ");
        Serial.println(lapNumber);

        int r = doc["ChannelColorR"];
        int g = doc["ChannelColorG"];
        int b = doc["ChannelColorB"];

        Serial.print("Received LED RGB values -> R: ");
        Serial.print(r);
        Serial.print(", G: ");
        Serial.print(g);
        Serial.print(", B: ");
        Serial.println(b);

        if (raceEnd && (position == 1)) {
            Serial.println("Pilot has finished first -> Wave the checkered flag");
            waveChequeredFlag(CRGB(r, g, b)); // Use Pilot colour as the highlight color
        } else {

            setLEDs(CRGB(r, g, b)); // Set the LEDs to the specified Pilot RGB values
        }
        // Send a response
        server.send(200, "text/plain", "LED Colour state updated!");
    }
    else
    {
        Serial.println("PilotName or LapNumber or colour information is missing.");
        server.send(400, "text/plain", "No valid race state or color information");  
        return;
    }
}

void setup()
{
    Serial.begin(115200);

    // Initialise the LEDs in the ledControl file
    initLEDs();

    // initialise Wifi as per the connect-wifi file
    initWifi();

    // To reach here we must have a WiFI connection. Set the LEDs purple
    setLEDs(CRGB::Purple);

    initMDNS();

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
