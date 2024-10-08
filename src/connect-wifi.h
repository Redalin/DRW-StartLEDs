// connect-wifi.h
#ifndef CONNECT_WIFI_H
#define CONNECT_WIFI_H

// Removing the Wifi Part into another file
#include <WiFi.h>
#include <WiFiClientSecure.h>

int scanForWifi();
String connectToWifi();

#endif  // CONNECT_WIFI_H