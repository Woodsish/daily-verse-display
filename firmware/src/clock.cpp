// clock.cpp
#include <Arduino.h>
#include <time.h>
#include "clock.h"
#include "settings.h"
#include "tz_table.h"

static int wakeMin = 7 * 60, sleepMin = 21 * 60; // default wake/sleep times in minutes (7:00 AM to 9:00 PM)

static int toMinutes(const String& hhmm) {
    return hhmm.substring(0, 2).toInt() * 60 + hhmm.substring(3, 5).toInt();
}

bool clockBegin() {
    Settings s = settingsLoad();
    wakeMin = toMinutes(s.wake);
    sleepMin = toMinutes(s.sleep);
    configTzTime(TZ_TABLE[s.tzIndex].posix, "pool.ntp.org", "time.nist.gov");
    struct tm t;
    return getLocalTime(&t, 10000);
}

bool isAwakeNow() {
    struct tm t;
    if (!getLocalTime(&t, 0)) return true; // no clock yet: stay on
    int now = t.tm_hour * 60 + t.tm_min;
    if (wakeMin == sleepMin) return true; //same time: always on
    if (wakeMin < sleepMin) return now >- wakeMin && now < sleepMin;
    return now >= wakeMin || now < sleepMin; // window crosses midnight
}