// wifi_setup.cpp
#include <Arduino.h>
#include <WiFiManager.h>
#include "wifi_setup.h"
#include "settings.h"
#include "tz_table.h"
#include "display.h"

static WiFiManager wm;
static WiFiManagerParameter *pTz, *pWake, *pSleep;
static String tzHtml;   // HTML for the timezone dropdown

static String buildTzHtml(int selected) {
    String h = "<br><label for='tzsel'>Timezone</label><select name='tzsel' id='tzsel'>";
    for (int i = 0; i < TZ_COUNT; i++) {
        h += "<option value='";
        h += String(i);
        h += "'";
        if (i == selected) { h += " selected"; }
        h += ">";
        h.concat(TZ_TABLE[i].label);
        h += "</option>";
    }
    h += "</select>";
    return h;
}

static void onSave() {
    int tzi = wm.server->hasArg("tzsel") ? wm.server->arg("tzsel").toInt() : 0;
    settingsSave(tzi, pWake->getValue(), pSleep->getValue());
}

void wifiSetup(bool forceReset) {
    if (forceReset) wm.resetSettings();
    Settings s = settingsLoad();

    tzHtml = buildTzHtml(s.tzIndex);
    pTz = new WiFiManagerParameter(tzHtml.c_str());
    pWake = new WiFiManagerParameter("wake", "Wake time (HH:MM)", s.wake.c_str(), 5, "type='time'");
    pSleep = new WiFiManagerParameter("sleep", "Sleep time (HH:MM)", s.sleep.c_str(), 5, "type='time'");
    wm.addParameter(pTz); wm.addParameter(pWake); wm.addParameter(pSleep);
    wm.setSaveParamsCallback(onSave); wm.setConfigPortalTimeout(180);

    WiFi.mode(WIFI_STA);
    String name = "VerseDisplay-" + WiFi.macAddress().substring(12);
    name.replace(":", "");
    wm.setAPCallback([](WiFiManager* w) {
        displayShowSetup(w->getConfigPortalSSID().c_str());
    });
    if (!wm.autoConnect(name.c_str())) ESP.restart();
}