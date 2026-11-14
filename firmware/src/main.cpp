#include <Arduino.h>
#include <WiFi.h>
#include "settings.h"
#include "wifi_setup.h"
#include "clock.h"
#include "display.h"
#include "ota.h"
#include "content.h"

static bool lastAwake = true;
static bool haveImage = false;
static uint32_t lastRefresh = 0;
static const uint32_t REFRESH_MS = 60UL * 60 * 1000; // hourly once there is an image
static const uint32_t RETRY_MS = 5UL * 60 * 1000; //every 5 min until the first one

static void refreshAll() {
  if (WiFi.status() != WL_CONNECTED) return;
  otaCheckAndUpdate();
  if (contentRefresh()) haveImage = true;
  lastRefresh = millis();
}

void setup() {
  Serial.begin(115200);
  settingsBegin();
  displayBegin();

  pinMode(0, INPUT_PULLUP); // BOOT button
  displayShowStatus("Connecting to WiFi", "Starting up...");
  wifiSetup(digitalRead(0) == LOW); //held at power-up = reset WiFi settings

  displayShowStatus("Connected", WiFi.SSID().c_str());
  Serial.println(clockBegin() ? "Time synced" : "Time sync failed:");
  
  displayShowStatus("Loading today's verse", "");
  refreshAll();
  if (!haveImage) displayShowStatus("Couldn't load verse", "Will retry in a few minutes");

  lastAwake = isAwakeNow();
  displaySetOn(lastAwake);
}

void loop() {
  bool awake = isAwakeNow();
  uint32_t interval = haveImage ? REFRESH_MS : RETRY_MS;

  if (awake != lastAwake) {
    lastAwake = awake;
    if (awake) refreshAll();  // new day's image ready when it wakes
    displaySetOn(awake);
  } else if (awake && millis() - lastRefresh > interval) {
    refreshAll();
  }
  delay(1000);
}