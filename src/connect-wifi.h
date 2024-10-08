// connect-wifi.h
#ifndef CONNECT_WIFI_H
#define CONNECT_WIFI_H

// Wifi Part into another file
#include <WiFi.h>
#include <WiFiClientSecure.h>

void initMDNS();
void initWifi();
int scanForWifi();
String connectToWifi();

#endif  // CONNECT_WIFI_H