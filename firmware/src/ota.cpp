// ota.cpp
#include "ota.h"
#include "config.h"
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <time.h>

static int fetchLatestBuild() {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    http.begin(client, String(REPO_BASE) + "release/version.json");
    int build = -1;
    if (http.GET() == 200) {
        JsonDocument doc;
        if (!deserializeJson(doc, http.getString())) build  = doc["build"] | -1;
    }
    http.end();
    return build;
}

void otaCheckAndUpdate() {
    int latest = fetchLatestBuild();
    Serial.printf("OTA: running build %d, latest %d\n", FW_BUILD, latest);
    if (latest <= FW_BUILD) return;

    // Throttle: at most one update attempt per 30 minutes (needs a synced clock)
    Preferences prefs;
    prefs.begin("ota", false);
    uint32_t now = (uint32_t)time(nullptr);
    if (now - prefs.getULong("last", 0) < 1800) { prefs.end(); return; }
    prefs.putULong("last", now);
    prefs.end();

    WiFiClientSecure client;
    client.setInsecure();
    httpUpdate.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    t_httpUpdate_return r = 
        httpUpdate.update(client, String(REPO_BASE) + "release/firmware.bin");
    if (r == HTTP_UPDATE_FAILED)
        Serial.printf("OTA failed: %s\n", httpUpdate.getLastErrorString().c_str());
    // On success the device reboots into the new build by itself.
}