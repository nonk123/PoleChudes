#include <string.h>

#include "raylib.h"

#include "text.h"

extern Font font;
#define FONT_SIZE (16)
#define CELL_SIZE (18)

static char lines[MAX_LINES][LINE_MAX] = {0};

void setLine(int line, const char* text) {
	if (line >= 0 && line < MAX_LINES)
		strncpy(lines[line], text, LINE_MAX);
}

void clearLines() {
	for (size_t i = 0; i < MAX_LINES; i++)
		lines[i][0] = 0;
}

void drawTextLines() {
	for (size_t i = 0; i < MAX_LINES; i++) {
		const char* text = lines[i];
		const int y = 24 + i * (4 + CELL_SIZE) - i;

		for (size_t j = 0; j < strlen(text); j++) {
			if (text[j] == ' ')
				continue;

			const int x = 128 + j * CELL_SIZE - j;
			DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, BLACK);
			DrawRectangle(x + 1, y + 1, CELL_SIZE - 2, CELL_SIZE - 2, LIGHTGRAY);

			Vector2 pos = {.x = x + 5.0, .y = y + 2.0};
			DrawTextCodepoint(font, text[j], pos, FONT_SIZE, BLACK);
		}
	}
}

static bool vedaet = false;

bool isVedaet() {
	return vedaet;
}

void setVedaet(bool value) {
	vedaet = value;
}
