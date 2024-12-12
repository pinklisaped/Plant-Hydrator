#include "interface.h"
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>

void startInterface(AsyncWebServer &server) {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/interface.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/style.css", "text/css");
  });

  server.on("/save", HTTP_POST, [](AsyncWebServerRequest *request) {
    String interval = request->getParam("interval", true)->value();
    String duration = request->getParam("duration", true)->value();
    saveUserConfig(interval, duration);
    request->send(200, "text/plain", "Saved!");
  });

  server.begin();
}

void saveUserConfig(const String &interval, const String &duration) {
  File configFile = LittleFS.open("/conf/config.cfg", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return;
  }

  configFile.println("interval=" + interval);
  configFile.println("duration=" + duration);
  configFile.close();
  Serial.println("Configuration saved");
}
