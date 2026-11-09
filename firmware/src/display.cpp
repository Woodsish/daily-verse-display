// display.cpp
#include <Arduino.h>
#include "display.h"
#include "lgfx_config.h"

static LGFX lcd;

static const int DISPLAY_ROTATION = 1;      // 0 = portrait 320x480; 1 or 3 = landscape 480x320
static const int AWAKE_BRIGHTNESS = 255;    // 0-255
static const int ASLEEP_BRIGHTNESS = 0;     // set to e.g. 15 for "dim" instead of off

static void centered(const char* s, int cx, int y, bool large) {
    if (large) lcd.setFont(&lgfx::fonts::Font4);
    else       lcd.setFont(&lgfx::fonts::Font2);
    lcd.drawString(s, cx - lcd.textWidth(s) / 2, y);
}

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

void displayShowStatus(const char* line1, const char* line2) {
    lcd.setBrightness(AWAKE_BRIGHTNESS);
    lcd.fillScreen(TFT_BLACK);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    int cx = lcd.width() / 2;
    centered(line1, cx, lcd.height() / 2 -24, true);
    centered(line2, cx, lcd.height() / 2 + 16, false);
}

void displayShowSetup(const char* apName) {
    lcd.setBrightness(AWAKE_BRIGHTNESS);
    lcd.fillScreen(TFT_BLACK);
    lcd.setTextColor(TFT_WHITE, TFT_BLACK);

    bool wide = lcd.width() > lcd.height();
    int qr = wide ? 261 : 232; // multiples of 29 (QR version 3 grid)
    int qx = wide ? 20 : (lcd.width() - qr) / 2;
    int qy = wide ? (lcd.height() - qr) / 2 : 60;
    int cx = wide ? qx + qr + (lcd.width() - qx - qr) / 2 : lcd.width() / 2;
    int y  = wide ? 70 : qy + qr + 20;
    
    // White box behind the code so it scans on a black screen
    lcd.fillRect(qx - 8, qy - 8, qr + 16, qr + 16, TFT_WHITE);
    String wifi = String("WIFI:S:") + apName + ";T:nopass;;";
    lcd.qrcode(wifi.c_str(), qx, qy, qr, 3);

    lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    if (!wide) centered("Set up WiFi", cx, 16, true);
    centered("Scan this code,", cx, y, false);  y += 20;
    centered("or join WiFi:", cx, y, false);    y += 22;
    centered(apName, cx, y, !wide);             y += wide ? 28 : 38;
    centered("Then choose your", cx, y, false); y += 20;
    centered("home WiFi on the", cx, y, false); y += 20;
    centered("page that opens", cx, y, false);
}