#pragma once

#include <stdbool.h>

#define MAX_LINES (4)
#define LINE_MAX (21)

void setLine(int, const char*);
void clearLines(), drawTextLines();

bool isVedaet();
void setVedaet(bool);
