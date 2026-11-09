#include <Arduino.h>
#include <WiFi.h>
#include "settings.h"
#include "wifi_setup.h"
#include "clock.h"
#include "display.h"
#include "ota.h"
#include "content.h"

static bool lastAwake = true;
static uint32_t lastRefresh = 0;
static const uint32_t REFRESH_MS = 60UL * 60 * 1000; // hourly

static void refreshAll() {
  if (WiFi.status() != WL_CONNECTED) return;
  otaCheckAndUpdate();
  contentRefresh();
  lastRefresh = millis();
}

void setup() {
  Serial.begin(115200);
  settingsBegin();
  displayBegin();

  pinMode(0, INPUT_PULLUP); // BOOT button
  wifiSetup(digitalRead(0) == LOW); //held at power-up = reset WiFi settings

  Serial.println(clockBegin() ? "Time synced" : "Time sync failed:");
  refreshAll();
  lastAwake = isAwakeNow();
  displaySetOn(lastAwake);
}

void loop() {
  bool awake = isAwakeNow();
  if (awake != lastAwake) {
    lastAwake = awake;
    if (awake) refreshAll();  // new day's image ready when it wakes
    displaySetOn(awake);
  } else if (awake && millis() - lastRefresh > REFRESH_MS) {
    refreshAll();
  }
  delay(1000);
}