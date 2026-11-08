#include <Arduino.h>
#include <WiFiManager.h>
#include <Preferences.h>
#include <time.h>

Preferences prefs;
WiFiManager wm;
WiFiManagerParameter pTz("tz", "Timezone (POSIX)", "PST8PDT,M3.2.0,M11.1.0", 40);
WiFiManagerParameter pWake("wake", "Wake (HH:MM)", "07:00", 6);
WiFiManagerParameter pSleep("sleep", "Sleep (HH:MM)", "22:00", 6);

void saveSettings() {
  Serial.println("Saving settings to Preferences...");
  prefs.putString("tz", pTz.getValue());
  prefs.putString("wake", pWake.getValue());
  prefs.putString("sleep", pSleep.getValue());
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  prefs.begin("verse", false);

  // Set Wi-Fi mode to Station explicitly to avoid connection issues
  WiFi.mode(WIFI_STA);

  // Pin 0 (Boot button on the XIAO) triggers a factory reset if held during boot.
  pinMode(0, INPUT_PULLUP);
  if (digitalRead(0) == LOW) {
    Serial.println("Resetting WiFiManager settings...");
    wm.resetSettings();
    prefs.clear(); // Clear custom settings on factory reset
  }

  // Load previously saved settings into web inputs so they don't reset to defaults
  String savedTz = prefs.getString("tz", "PST8PDT,M3.2.0,M11.1.0");
  String savedWake = prefs.getString("wake", "07:00");
  String savedSleep = prefs.getString("sleep", "22:00");
  pTz.setValue(savedTz.c_str(), 40);
  pWake.setValue(savedWake.c_str(), 6);
  pSleep.setValue(savedSleep.c_str(), 6);

  // Set up WiFiManager parameters
  wm.addParameter(&pTz); wm.addParameter(&pWake); wm.addParameter(&pSleep);
  wm.setSaveParamsCallback(saveSettings);
  wm.setConfigPortalTimeout(180);

  // Generate a unique AP name based on the MAC Address
  String name = "VerseDisplay-" + WiFi.macAddress().substring(12);
  name.replace(":", "");

  // Start WiFi Autoconnect and Config Portal
  Serial.println("Attempting to connect to Wi-Fi...");
  if (!wm.autoConnect(name.c_str())) {
    Serial.println("Failed to connect or portal timeout. Restarting ESP32...");
    delay(3000);
    ESP.restart(); // This is the ONLY restart you need in setup()
  }

  // Successful Connection
  Serial.println("Wi-Fi connected successfully!");

  // Configure Timezone
  configTzTime(prefs.getString("tz", "PST8PDT,M3.2.0,M11.1.0").c_str(),
                "pool.ntp.org", "time.nist.gov");

  // Sync and display the time
  struct tm t;
  if (getLocalTime(&t, 10000)) {
    Serial.println(&t, "%F %T");
  } else {
    Serial.println("Failed to obtain time");
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
