// Include the Wifi Headers
#include "connect-wifi.h"

// Array of known Wifi network credentials
const char* KNOWN_SSID[] = {"DRW", "dummy.ssid", "dummySSID"};
const char* KNOWN_PASSWORD[] = {"wellington", "dummypass", "dummySSIDpass"};
const int   KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks

// If we are hosting our own Wifi these are the credentials
const char* AP_SSID = "DRW"; 
const char* AP_PASSWORD = "wellington";

int visibleNetworks = 0;

int scanForWifi() {
  Serial.println(F("WiFi scan start"));
  visibleNetworks = WiFi.scanNetworks();
  Serial.println(F("WiFi scan done"));
  return visibleNetworks;
}

String connectToWifi() {
  int i, n;
  bool wifiFound = false;
  Serial.println(F("Found the following networks:"));
  for (i = 0; i < visibleNetworks; ++i) {
    Serial.println(WiFi.SSID(i));
  }
  // ----------------------------------------------------------------
  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks
  // ----------------------------------------------------------------
  for (i = 0; i < visibleNetworks; ++i) {
    Serial.print(F("Checking: "));
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++) { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str())) {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      } else { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound) break; // break from the "for each visible network" loop
  } // end for each visible network
  if (wifiFound) {
    Serial.print(F("\nConnecting to "));
    Serial.println(KNOWN_SSID[n]);
    WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
    return KNOWN_SSID[n];
  } else {
    // We don't have WiFi, lets create our own
    WiFi.softAP(AP_SSID, AP_PASSWORD);

    IPAddress IP = WiFi.softAPIP();
    Serial.print("Creating local Wifi. IP address: ");
    Serial.println(IP);

    // Print ESP32 Local IP Address
    Serial.println(WiFi.localIP());
    return AP_SSID;
  }
}