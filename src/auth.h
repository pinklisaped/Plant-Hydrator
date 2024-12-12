#ifndef AUTH_H
#define AUTH_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

String loadPassword();
bool checkAuth(AsyncWebServerRequest *request);
bool connectWiFi(const String &ssid, const String &password);
void saveWiFiConfig(const String &ssid, const String &password);
bool loadWiFiConfig();

#endif  // AUTH_H
