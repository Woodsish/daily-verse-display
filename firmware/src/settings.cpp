// settings.cpp
#include "settings.h"
#include "tz_table.h"
#include <Preferences.h>

static Preferences prefs;

void settingsBegin() { prefs.begin("verse", false); }

Settings settingsLoad() {
    Settings s;
    s.tzIndex = constrain(prefs.getInt("tzi", 0), 0, TZ_COUNT - 1);
    s.wake = prefs.getString("wake", "07:00");
    s.sleep = prefs.getString("sleep", "22:00");
    return s;
}

void settingsSave(int tzIndex, const String& wake, const String& sleep) {
    prefs.putInt("tzi", tzIndex);
    prefs.putString("wake", wake);
    prefs.putString("sleep", sleep);
}