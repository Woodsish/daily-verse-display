// display.cpp
#include <Arduino.h>
#include "display.h"

static const int BL_PIN = D6; // CHANGE to the pin my TFT backlight is connected to

bool displayShowJpeg(const uint8_t* data, size_t len) {
    Serial.printf("Got JPEG: %u bytes\n", (unsigned)len);
    return true; // real decode/draw goes here once the TFT driver is set up
}

void displayBegin() { pinMode(BL_PIN, OUTPUT); digitalWrite(BL_PIN, HIGH); }
void displaySetOn(bool on) { digitalWrite(BL_PIN, on ? HIGH : LOW); }