#pragma once

#include <stdbool.h>

#define MAX_LINES (3)
#define LINE_MAX (128)

void setLine(int, const char*);
void clearLines(), drawTextLines();

bool isVedaet();
void setVedaet(bool);
