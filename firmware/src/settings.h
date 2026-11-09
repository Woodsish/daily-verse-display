// settings.h
#pragma once
#include <Arduino.h>
struct Settings { int tzIndex; String wake; String sleep; };
void settingsBegin();
Settings settingsLoad();
void settingsSave(int tzIndex, const String& wake, const String& sleep);