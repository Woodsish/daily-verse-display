// content.cpp
#include "content.h"
#include "config.h"
#include "display.h"
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

static String lastEtag;
static const int MAX_IMG = 200 * 1024;

bool displayShowJpeg(uint8_t* data, size_t len);

bool contentRefresh() {
    WiFiClientSecure client;
    client.setInsecure();
    HTTPClient http;
    const char* keys[] = {"ETag"};
    http.collectHeaders(keys, 1);
    http.begin(client, String(REPO_BASE) + "data/today.jpg");
    if (lastEtag.length()) http.addHeader("If-None-Match", lastEtag);

    int code = http.GET();
    if (code == 304) { http.end(); return false; }
    if (code != 200) {
        Serial.printf("Image HTTP %d\n", code);
        http.end();
        return false;
    }

    int len = http.getSize();
    if (len <= 0 || len > MAX_IMG) { http.end(); return false; }
    uint8_t* buf = (uint8_t*)malloc(len);
    if (!buf) { http.end(); return false; }

    WiFiClient* stream = http.getStreamPtr();
    int got = 0;
    uint32_t start = millis();
    while (got < len && millis() - start < 20000) {
        got += stream->readBytes(buf + got, len -got);
    }

    bool ok = (got == len && buf[0] == 0xFF && buf[1] == 0xD8); // JPEG magic bytes
    if (ok && displayShowJpeg(buf, len)) lastEtag = http.header("ETag");
    else ok = false;
    
    free(buf);
    http.end();
    return ok;
}