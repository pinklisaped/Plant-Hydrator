#include "wizard.h"
#include "auth.h"
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <DNSServer.h>

DNSServer dnsServer;
volatile bool isWifiScanning = false;
bool isWizardStarted = false;

void startWizard(AsyncWebServer &server) {
  isWizardStarted = true;

  IPAddress APIP(172, 0, 0, 1);  // Gateway
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  dnsServer.start(53, "*", APIP);  // DNS spoofing (Only for HTTP)

  WiFi.softAP("Plant Hydrator CFG", "12345678");

  Serial.println("Setting up Wi-Fi Wizard...");
  WiFi.scanNetworks(true);

  server.onNotFound([](AsyncWebServerRequest *request) {
    request->redirect("/");
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/wizard.html", "text/html");
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/www/style.css", "text/css");
  });

  // Wi-Fi scan
  server.on("/start_scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (!isWifiScanning) {
      WiFi.scanNetworks(true);  // Асинхронное сканирование
      isWifiScanning = true;
    }
    request->send(200, "application/json", "{\"status\":\"started\"}");
  });

  // Get SSID list
  server.on("/scan_wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (isWifiScanning && WiFi.scanComplete() == WIFI_SCAN_RUNNING) {
      request->send(200, "application/json", "{\"status\":\"in_progress\"}");
    } else if (isWifiScanning && WiFi.scanComplete() >= 0) {
      isWifiScanning = false;
      int n = WiFi.scanComplete();
      String networks = "{\"status\":\"done\", \"networks\":[";
      for (int i = 0; i < n; i++) {
        networks += "{\"ssid\":\"" + WiFi.SSID(i) + "\",\"rssi\":" + String(WiFi.RSSI(i)) + "}";
        if (i < n - 1) networks += ",";
      }
      networks += "]}";
      request->send(200, "application/json", networks);
    } else {
      request->send(500, "application/json", "{\"status\":\"error\"}");
    }
  });

  server.on("/save_wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ssid = request->getParam("ssid", true)->value();
    String pass = request->getParam("password", true)->value();
    if (connectWiFi(ssid, pass)) {
      saveWiFiConfig(ssid, pass);
      request->send(200, "text/plain", "Saved! Rebooting...");
      ESP.restart();
    } else {
      request->send(500, "text/plain", "Failed to connect!");
    }
  });

  server.begin();
}

void tickWizard() {
  if (isWizardStarted) {
    dnsServer.processNextRequest();
    yield();
  }
}
