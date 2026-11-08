#pragma once
struct TzEntry { const char* label; const char* posix; };

static const TzEntry TZ_TABLE[] = {
    {"Pacific (Seattle, LA)",   "PST8PDT,M3.2.0,M11.1.0"},
    {"Mountain (Denver)",       "MST7MDT,M3.2.0,M11.1.0"},
    {"Arizona (Phoenix)",       "MST7"}, // No DST
    {"Central (Chicago)",       "CST6CDT,M3.2.0,M11.1.0"},
    {"Eastern (New York)",      "EST5EDT,M3.2.0,M11.1.0"},
    {"Alaska (Anchorage)",      "AKST9AKDT,M3.2.0,M11.1.0"},
    {"Hawaii (Honolulu)",       "HST10"},
    {"UK",                      "GMT0BST,M3.5.0/1,M10.5.0/2"},
    {"Central Europe",          "CET-1CEST,M3.5.0,M10.5.0/3"},
    {"UTC",                     "UTC0"}
};
inline constexpr int TZ_COUNT = sizeof(TZ_TABLE) / sizeof(TzEntry);