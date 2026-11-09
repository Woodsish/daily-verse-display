#include <Arduino.h>
#include "settings.h"
#include "wifi_setup.h"
#include "clock.h"
#include "display.h"

static bool lastAwake = true;

void setup() {
  Serial.begin(115200);
  settingsBegin();
  displayBegin();

  pinMode(0, INPUT_PULLUP); // BOOT button
  wifiSetup(digitalRead(0) == LOW); //held at power-up = reset WiFi settings

  Serial.println(clockBegin() ? "Time synced" : "Time sync failed:");
  lastAwake = isAwakeNow();
  displaySetOn(lastAwake);
}

void loop() {
  bool awake = isAwakeNow();
  if (awake != lastAwake) {
    lastAwake = awake;
    displaySetOn(awake);
    Serial.println(awake ? "Display ON" : "Display OFF");
  }
  delay(1000);
}