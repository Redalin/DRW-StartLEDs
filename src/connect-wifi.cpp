// Include the Wifi Headers
#include "connect-wifi.h"
#include "config.h"
#include <ESPmDNS.h>

// The device hostname
const char* hostname = "drwleds";

// Array of known Wifi network credentials
const char *KNOWN_SSID[] = {"DRW",  "dummySSID"};
const char *KNOWN_PASSWORD[] = {"wellington", "dummySSIDpass"};
const int KNOWN_SSID_COUNT = sizeof(KNOWN_SSID) / sizeof(KNOWN_SSID[0]); // number of known networks

int visibleNetworks = 0;

void initMDNS() {
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
}

void initWifi()
{
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
}

int scanForWifi()
{
  Serial.println(F("WiFi scan start"));
  visibleNetworks = WiFi.scanNetworks();
  Serial.println(F("WiFi scan done"));
  return visibleNetworks;
}

String connectToWifi()
{
  int i, n;
  bool wifiFound = false;
  Serial.println(F("Found the following networks:"));
  for (i = 0; i < visibleNetworks; ++i)
  {
    Serial.println(WiFi.SSID(i));
  }
  // ----------------------------------------------------------------
  // check if we recognize one by comparing the visible networks
  // one by one with our list of known networks
  // ----------------------------------------------------------------
  for (i = 0; i < visibleNetworks; ++i)
  {
    Serial.print(F("Checking: "));
    Serial.println(WiFi.SSID(i)); // Print current SSID
    for (n = 0; n < KNOWN_SSID_COUNT; n++)
    { // walk through the list of known SSID and check for a match
      if (strcmp(KNOWN_SSID[n], WiFi.SSID(i).c_str()))
      {
        Serial.print(F("\tNot matching "));
        Serial.println(KNOWN_SSID[n]);
      }
      else
      { // we got a match
        wifiFound = true;
        break; // n is the network index we found
      }
    } // end for each known wifi SSID
    if (wifiFound)
      break; // break from the "for each visible network" loop
  } // end for each visible network
  if (wifiFound)
  {
    Serial.print(F("\nConnecting to "));
    Serial.println(KNOWN_SSID[n]);
    WiFi.begin(KNOWN_SSID[n], KNOWN_PASSWORD[n]);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP address:\t");
    Serial.println(WiFi.localIP());
    return KNOWN_SSID[n];
  }
}