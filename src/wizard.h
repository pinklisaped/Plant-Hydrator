#ifndef WIZARD_H
#define WIZARD_H

#include <Arduino.h>
#include <ESPAsyncWebServer.h>

void startWizard(AsyncWebServer &server);
void tickWizard();

#endif  // WIZARD_H
