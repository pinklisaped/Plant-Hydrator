#include "auth.h"
#include <LittleFS.h>

bool checkAuth() {
  if (LittleFS.exists("/conf/password.cfg")) {
    File file = LittleFS.open("/conf/password.cfg", "r");
    // Логика проверки пароля
    file.close();
    return true;
  }
  return false;
}

bool loadWiFiConfig() {
  if (LittleFS.exists("/conf/wifi.cfg")) {
    File file = LittleFS.open("/conf/wifi.cfg", "r");
    String ssid = file.readStringUntil('\n');
    String pass = file.readStringUntil('\n');
    file.close();
    return connectWiFi(ssid, pass);
  }
  return false;
}

bool connectWiFi(const String &ssid, const String &password) {
  WiFi.begin(ssid.c_str(), password.c_str());
  for (int i = 0; i < 20; ++i) {
    if (WiFi.status() == WL_CONNECTED) return true;
    delay(500);
  }
  return false;
}

void saveWiFiConfig(const String &ssid, const String &password) {
  File file = LittleFS.open("/conf/wifi.cfg", "w");
  file.println(ssid);
  file.println(password);
  file.close();
}
