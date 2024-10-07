# DRW-StartLEDs
 LED code that receives JSON messages from FPVTrackside

 ## Hardware required
 The Code is written for Platformio to be deployed to an ESP32 connected to a strip of WS1812 LEDs on PIN16.

## Updating
The main code is in the /src folder. 
Update the config.h with your hostname.
Update the array in connect-wifi.cpp with your WiFi credentials

## Example messages
The /test folder contains an output.txt file with JSON data that was generated from FPVTrackside and captured by the ESP32, then used to handle the conditions for updating the LED strip

## About the Author
Chris Riddell AKA Redalin is a drone racer and is constanty trying to make the Wellington races more fun by adding tech overhead to our race directors. With just enough knowledge of Code and ChatGPT he's able to create these things.