// display.cpp
#include <Arduino.h>
#include "display.h"
#include "lgfx_config.h"

static LGFX lcd;

static const int DISPLAY_ROTATION = 1;      // 0 = portrait 320x480; 1 or 3 = landscape 480x320
static const int AWAKE_BRIGHTNESS = 255;    // 0-255
static const int ASLEEP_BRIGHTNESS = 0;     // set to e.g. 15 for "dim" instead of off

void displayBegin() {
    lcd.init();
    lcd.setRotation(DISPLAY_ROTATION);
    lcd.setBrightness(AWAKE_BRIGHTNESS);
    // Bring-up check: should flash red, green, blue.
    // Delete when wiring is correct.
    lcd.fillScreen(TFT_RED); delay(400);
    lcd.fillScreen(TFT_GREEN); delay(400);
    lcd.fillScreen(TFT_BLUE); delay(400);
    lcd.fillScreen(TFT_BLACK);
}

void displaySetOn(bool on) {
    lcd.setBrightness(on ? AWAKE_BRIGHTNESS : ASLEEP_BRIGHTNESS);
}

bool displayShowJpeg(const uint8_t* data, size_t len) {
    return lcd.drawJpg(data, len, 0, 0);
}