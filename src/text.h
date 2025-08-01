#pragma once

#include <stdbool.h>

#define MAX_LINES (4)
#define LINE_MAX (21)
#define VEDA_MAX (128)

#define VEDA_WIDTH (488)
#define VEDA_Y (329)
#define VEDA_HEIGHT (480 - VEDA_Y)

void setLine(int, const char*);
void clearLines(), textUpdate();

bool isVedaet();
void vedaem(float, const char*), stopVeda();
