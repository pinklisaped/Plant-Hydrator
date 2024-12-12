#ifndef INTERFACE_H
#define INTERFACE_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void startInterface(AsyncWebServer &server);
void saveUserConfig(const String &interval, const String &duration);

#endif  // INTERFACE_H
