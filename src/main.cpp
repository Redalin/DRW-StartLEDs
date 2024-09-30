#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "config.h"  // Include the Wi-Fi credentials

// Constants
#define LED_PIN 16       // Pin where the WS2812 LED strip is connected
#define NUM_LEDS 12      // Number of LEDs in the strip
#define BRIGHTNESS 128   // LED brightness
#define IDLE_TIMEOUT 60000  // 60 seconds idle time

CRGB leds[NUM_LEDS];      // Create an array to hold the LED colors
WebServer server(80);     // Create a web server on port 80

unsigned long lastMessageTime = 0;  // Time of the last received message

// Set all LEDs to the specified color
void setLEDs(CRGB color) {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = color;
  }
  FastLED.show();  // Update the LED colors
}

// Generate a fire-like effect
void fireEffect() {
  for (int i = 0; i < NUM_LEDS; i++) {
    // Random flicker effect with red, orange, and yellow tones
    int flicker = random(100);
    int r = 255;
    int g = random(50, 150);  // Orange to yellow range
    int b = 0;
    leds[i] = CRGB(r - flicker, g - flicker, b);
  }
  FastLED.show();
  delay(100);  // Adjust for smoother effect
}

void handleMessage() {
  // Buffer to store the incoming JSON
  const size_t capacity = JSON_OBJECT_SIZE(11) + 300;
  StaticJsonDocument<capacity> doc;

  String jsonMessage = server.arg("plain");  // Get the incoming JSON message
  DeserializationError error = deserializeJson(doc, jsonMessage);  // Parse the JSON

  Serial.println(jsonMessage);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    server.send(400, "text/plain", "Invalid JSON format");
    return;
  }

  // Display PilotName and LapNumber if available
  if (doc.containsKey("PilotName") && doc.containsKey("LapNumber") && doc.containsKey("Position")) {
    // Reset the idle timer
    lastMessageTime = millis();

    const char* pilotName = doc["PilotName"];
    int lapNumber = doc["LapNumber"];
    int position = doc["Position"];
    
    Serial.print("PilotName: ");
    Serial.println(pilotName);
    Serial.print("LapNumber: ");
    Serial.println(lapNumber);
    Serial.print("Position: ");
    Serial.println(position);
  } else {
    Serial.println("PilotName LapNumber or position is missing.");
  }

  // Check if the "State" field is present
  if (doc.containsKey("State")) {
    const char* state = doc["State"];  // Extract the "State" field
    // Reset the idle timer
    lastMessageTime = millis();
    
    if (strcmp(state, "Arm") == 0) {
      Serial.println("State: Arm -> Setting LEDs to Orange");
      setLEDs(CRGB(255, 165, 0));  // Orange color
    } else if (strcmp(state, "Start") == 0) {
      Serial.println("State: Start -> Setting LEDs to Green");
      setLEDs(CRGB(0, 255, 0));    // Green color
    } else if (strcmp(state, "Stop") == 0) {
      Serial.println("State: Stop -> Setting LEDs to Red");
      setLEDs(CRGB(255, 0, 0));    // Red color
    } else if (strcmp(state, "End") == 0) {
      Serial.println("State: End -> Setting LEDs to Red");
      setLEDs(CRGB(255, 0, 0));    // Red color
      } else if (strcmp(state, "Cancel") == 0) {
      Serial.println("State: Cancel -> Setting LEDs to Red");
      setLEDs(CRGB(255, 0, 0));    // Red color
    } else {
      Serial.println("Unknown State -> No LED action");
      server.send(400, "text/plain", "Unknown state value");
      return;
    }
  } else {
    // If "State" is not present, fall back to using RGB values from the JSON
    if (doc.containsKey("ChannelColorR") && doc.containsKey("ChannelColorG") && doc.containsKey("ChannelColorB")) {
      // Reset the idle timer
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
      
      setLEDs(CRGB(r, g, b));  // Set the LEDs to the specified RGB values
    } else {
      Serial.println("No valid color information found in JSON");
      server.send(400, "text/plain", "No valid state or color information");
      return;
    }
  }

  // Send a response
  server.send(200, "text/plain", "LED state updated!");
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the FastLED library
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Print the IP address
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Define the route for handling the incoming message
  server.on("/led", HTTP_POST, handleMessage);
  
  // Start the web server
  server.begin();
  Serial.println("Server started. Waiting for messages...");

  // Initialize last message time
  lastMessageTime = millis();
}

void loop() {
  server.handleClient();  // Handle incoming requests

  // Check for idle condition (no message received in the last 60 seconds)
  if (millis() - lastMessageTime > IDLE_TIMEOUT) {
    fireEffect();  // Trigger the fire effect
  }
}
