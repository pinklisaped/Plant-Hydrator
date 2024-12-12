#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include "wizard.h"
#include "interface.h"
#include "auth.h"

// Consts
const uint8_t SIGNAL_PIN = D5;
const uint8_t RESET_PIN = D6;
const uint32_t START_DELAY_MS = 5000;
const uint32_t SLEEP_DELAY_MS = 60000;

AsyncWebServer server(80);
bool isAuthenticated = false;

void handleReset() {
  unsigned long pressStart = millis();

  // Wait press RESET_PIN
  while (digitalRead(RESET_PIN) == HIGH) {
    if (millis() - pressStart > 10000) {  // 10 sec
      Serial.println("Long press detected. Resetting configuration...");

      // Delete configs
      LittleFS.remove("/wifi.cfg");
      LittleFS.remove("/password.cfg");
      LittleFS.remove("/config.cfg");

      // Reboot ESP
      ESP.restart();
    }
  }
  Serial.println("Short press detected. Reset skipped.");
}

void setup() {
  Serial.begin(115200); // COM initialization
  pinMode(SIGNAL_PIN, OUTPUT);
  // pinMode(RESET_PIN, INPUT_PULLUP);

  // LittleFS init
  if (!LittleFS.begin()) {
    Serial.println("Error mounting LittleFS");
    return;
  }
  Serial.println("Littlefs initialized.");

  // Run wizard
  if (!loadWiFiConfig()) {
    Serial.println("Load wizard.");
    startWizard(server);
  } else {
    Serial.println("Load interface.");
    startInterface(server);
  }
}

void loop() {
  Serial.println("Wizard ticked.");
  tickWizard();
  delay(1000);
  // handleReset();
}
