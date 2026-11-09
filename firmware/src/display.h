// display.h
#pragma once
#include <stddef.h>
#include <stdint.h>

void displayBegin();
void displaySetOn(bool on);
bool displayShowJpeg(const uint8_t* data, size_t len);

void displayShowStatus(const char* line1, const char* line2);
void displayShowSetup(const char* apName);