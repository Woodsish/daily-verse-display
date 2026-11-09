// display.h
#pragma once
#include <stddef.h>
#include <stdint.h>

bool displayShowJpeg(const uint8_t* data, size_t len);

void displayBegin();
void displaySetOn(bool on);