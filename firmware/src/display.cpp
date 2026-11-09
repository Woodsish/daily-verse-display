// display.cpp
#include <Arduino.h>
#include "display.h"

static const int BL_PIN = D6; // CHANGE to the pin my TFT backlight is connected to

void displayBegin() { pinMode(BL_PIN, OUTPUT); digitalWrite(BL_PIN, HIGH); }
void displaySetOn(bool on) { digitalWrite(BL_PIN, on ? HIGH : LOW); }